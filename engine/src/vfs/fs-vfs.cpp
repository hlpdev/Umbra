#include "Umbra/fs-vfs.hpp"
#include "Umbra/umbra.hpp"

#include <fstream>
#include <fmt/format.h>

using namespace std::string_literals;

umbra::VFSFSMount::VFSFSMount(const std::filesystem::path &directory, const vfs::permissions::VFSPermission permissions) : IVFSMount(permissions) {
  directory_ = directory;

  create_directories(directory_);
}

bool umbra::VFSFSMount::exists_s(const std::string_view virtual_path) const {
  return std::filesystem::exists(directory_ / virtual_path);
}

std::vector<uint8_t> umbra::VFSFSMount::read_s(const std::string_view virtual_path) const {
  std::ifstream file(directory_ / virtual_path);
  if (!file.is_open()) {
    umbra_fail("VFSFS: could not open file '"s + std::string(virtual_path) + "'");
  }

  const size_t file_size = std::filesystem::file_size(directory_ / virtual_path);

  std::vector<uint8_t> data(file_size);
  file.read(reinterpret_cast<char*>(data.data()), file_size);

  file.close();
  return data;
}

std::vector<std::string> umbra::VFSFSMount::list_s(const std::string_view virtual_path = {}) const {
  std::vector<std::string> list;
  for (const auto& entry : std::filesystem::directory_iterator(directory_ / virtual_path)) {
    if (entry.is_regular_file()) {
      list.push_back(entry.path().string());
    }
  }

  return list;
}

void umbra::VFSFSMount::write_s(const std::string_view virtual_path, const std::vector<uint8_t>& data) const {
  if (!exists_s(virtual_path)) {
    umbra_fail("VFSFS: path did not resolve to an existing file");
  }

  std::ofstream file(directory_ / virtual_path);
  file.write(reinterpret_cast<char*>(const_cast<unsigned char*>(data.data())), data.size());
  file.close();
}

void umbra::VFSFSMount::execute_s(std::string_view virtual_path, const std::shared_ptr<sol::state>& lua_state) const {
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

void umbra::VFSFSMount::create_s(const std::string_view virtual_path) const {
  if (exists_s(virtual_path)) {
    umbra_fail("VFSFS: a file already exists at the path '"s + std::string(virtual_path) + "'");
  }

  std::ofstream file(directory_ / virtual_path);
  if (!file.is_open()) {
    umbra_fail("VFSFS: failed to create file at the path '"s + std::string(virtual_path) + "'");
  }

  file.close();
}

void umbra::VFSFSMount::remove_s(const std::string_view virtual_path) const {
  if (!exists_s(virtual_path)) {
    umbra_fail("VFSFS: path did not resolve to an existing file");
  }

  std::filesystem::remove(directory_ / virtual_path);
}
