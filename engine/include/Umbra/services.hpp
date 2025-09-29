#pragma once

#include <ranges>
#include <sol/sol.hpp>

#include "Umbra/umbra.hpp"

using namespace std::string_literals;

namespace umbra {

  struct IService {

    virtual ~IService() = default;

  };

  class UMBRA_API ServiceRegistry final {
  public:

    explicit ServiceRegistry(const std::shared_ptr<sol::state>& lua_state) : lua_state_(lua_state) {}
    ~ServiceRegistry() = default;

    ServiceRegistry(const ServiceRegistry&) = delete;
    ServiceRegistry& operator=(const ServiceRegistry&) = delete;
    ServiceRegistry(ServiceRegistry&&) = delete;
    ServiceRegistry& operator=(ServiceRegistry&&) = delete;

    template<class T, class... Args>
    std::shared_ptr<T> register_service(const std::string& name, Args&&... args) {
      static_assert(std::is_base_of_v<IService, T>, "Registered services must inherit from IService");

      auto service = std::make_shared<T>(std::forward<Args>(args)...);
      services_.emplace(name, std::move(service));
      return service;
    }

    template<class T>
    std::shared_ptr<T> fetch_service(const std::string& name) const {
      static_assert(std::is_base_of_v<IService, T>, "Registered services must inherit from IService");

      const auto it = services_.find(name);
      return it == services_.end() ? nullptr : static_cast<T*>(it->second.get());
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<IService>> services_;

    std::shared_ptr<sol::state> lua_state_;
  };

}
