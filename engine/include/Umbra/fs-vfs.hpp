#pragma once

#include "Umbra/vfs.hpp"

#include <filesystem>

namespace umbra {

  UMBRA_API class VFSFSMount final : public IVFSMount {
  public:

    explicit VFSFSMount(const std::filesystem::path& directory, const vfs::permissions::VFSPermission permissions);

  protected:
    bool exists_s(std::string_view virtual_path) const override;

    std::vector<uint8_t> read_s(std::string_view virtual_path) const override;
    std::vector<std::string> list_s(std::string_view virtual_path) const override;

    void write_s(std::string_view virtual_path, std::vector<uint8_t> data) const override;

    void execute_s(std::string_view virtual_path) const override;

    void create_s(std::string_view virtual_path) const override;
    void remove_s(std::string_view virtual_path) const override;

    vfs::permissions::VFSPermission permission_;
    std::filesystem::path directory_;
  };

}