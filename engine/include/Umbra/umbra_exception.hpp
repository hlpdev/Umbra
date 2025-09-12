#pragma once

#include <stdexcept>

namespace umbra {

  class UMBRA_API UmbraException final : public std::runtime_error {
  public:
    explicit UmbraException(const std::string& message) : std::runtime_error(message) {}
  };

}