#include "Umbra/pak-vfs.hpp"

#include <fmt/format.h>

umbra::VFSPakMount::VFSPakMount(const std::filesystem::path &pak_path, const std::vector<uint8_t> &secret, const vfs::permissions::VFSPermission permissions) : IVFSMount(permissions) {
  reader_ = std::make_unique<PakReader>(pak_path, secret);
  all_ = reader_->list();
  std::ranges::sort(all_);
}

bool umbra::VFSPakMount::exists_s(std::string_view virtual_path) const {
  return std::ranges::binary_search(all_, std::string(virtual_path));
}

std::vector<uint8_t> umbra::VFSPakMount::read_s(std::string_view virtual_path) const {
  return reader_->read(std::string(virtual_path));
}

std::vector<std::string> umbra::VFSPakMount::list_s(std::string_view virtual_path) const {
  if (virtual_path.empty()) return all_;

  std::vector<std::string> out;
  auto directory = std::string(virtual_path);

  if (!directory.empty() && directory.back() != '/') {
    directory.push_back('/');
  }

  for (const std::string& path : all_) {
    if (path.starts_with(directory)) {
      out.push_back(path);
    }
  }

  return out;
}

void umbra::VFSPakMount::write_s(std::string_view virtual_path, const std::vector<uint8_t>&) const {
  umbra_fail("VFSPak: pak mounts do not support write");
}

void umbra::VFSPakMount::execute_s(std::string_view virtual_path, sol::state* lua_state) const {
  if (!exists_s(virtual_path)) {
    umbra_fail("VFSPak: path did not resolve to an existing file");
  }

  std::vector<uint8_t> script_bytes = read_s(virtual_path);
  const std::string script(script_bytes.begin(), script_bytes.end());

  const sol::protected_function_result result = lua_state->do_string(script, std::string(virtual_path), sol::load_mode::text);
  if (!result.valid()) {
    const sol::error err = result;
    umbra_fail(fmt::format("Lua: error in script '{}': {}", virtual_path, err.what()));
  }
}

void umbra::VFSPakMount::create_s(std::string_view virtual_path) const {
  umbra_fail("VFSPak: pak mounts do not support create");
}

void umbra::VFSPakMount::remove_s(std::string_view virtual_path) const {
  umbra_fail("VFSPak: pak mounts do not support remove");
}
