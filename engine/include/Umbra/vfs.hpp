#pragma once

#include "Umbra/umbra.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <sol/state.hpp>

namespace umbra {

  namespace vfs::permissions {
    enum VFSPermission : uint32_t {
      NONE = 0,
      READ = 1U << 0,
      WRITE = 1U << 1,
      CREATE = 1U << 2,
      REMOVE = 1U << 3,
      LIST = 1U << 4,
      EXECUTE = 1U << 5
    };

    constexpr VFSPermission operator|(const VFSPermission a, const VFSPermission b) noexcept {
      return static_cast<VFSPermission>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr VFSPermission operator&(const VFSPermission a, const VFSPermission b) noexcept {
      return static_cast<VFSPermission>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    constexpr VFSPermission operator^(const VFSPermission a, const VFSPermission b) noexcept {
      return static_cast<VFSPermission>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
    }

    constexpr VFSPermission operator~(const VFSPermission a) noexcept {
      return static_cast<VFSPermission>(~static_cast<uint32_t>(a));
    }

    inline VFSPermission& operator|=(VFSPermission& a, const VFSPermission b) noexcept { return a = (a | b); }
    inline VFSPermission& operator&=(VFSPermission& a, const VFSPermission b) noexcept { return a = (a & b); }
    inline VFSPermission& operator^=(VFSPermission& a, const VFSPermission b) noexcept { return a = (a ^ b); }

    constexpr bool has_all_permissions(const VFSPermission set, const VFSPermission required) noexcept {
      return (set & required) == required;
    }

    constexpr bool has_any_permission(const VFSPermission set, const VFSPermission any_of) noexcept {
      return static_cast<uint32_t>(set & any_of) != 0u;
    }
  }

  class UMBRA_API IVFSMount {
  public:

    explicit IVFSMount(vfs::permissions::VFSPermission permissions);
    virtual ~IVFSMount() = default;

    bool exists(std::string_view virtual_path) const;

    std::vector<uint8_t> read(std::string_view virtual_path) const;
    std::vector<std::string> list(std::string_view virtual_path) const;

    void create(std::string_view virtual_path) const;
    void remove(std::string_view virtual_path) const;
    void write(std::string_view virtual_path, const std::vector<uint8_t>& data) const;

    void execute(std::string_view virtual_path, sol::state* lua_state) const;

    constexpr vfs::permissions::VFSPermission permissions() const noexcept {
      return permissions_;
    };

  protected:
    virtual bool exists_s(std::string_view virtual_path) const = 0;

    virtual std::vector<uint8_t> read_s(std::string_view virtual_path) const = 0;
    virtual std::vector<std::string> list_s(std::string_view dir) const = 0;

    virtual void create_s(std::string_view virtual_path) const = 0;
    virtual void remove_s(std::string_view virtual_path) const = 0;
    virtual void write_s(std::string_view virtual_path, std::vector<uint8_t> data) const = 0;

    virtual void execute_s(std::string_view virtual_path, sol::state* lua_state) const = 0;

  private:
    vfs::permissions::VFSPermission permissions_;
  };

  class UMBRA_API VFS final {
  public:

    void mount(std::string prefix, std::unique_ptr<IVFSMount> mount);
    void unmount(std::string_view prefix) noexcept;

    bool exists(std::string_view virtual_path) const noexcept;
    std::vector<uint8_t> read(std::string_view virtual_path) const;
    std::vector<std::string> list(std::string_view virtual_path) const;

    void create(std::string_view virtual_path) const;
    void remove(std::string_view virtual_path) const;
    void write(std::string_view virtual_path, std::vector<uint8_t> data) const;

    void execute(std::string_view virtual_path) const;

    bool has_permission(std::string_view mount_prefix, vfs::permissions::VFSPermission permission) const noexcept;

    void set_lua_state(sol::state* lua_state);

  private:

    std::unordered_map<std::string, std::unique_ptr<IVFSMount>> mounts_;

    std::pair<const IVFSMount*, std::string> route(std::string_view virtual_path) const noexcept;

    sol::state* lua_state_;

  };

}
