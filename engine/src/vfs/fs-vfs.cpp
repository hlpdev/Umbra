#include "Umbra/fs-vfs.hpp"
#include "Umbra/umbra.hpp"

#include <fstream>

using namespace std::string_literals;

umbra::VFSFSMount::VFSFSMount(const std::filesystem::path &directory, const vfs::permissions::VFSPermission permissions) : IVFSMount(permissions) {
  directory_ = directory;

  create_directories(directory_);
}

bool umbra::VFSFSMount::exists_s(std::string_view virtual_path) const {
  return std::filesystem::exists(directory_ / virtual_path);
}

std::vector<uint8_t> umbra::VFSFSMount::read_s(std::string_view virtual_path) const {
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

std::vector<std::string> umbra::VFSFSMount::list_s(std::string_view virtual_path = {}) const {
  std::vector<std::string> list;
  for (const auto& entry : std::filesystem::directory_iterator(directory_ / virtual_path)) {
    if (entry.is_regular_file()) {
      list.push_back(entry.path().string());
    }
  }

  return list;
}

void umbra::VFSFSMount::write_s(std::string_view virtual_path, std::vector<uint8_t> data) const {
  if (!exists_s(virtual_path)) {
    umbra_fail("VFSFS: path did not resolve to an existing file");
  }

  std::ofstream file(directory_ / virtual_path);
  file.write(reinterpret_cast<char*>(data.data()), data.size());
  file.close();
}

void umbra::VFSFSMount::execute_s(std::string_view virtual_path) const {
  if (!exists_s(virtual_path)) {
    umbra_fail("VFSFS: path did not resolve to an existing file");
  }

  // TODO
  umbra_fail("VFSFS: execute not yet implemented for VFSFSMount");
}

void umbra::VFSFSMount::create_s(std::string_view virtual_path) const {
  if (exists_s(virtual_path)) {
    umbra_fail("VFSFS: a file already exists at the path '"s + std::string(virtual_path) + "'");
  }

  std::ofstream file(directory_ / virtual_path);
  if (!file.is_open()) {
    umbra_fail("VFSFS: failed to create file at the path '"s + std::string(virtual_path) + "'");
  }

  file.close();
}

void umbra::VFSFSMount::remove_s(std::string_view virtual_path) const {
  if (!exists_s(virtual_path)) {
    umbra_fail("VFSFS: path did not resolve to an existing file");
  }

  std::filesystem::remove(directory_ / virtual_path);
}
