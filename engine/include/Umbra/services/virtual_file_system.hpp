#pragma once

#include "Umbra/services.hpp"
#include "Umbra/engine_state.hpp"
#include "Umbra/types/data/file.hpp"

namespace umbra {

  class UMBRA_API VirtualFileSystemService final : public IService {
  public:

    VirtualFileSystemService(const VirtualFileSystemService&) = delete;
    VirtualFileSystemService& operator=(const VirtualFileSystemService&) = delete;
    VirtualFileSystemService(VirtualFileSystemService&&) = delete;
    VirtualFileSystemService& operator=(VirtualFileSystemService&&) = delete;

    explicit VirtualFileSystemService(EngineState* engine_state) : engine_state_(engine_state) {}
    ~VirtualFileSystemService() override = default;

    const char* name() override { return "VirtualFileSystem"; }

    bool exists(const std::string_view virtual_path) const {
      return engine_state_->vfs->exists(virtual_path);
    }

    File read(const std::string_view virtual_path) const {
      auto file = File(engine_state_->vfs->read(virtual_path));

      return std::move(file);
    }

    std::vector<std::string> list(const std::string_view virtual_path) const {
      return engine_state_->vfs->list(virtual_path);
    }

    void create(const std::string_view virtual_path) const {
      engine_state_->vfs->create(virtual_path);
    }

    void remove(const std::string_view virtual_path) const {
      engine_state_->vfs->remove(virtual_path);
    }

    void write(const std::string_view virtual_path, std::string_view data) const {
      engine_state_->vfs->write(virtual_path, std::vector<uint8_t>(data.begin(), data.end()));
    }

    void execute(const std::string_view virtual_path) const {
      engine_state_->vfs->execute(virtual_path);
    }

    void bind(sol::state& lua_state) {
      sol::usertype<VirtualFileSystemService> user_type = lua_state.new_usertype<VirtualFileSystemService>(name(),
        "exists", &VirtualFileSystemService::exists,
        "read", &VirtualFileSystemService::read,
        "list", &VirtualFileSystemService::list,
        "create", &VirtualFileSystemService::create,
        "remove", &VirtualFileSystemService::remove,
        "write", &VirtualFileSystemService::write,
        "execute", &VirtualFileSystemService::execute
      );
    }

  private:
    EngineState* engine_state_;
  };

}