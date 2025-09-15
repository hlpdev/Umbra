#pragma once

#include "Umbra/types.hpp"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>
#include <sol/sol.hpp>

namespace umbra {

  struct UMBRA_API StaticArray final : IType {
    std::vector<sol::object> data;

    const char* name() override { return "StaticArray"; }

    StaticArray() = default;

    StaticArray(const int count, const sol::this_state this_state) {
      const size_t n = count > 0 ? count : 0;
      data.reserve(n);
      for (size_t i = 0; i < n; i++) {
        data.emplace_back(make_object(this_state, sol::lua_nil));
      }
    }

    int size() const noexcept { return static_cast<int>(data.size()); }
    bool empty() const noexcept { return size() == 0; }

    sol::object get(const int index, const sol::this_state this_state) const noexcept {
      const int data_size = size();

      if (index < 1 || index > data_size) {
        return make_object(this_state, sol::lua_nil);
      }

      return data[index - 1];
    }

    void set(const int index, const sol::object& value) noexcept {
      const int data_size = size();

      if (index < 1 || index > data_size) {
        return;
      }

      data[index - 1] = value;
    }

    void fill(const sol::object &value) {
      for (auto& entry : data) {
        entry = value;
      }
    }

    static StaticArray from_table(const sol::table& table) {
      StaticArray out;
      const size_t data_size = table.size();
      out.data.reserve(data_size);

      for (size_t i = 1; i <= data_size; ++i) {
        out.data.emplace_back(table.get<sol::object>(i));
      }

      return out;
    }

    sol::as_table_t<std::vector<sol::object>> to_table() const {
      return as_table(data);
    }

    bool equals(const StaticArray& other, const sol::this_state this_state) const {
      if (data.size() != other.data.size()) {
        return false;
      }

      sol::state_view state_view(this_state);
      const sol::function raw_equal = state_view["rawequal"];

      for (size_t i = 0; i < data.size(); ++i) {
        if (raw_equal.valid()) {
          if (!raw_equal(data[i], other.data[i])) {
            return false;
          }
        } else {
          if (data[i] != other.data[i]) {
            return false;
          }
        }
      }

      return true;
    }

    std::string to_string(const sol::this_state this_state) const {
      sol::state_view state_view(this_state);
      const sol::function to_string = state_view["tostring"];
      std::ostringstream string_stream;

      string_stream << "StaticArray(";
      for (size_t i = 0; i < data.size(); ++i) {
        if (i) {
          string_stream << ", ";
        }

        if (data[i].is<std::string>()) {
          string_stream << '\'' << data[i].as<std::string>() << '\'';
          continue;
        }

        if (to_string.valid()) {
          sol::object value = to_string(data[i]);
          if (value.is<std::string>()) {
            string_stream << value.as<std::string>();
          } else {
            string_stream << "<obj>";
          }
        } else {
          string_stream << "<obj>";
        }
      }
      string_stream << ")";

      return string_stream.str();
    }

    void bind(sol::state& lua_state) {
      sol::usertype<StaticArray> user_type = lua_state.new_usertype<StaticArray>(name(),
        sol::constructors<StaticArray(), StaticArray(int, sol::this_state)>(),

        "size", &StaticArray::size,
        "empty", &StaticArray::empty,
        "get", &StaticArray::get,
        "set", &StaticArray::set,
        "fill", &StaticArray::fill,
        "from_table", &StaticArray::from_table,
        "to_table", &StaticArray::to_table
      );

      user_type[sol::meta_function::length] = [](const StaticArray& static_array) { return static_array.size(); };

      user_type[sol::meta_function::equal_to] = [](const StaticArray& first, const StaticArray& second, const sol::this_state this_state) {
        return first.equals(second, this_state);
      };

      user_type[sol::meta_function::to_string] = [](const StaticArray& static_array, const sol::this_state this_state) {
        return static_array.to_string(this_state);
      };

      user_type[sol::meta_function::index] = [](const StaticArray& static_array, const sol::stack_object key, const sol::this_state this_state) {
        if (key.is<int>()) {
          const int index = key.as<int>();
          return static_array.get(index, this_state);
        }

        if (key.is<double>()) {
          const int index = static_cast<int>(key.as<double>());
          return static_array.get(index, this_state);
        }

        return make_object(this_state, sol::lua_nil);
      };

      user_type[sol::meta_function::new_index] = [](StaticArray& static_array, const sol::stack_object key, const sol::stack_object value) {
        constexpr auto to_int = [&](const sol::stack_object k) -> std::optional<int> {
          if (k.is<int>()) {
            return k.as<int>();
          }
          if (k.is<double>()) {
            return static_cast<int>(k.as<double>());
          }
          return std::nullopt;
        };

        if (const std::optional<int> optional_index = to_int(key)) {
          const int index = *optional_index;
          const auto new_value = sol::object(value);
          static_array.set(index, std::move(new_value));
        }
      };
    }

  };

}