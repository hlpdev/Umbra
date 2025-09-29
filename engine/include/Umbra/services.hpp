#pragma once

#include <ranges>
#include <sol/sol.hpp>

#include "Umbra/umbra.hpp"

using namespace std::string_literals;

namespace umbra {
  struct IService {

    virtual ~IService() = default;
    virtual const char* name() = 0;

  };

  namespace services_concepts {
    template<class T>
    concept has_static_bind_name = requires(sol::state& lua_state, const char* name) {
      { T::bind(lua_state, name) } -> std::same_as<void>;
    };

    template<class T>
    concept has_static_bind = requires(sol::state& lua_state) {
      { T::bind(lua_state) } -> std::same_as<void>;
    };

    template<class T>
    concept has_member_bind = requires(T t, sol::state& lua_state) {
      { t.bind(lua_state) } -> std::same_as<void>;
    };

    template<class T>
    inline constexpr bool always_false_v = false;
  }

  class UMBRA_API ServiceRegistry final {
  public:

    explicit ServiceRegistry(const std::shared_ptr<sol::state>& lua_state) : lua_state_(lua_state) {}
    ~ServiceRegistry() = default;

    ServiceRegistry(const ServiceRegistry&) = delete;
    ServiceRegistry& operator=(const ServiceRegistry&) = delete;
    ServiceRegistry(ServiceRegistry&&) = delete;
    ServiceRegistry& operator=(ServiceRegistry&&) = delete;

    template<class T, class... Args>
    bool register_service(Args&&... args) {
      static_assert(std::is_base_of_v<IService, T>, "Registered services must inherit from IService");

      auto instance = std::make_shared<T>(std::forward<Args>(args)...);

      const char* raw_name = instance->name();
      const std::string type_name = (raw_name && *raw_name) ? raw_name : typeid(T).name();

      if (services_.contains(type_name)) {
        return false;
      }

      sol::state& lua_state = *lua_state_;

      if constexpr (services_concepts::has_static_bind_name<T>) {
        T::bind(lua_state, type_name.c_str());
      } else if constexpr (services_concepts::has_static_bind<T>) {
        T::bind(lua_state);
      } else if constexpr (services_concepts::has_member_bind<T>) {
        instance->bind(lua_state);
      } else {
        static_assert(services_concepts::always_false_v<T>,
          "Service must provide one of:\n"
          "   static void bind(sol::state& lua_state, const char* name);\n"
          "   static void bind(sol::state& lua_state);\n"
          "   void bind(sol::state& lua_state);"
        );
      }

      sol::object lua_instance = make_object(*lua_state_, instance);

      services_.emplace(type_name, instance);
      lua_services_.emplace(type_name, std::move(lua_instance));
      return true;
    }

    template<class T>
    std::shared_ptr<T> fetch_service(const std::string& name) {
      static_assert(std::is_base_of_v<IService, T>, "Registered services must inherit from IService");

      const auto it = services_.find(name);
      return it == services_.end() ? nullptr : it->second;
    }

    sol::object fetch_service_lua(const std::string& name) {
      const auto it = lua_services_.find(name);
      return it == lua_services_.end() ? make_object(*lua_state_, sol::nil) : it->second;
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<IService>> services_;
    std::unordered_map<std::string, sol::object> lua_services_;
    const std::shared_ptr<sol::state> lua_state_;
  };

}
