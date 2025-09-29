#pragma once

#include "Umbra/types.hpp"

namespace umbra {

  struct UMBRA_API File final : IType {
    std::vector<uint8_t> data;

    const char* name() override { return "File"; }

    explicit File(const std::vector<uint8_t>& data) noexcept : data(data) {}
    File() noexcept {}

    size_t size() const noexcept {
      return data.size();
    }

    std::string as_string() const noexcept {
      return std::string(data.begin(), data.end());
    }

    void bind(sol::state& lua_state) {
      sol::usertype<File> user_type = lua_state.new_usertype<File>(name(),
        "size", &File::size,
        "as_string", &File::as_string
      );
    }
  };

}