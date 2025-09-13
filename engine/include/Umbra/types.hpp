#pragma once

#include "Umbra/umbra.hpp"

#include <memory>
#include <unordered_set>
#include <sol/sol.hpp>

namespace umbra {

  struct IType {

    virtual ~IType() = default;
    virtual const char* name() = 0;

  };

  template<class T>
  concept has_static_bind_name = requires(sol::state& lua_state, const char* n) {
    { T::bind(lua_state, n) } -> std::same_as<void>;
  };

  template<class T>
  concept has_static_bind = requires(sol::state& lua_state) {
    { T::bind(lua_state) } -> std::same_as<void>;
  };

  template<class T>
  concept has_member_bind = requires(T t, sol::state& lua_state) {
    { t.bind(lua_state) } -> std::same_as<void>;
  };

  template<class>
  inline constexpr bool always_false_v = false;

  class UMBRA_API TypeRegistry final {
  public:

    explicit TypeRegistry(const std::shared_ptr<sol::state>& lua_state) : lua_state_(lua_state) {}
    ~TypeRegistry() = default;

    TypeRegistry(const TypeRegistry&) = delete;
    TypeRegistry& operator=(const TypeRegistry&) = delete;
    TypeRegistry(TypeRegistry&&) = delete;
    TypeRegistry& operator=(TypeRegistry&&) = delete;

    template<class T, class... Args>
    bool register_type(Args&&... args) {
      static_assert(std::is_base_of_v<IType, T>, "Registered types must inherit from IType");

      T instance(std::forward<Args>(args)...);
      const char* raw_name = instance.name();
      const std::string type_name = (raw_name && *raw_name) ? raw_name : typeid(T).name();

      if (!mark_once(type_name)) {
        return false;
      }

      sol::state& lua_state = *lua_state_;

      if constexpr (has_static_bind_name<T>) {
        T::bind(lua_state, type_name.c_str());
      } else if constexpr (has_static_bind<T>) {
        T::bind(lua_state);
      } else if constexpr (has_member_bind<T>) {
        instance.bind(lua_state);
      } else {
        static_assert(always_false_v<T>,
          "Type must provide one of:\n"
          "   static void bind(sol::state& lua_state, const char* name);\n"
          "   static void bind(sol::state& lua_state);\n"
          "   void bind(sol::state& lua_state);"
        );
      }

      return true;
    }

  private:
    bool mark_once(const std::string_view name) {
      auto [it, inserted] = registered_types_.insert(std::string(name));
      if (!inserted) {
        return false;
      }

      std::string sentinel = "__usertype_" + std::string(name);
      const sol::object sentinel_obj = (*lua_state_)[sentinel];
      if (!sentinel_obj.valid()) {
        (*lua_state_)[sentinel] = true;
      }

      return true;
    }

    std::unordered_set<std::string> registered_types_;

    std::shared_ptr<sol::state> lua_state_;

  };

}