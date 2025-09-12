#include "Umbra/pak.hpp"

#include <cstring>
#include <fstream>
#include <unordered_map>
#include <zstd.h>

static void derive_key(std::vector<uint8_t>& out_key, const uint8_t salt[16], const std::vector<uint8_t>& secret) {
  out_key.resize(crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
  if (crypto_pwhash(out_key.data(), out_key.size(), reinterpret_cast<const char*>(secret.data()), secret.size(), salt, crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE, crypto_pwhash_ALG_DEFAULT) != 0) {
    umbra::umbra_fail("PakReader: pak key derivation failed");
  }
}

umbra::PakReader::PakReader(const std::filesystem::path &path, const std::vector<uint8_t> &secret) {
  if (sodium_init() < 0) {
    umbra_fail("PakReader: failed to initialize");
  }

  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    umbra_fail("PakReader: failed to open file");
  }

  pak_file.path = path;

  PakHeader header{};
  file.read(reinterpret_cast<char*>(&header), sizeof(header));
  if (!file || std::memcmp(header.magic, PAK_MAGIC, sizeof(header.magic)) != 0) {
    umbra_fail("PakReader: invalid pak file");
  }

  pak_file.header = header;
  derive_key(pak_file.key, header.salt, secret);

  pak_file.entries.reserve(header.file_count);
  for (uint32_t i = 0; i < header.file_count; i++) {
    PakEntryFixed fixed_entry{};

    file.read(reinterpret_cast<char*>(&fixed_entry), sizeof(fixed_entry));
    if (!file) {
      umbra_fail("PakReader: failed to read fixed entry");
    }

    std::string virtual_path; virtual_path.resize(fixed_entry.path_length);
    file.read(virtual_path.data(), fixed_entry.path_length);
    if (!file) {
      umbra_fail("PakReader: failed to read virtual path");
    }

    PakEntry entry{};
    entry.offset = fixed_entry.offset;
    entry.cipher_size = fixed_entry.cipher_size;
    entry.raw_size = fixed_entry.raw_size;
    entry.nonce.assign(fixed_entry.nonce, fixed_entry.nonce + sizeof(fixed_entry.nonce));
    entry.path = std::move(virtual_path);

    pak_file.index[entry.path] = pak_file.entries.size();
    pak_file.entries.push_back(entry);
  }
}

bool umbra::PakReader::contains(const std::string& virtual_path) const {
  return pak_file.index.contains(virtual_path);
}

std::vector<uint8_t> umbra::PakReader::read(const std::string& virtual_path) const {
  const auto it = pak_file.index.find(virtual_path);
  if (it == pak_file.index.end()) {
    umbra_fail("PakReader: path '" + virtual_path + "' not found");
  }

  const PakEntry& entry = pak_file.entries.at(it->second);

  std::ifstream file(pak_file.path, std::ios::binary);
  if (!file) {
    umbra_fail("PakReader: failed to open file");
  }

  file.seekg(entry.offset, std::ios::beg);
  std::vector<uint8_t> cipher(entry.cipher_size);

  file.read(reinterpret_cast<char*>(cipher.data()), entry.cipher_size);
  if (!file) {
    umbra_fail("PakReader: failed to read cipher");
  }

  const auto ad = reinterpret_cast<const uint8_t*>(entry.path.data());
  const uint64_t ad_len = entry.path.size();

  if (entry.cipher_size < crypto_aead_xchacha20poly1305_ietf_ABYTES) {
    umbra_fail("PakReader: bad cipher size");
  }

  const size_t compressed_capacity = entry.cipher_size - crypto_aead_xchacha20poly1305_ietf_ABYTES;
  std::vector<uint8_t> compressed(compressed_capacity);

  size_t compressed_size = 0;
  if (crypto_aead_xchacha20poly1305_ietf_decrypt(
    compressed.data(), &compressed_size, nullptr,
    cipher.data(), cipher.size(),
    ad, ad_len,
    entry.nonce.data(),
    pak_file.key.data()
  ) != 0) {
    umbra_fail("PakReader: pak decryption failed");
  }

  compressed.resize(compressed_size);

  std::vector<uint8_t> out(entry.raw_size);
  const size_t result = ZSTD_decompress(out.data(), out.size(), compressed.data(), compressed_size);
  if (ZSTD_isError(result) || result != out.size()) {
    umbra_fail("PakReader: decompression failed");
  }

  return out;
}

std::vector<std::string> umbra::PakReader::list() const {
  std::vector<std::string> out(pak_file.entries.size());
  for (PakEntry entry : pak_file.entries) {
    out.push_back(entry.path);
  }

  return out;
}
