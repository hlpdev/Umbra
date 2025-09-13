#pragma once

#include "Umbra/umbra.hpp"

#include <unordered_set>
#include <sol/sol.hpp>

namespace umbra {

  class UMBRA_API IBuiltin {
  public:

    virtual ~IBuiltin() = default;
    virtual void register_builtin(const std::shared_ptr<sol::state>& lua_state) = 0;

  };

  class UMBRA_API BuiltinRegistry final {
  public:

    explicit BuiltinRegistry(const std::shared_ptr<sol::state>& lua_state) : lua_state_(lua_state) {}
    ~BuiltinRegistry() = default;

    BuiltinRegistry(const BuiltinRegistry&) = delete;
    BuiltinRegistry& operator=(const BuiltinRegistry&) = delete;
    BuiltinRegistry(BuiltinRegistry&&) = delete;
    BuiltinRegistry& operator=(BuiltinRegistry&&) = delete;

    template<class T, class... Args>
    bool register_builtin(const std::string& name, Args&&... args) {
      static_assert(std::is_base_of_v<IBuiltin, T>, "Registered builtins must inherit from IBuiltin");

      auto builtin = std::make_shared<T>(std::forward<Args>(args)...);

      if (registered_builtins_.contains(name)) {
        return false;
      }

      builtin->register_builtin(lua_state_);

      registered_builtins_.insert(name);
      return true;
    }

  private:
    std::unordered_set<std::string> registered_builtins_;

    std::shared_ptr<sol::state> lua_state_;
  };
}