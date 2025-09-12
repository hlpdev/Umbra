#include "Umbra/vfs.hpp"
#include "Umbra/umbra.hpp"

umbra::IVFSMount::IVFSMount(const vfs::permissions::VFSPermission permissions) {
  permissions_ = permissions;
}

bool umbra::IVFSMount::exists(std::string_view virtual_path) const {
  if (!has_all_permissions(permissions(), vfs::permissions::READ)) {
    umbra_fail("VFS: insufficient read permissions");
  }

  return exists_s(virtual_path);
}

std::vector<uint8_t> umbra::IVFSMount::read(std::string_view virtual_path) const {
  if (!has_all_permissions(permissions(), vfs::permissions::READ)) {
    umbra_fail("VFS: insufficient read permissions");
  }

  return read_s(virtual_path);
}

std::vector<std::string> umbra::IVFSMount::list(std::string_view virtual_path) const {
  if (!has_all_permissions(permissions(), vfs::permissions::LIST)) {
    umbra_fail("VFS: insufficient list permissions");
  }

  return list_s(virtual_path);
}

void umbra::IVFSMount::write(const std::string_view virtual_path, const std::vector<uint8_t>& data) const {
  if (!has_all_permissions(permissions(), vfs::permissions::WRITE)) {
    umbra_fail("VFS: insufficient write permissions");
  }

  write_s(virtual_path, data);
}

void umbra::IVFSMount::execute(const std::string_view virtual_path, const std::shared_ptr<sol::state>& lua_state) const {
  if (!has_all_permissions(permissions(), vfs::permissions::EXECUTE)) {
    umbra_fail("VFS: insufficient execute permissions");
  }

  execute_s(virtual_path, lua_state);
}

void umbra::IVFSMount::create(std::string_view virtual_path) const {
  if (!has_all_permissions(permissions(), vfs::permissions::CREATE)) {
    umbra_fail("VFS: insufficient create permissions");
  }

  create_s(virtual_path);
}

void umbra::IVFSMount::remove(std::string_view virtual_path) const {
  if (!has_all_permissions(permissions(), vfs::permissions::REMOVE)) {
    umbra_fail("VFS: insufficient remove permissions");
  }

  remove_s(virtual_path);
}

umbra::VFS::VFS(const std::shared_ptr<sol::state> &lua_state) : lua_state_(lua_state) {}

void umbra::VFS::mount(std::string prefix, std::unique_ptr<IVFSMount> mount) {
  if (prefix.empty() || !prefix.ends_with("://")) {
    umbra_fail("VFS: mount prefix must end with '://'");
  }

  mounts_[std::move(prefix)] = std::move(mount);
}

void umbra::VFS::unmount(std::string_view prefix) noexcept {
  mounts_.erase(std::string(prefix));
}

bool umbra::VFS::exists(std::string_view virtual_path) const noexcept {
  auto [mount, sub] = route(virtual_path);
  return mount && mount->exists(sub);
}

std::vector<uint8_t> umbra::VFS::read(std::string_view virtual_path) const {
  auto [mount, sub] = route(virtual_path);
  if (!mount) {
    umbra_fail("VFS: mount not found");
  }

  return mount->read(sub);
}

std::vector<std::string> umbra::VFS::list(std::string_view virtual_path) const {
  auto [mount, sub] = route(virtual_path);
  if (!mount) {
    umbra_fail("VFS: mount not found");
  }

  return mount->list(sub);
}

void umbra::VFS::create(std::string_view virtual_path) const {
  auto [mount, sub] = route(virtual_path);
  if (!mount) {
    umbra_fail("VFS: mount not found");
  }

  return mount->create(sub);
}

void umbra::VFS::remove(std::string_view virtual_path) const {
  auto [mount, sub] = route(virtual_path);
  if (!mount) {
    umbra_fail("VFS: mount not found");
  }

  return mount->remove(sub);
}


void umbra::VFS::write(const std::string_view virtual_path, const std::vector<uint8_t>& data) const {
  auto [mount, sub] = route(virtual_path);
  if (!mount) {
    umbra_fail("VFS: mount not found");
  }

  mount->write(sub, data);
}

void umbra::VFS::execute(std::string_view virtual_path) const {
  auto [mount, sub] = route(virtual_path);
  if (!mount) {
    umbra_fail("VFS: mount not found");
  }

  return mount->execute(sub, lua_state_);
}

std::pair<const umbra::IVFSMount *, std::string> umbra::VFS::route(std::string_view virtual_path) const noexcept {
  for (auto const& [prefix, mount] : mounts_) {
    if (virtual_path.starts_with(prefix)) {
      std::string sub;
      if (virtual_path.size() > prefix.size() && virtual_path[prefix.size()] == '/') {
        sub = std::string(virtual_path.substr(prefix.size() + 1));
      } else {
        sub = std::string(virtual_path.substr(prefix.size()));
      }

      return { mount.get(), std::move(sub) };
    }
  }

  return { nullptr, {} };
}

bool umbra::VFS::has_permission(std::string_view mount_prefix, vfs::permissions::VFSPermission permission) const noexcept {
  auto [mount, _] = route(mount_prefix);
  if (!mount) {
    umbra_fail("VFS: mount not found");
  }

  return has_all_permissions(mount->permissions(), permission);
}
