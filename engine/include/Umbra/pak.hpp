#pragma once

#include "Umbra/umbra.hpp"

#include <cstdint>
#include <filesystem>
#include <sodium.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace umbra {
  constexpr auto PAK_MAGIC = "UMBRAPAK\0";
  constexpr size_t PAK_MAGIC_LEN = sizeof(PAK_MAGIC); // NOLINT(*-sizeof-expression)

  struct PakEntry {
    uint64_t offset;
    uint64_t cipher_size;
    uint64_t raw_size;

    std::vector<uint8_t> nonce;

    std::string path;
  };

  struct PakEntryFixed {
    uint64_t offset;
    uint64_t cipher_size;
    uint64_t raw_size;

    uint8_t nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];

    uint32_t path_length;
  };

  struct PakHeader {
    char magic[PAK_MAGIC_LEN];

    uint32_t version;
    uint32_t file_count;

    uint8_t salt[16];
  };

  struct PakFile {
    std::filesystem::path path;
    PakHeader header;
    std::vector<PakEntry> entries;
    std::unordered_map<std::string, size_t> index;
    std::vector<uint8_t> key;
  };

  struct PendingItem {
    std::filesystem::path disk_path;
    std::string virtual_path;
    std::vector<uint8_t> cipher;
    std::vector<uint8_t> nonce;
    uint64_t raw_size;
  };

  class UMBRA_API PakReader {
  public:

    PakReader(const std::filesystem::path& path, const std::vector<uint8_t>& secret);

    bool contains(const std::string& virtual_path) const;

    std::vector<uint8_t> read(const std::string& virtual_path) const;
    std::vector<std::string> list() const;

  private:
    PakFile pak_file;
  };

  class UMBRA_API PakWriter {
  public:

    PakWriter(const std::filesystem::path& out_file, const std::vector<uint8_t>& secret, const std::filesystem::path& virtual_base);
    ~PakWriter();

    void add_file(const std::filesystem::path& disk_path, const std::filesystem::path& virtual_override = {});
    void add_tree(const std::filesystem::path& directory_path);

  private:
    std::filesystem::path out_file;
    std::filesystem::path virtual_base;
    std::vector<uint8_t> secret;
    std::vector<uint8_t> key;
    uint8_t salt[16];

    std::vector<PendingItem> items;
  };
}
