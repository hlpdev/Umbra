#pragma

#include <ranges>

#include "Umbra/umbra.hpp"

#include <sol/sol.hpp>

using namespace std::string_literals;

namespace umbra {

  struct IService {
    virtual ~IService() = default;
  };

  class UMBRA_API ServiceRegistry final {
  public:

    template<class T, class... Args>
    T* emplace(const std::string& name, Args&&... args) {
      static_assert(std::is_base_of_v<IService, T>, "Registered services must inherit from IService");

      auto service = std::make_unique<T>(std::forward<Args>(args)...);
      auto* raw = service.get();
      services_[name] = std::move(service);
      return raw;
    }

    template<class T>
    T* get(const std::string& name) const {
      static_assert(std::is_base_of_v<IService, T>, "Registered services must inherit from IService");

      const auto it = services_.find(name);
      return it == services_.end() ? nullptr : static_cast<T*>(it->second.get());
    }

  private:
    std::unordered_map<std::string, std::unique_ptr<IService>> services_;
  };

}
