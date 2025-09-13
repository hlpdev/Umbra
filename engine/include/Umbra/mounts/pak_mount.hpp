#pragma once

#include "Umbra/pak.hpp"
#include "Umbra/vfs.hpp"

namespace umbra {

  class UMBRA_API VFSPakMount final : public IVFSMount {
  public:

    explicit VFSPakMount(const std::filesystem::path& pak_path, const std::vector<uint8_t>& secret, vfs::permissions::VFSPermission permissions);

  protected:
    bool exists_s(std::string_view virtual_path) const override;

    std::vector<uint8_t> read_s(std::string_view virtual_path) const override;
    std::vector<std::string> list_s(std::string_view virtual_path) const override;

    void write_s(std::string_view virtual_path, const std::vector<uint8_t>& data) const override;

    void execute_s(std::string_view virtual_path, const std::shared_ptr<sol::state>& lua_state) const override;

    void create_s(std::string_view virtual_path) const override;
    void remove_s(std::string_view virtual_path) const override;

    std::unique_ptr<PakReader> reader_;
    std::vector<std::string> all_;
  };

}