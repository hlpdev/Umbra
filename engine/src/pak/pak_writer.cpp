#include "Umbra/pak.hpp"

#include <fstream>
#include <zstd.h>
#include <cstring>

static std::vector<uint8_t> read_all(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    umbra::umbra_fail("PakWriter: failed to open file");
  }

  file.seekg(0, std::ios::end);
  const int64_t length = file.tellg();

  file.seekg(0, std::ios::beg);
  std::vector<uint8_t> out(length);

  if (length) {
    file.read(reinterpret_cast<char*>(out.data()), length);
  }

  return out;
}

static std::vector<std::filesystem::path> walk_files(const std::filesystem::path& root) {
  std::vector<std::filesystem::path> out;

  if (!exists(root)) return out;

  for (auto& entry : std::filesystem::directory_iterator(root)) {
    if (entry.is_regular_file()) {
      out.push_back(entry.path());
    }
  }

  return out;
}

static std::vector<uint8_t> zstd_compress(const std::vector<uint8_t>& in) {
  const size_t bound = ZSTD_compressBound(in.size());
  std::vector<uint8_t> out(bound);

  const size_t result = ZSTD_compress(out.data(), bound, in.data(), in.size(), 3);
  if (ZSTD_isError(result)) {
    umbra::umbra_fail("PakWriter: failed to compress data");
  }

  out.resize(result);
  return out;
}

static void derive_key(std::vector<uint8_t>& out_key, const uint8_t salt[16], const std::vector<uint8_t>& secret) {
  out_key.resize(crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
  if (crypto_pwhash(out_key.data(), out_key.size(), reinterpret_cast<const char*>(secret.data()), secret.size(), salt, crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE, crypto_pwhash_ALG_DEFAULT) != 0) {
    umbra::umbra_fail("PakReader: pak key derivation failed");
  }
}

umbra::PakWriter::PakWriter(const std::filesystem::path &out_file, const std::vector<uint8_t> &secret, const std::filesystem::path &virtual_base) : out_file(out_file), virtual_base(virtual_base), secret(secret) {
  if (sodium_init() < 0) {
    umbra_fail("PakWriter: failed to initialize sodium");
  }

  randombytes_buf(salt, sizeof(salt));
  derive_key(key, salt, secret);
}

umbra::PakWriter::~PakWriter() {
  const uint32_t file_count = static_cast<uint32_t>(items.size());
  constexpr uint64_t hdr_size = sizeof(PakHeader);

  uint64_t index_size = 0;
  for (PendingItem item : items) {
    index_size += sizeof(PakEntryFixed) + item.virtual_path.size();
  }

  const uint64_t data_start = hdr_size + index_size;

  std::vector<PakEntryFixed> fixed_entries;
  fixed_entries.reserve(file_count);

  uint64_t current = data_start;
  for (PendingItem item : items) {
    PakEntryFixed fixed{};
    fixed.offset = current;
    fixed.cipher_size = item.cipher.size();
    fixed.raw_size = item.raw_size;
    std::memcpy(fixed.nonce, item.nonce.data(), item.nonce.size());
    fixed.path_length = static_cast<uint32_t>(item.virtual_path.size());

    fixed_entries.push_back(fixed);

    current += item.cipher.size();
  }

  std::ofstream out(out_file, std::ios::binary);
  if (!out) {
    umbra_fail("PakWriter: failed to open file");
  }

  PakHeader header;
  std::memcpy(header.magic, PAK_MAGIC, sizeof(header.magic));
  header.version = UMBRA_VERSION;
  header.file_count = file_count;
  std::memcpy(header.salt, salt, 16);

  out.write(reinterpret_cast<const char*>(&header), sizeof(header));
  for (size_t i = 0; i < items.size(); ++i) {
    out.write(reinterpret_cast<const char*>(&fixed_entries[i]), sizeof(PakEntryFixed));
    out.write(items[i].virtual_path.data(), fixed_entries[i].path_length);
  }

  for (PendingItem item : items) {
    out.write(reinterpret_cast<const char*>(item.cipher.data()), static_cast<std::streamsize>(item.cipher.size()));
  }
}

void umbra::PakWriter::add_file(const std::filesystem::path &disk_path, const std::filesystem::path &virtual_override) {
  PendingItem item{};
  item.disk_path = disk_path;
  item.virtual_path = virtual_override.empty() ? relative(disk_path, virtual_base).generic_string() : virtual_override.generic_string();

  const std::vector<uint8_t> raw = read_all(disk_path);
  item.raw_size = raw.size();

  const std::vector<uint8_t> compressed = zstd_compress(raw);

  item.nonce.resize(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
  randombytes_buf(item.nonce.data(), item.nonce.size());

  const auto ad = reinterpret_cast<const uint8_t*>(item.virtual_path.data());
  const uint64_t ad_len = item.virtual_path.size();

  std::vector<uint8_t> cipher(compressed.size() + crypto_aead_xchacha20poly1305_ietf_ABYTES);
  unsigned long long cipher_length = 0;

  crypto_aead_xchacha20poly1305_ietf_encrypt(
    cipher.data(), &cipher_length,
    compressed.data(), compressed.size(),
    ad, ad_len,
    nullptr,
    item.nonce.data(),
    key.data()
  );

  cipher.resize(cipher_length);
  item.cipher = std::move(cipher);

  items.push_back(std::move(item));
}

void umbra::PakWriter::add_tree(const std::filesystem::path &directory_path) {
  for (std::filesystem::path& path : walk_files(directory_path)) {
    add_file(path);
  }
}
