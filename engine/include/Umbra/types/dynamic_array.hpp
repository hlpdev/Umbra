#pragma once

#include "Umbra/types.hpp"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <sol/sol.hpp>

namespace umbra {

  struct UMBRA_API DynamicArray final : IType {
    std::vector<sol::object> data;

    const char* name() override { return "DynamicArray"; }

    DynamicArray() noexcept = default;

    int size() const noexcept { return static_cast<int>(data.size()); }
    int capacity() const noexcept { return static_cast<int>(data.capacity()); }
    bool empty() const noexcept { return data.empty(); }

    void clear() noexcept { data.clear(); }
    void reserve(const int size) { data.reserve(std::abs(size)); }
    void shrink_to_fit() { data.shrink_to_fit(); }

    sol::object get(const int index, sol::this_state this_state) const noexcept {
      if (index < 1 || index > size()) {
        return make_object(this_state, sol::lua_nil);
      }

      return data[static_cast<size_t>(index - 1)];
    }

    void set(const int index, sol::object value) noexcept {
      if (index < 1 || index > size()) {
        return;
      }

      data[static_cast<size_t>(index - 1)] = std::move(value);
    }

    void push_back(sol::object value) { data.emplace_back(std::move(value)); }
    void push_front(sol::object value) { data.emplace(data.begin(), std::move(value)); }

    sol::object pop_back(const sol::this_state this_state) noexcept {
      if (data.empty()) {
        return make_object(this_state, sol::lua_nil);
      }

      sol::object value = std::move(data.back());
      data.pop_back();
      return value;
    }

    sol::object pop_front(const sol::this_state this_state) noexcept {
      if (data.empty()) {
        return make_object(this_state, sol::lua_nil);
      }

      sol::object value = std::move(data.front());
      data.erase(data.begin());
      return value;
    }

    void insert(const int index, sol::object value) {
      const size_t data_size = size();

      if (index >= 1 && index <= static_cast<int>(data_size + 1)) {
        data.insert(data.begin() + (index - 1), std::move(value));
      } else {
        data.emplace_back(std::move(value));
      }
    }

    void erase(const int index) noexcept {
      const size_t data_size = size();
      if (index < 1 || index > data_size) {
        return;
      }

      data.erase(data.begin() + (index - 1));
    }

    DynamicArray concat(const DynamicArray& other) const noexcept {
      DynamicArray out;
      out.data.reserve(data.size() + other.data.size());
      out.data.insert(out.data.end(), data.begin(), data.end());
      out.data.insert(out.data.end(), other.data.begin(), other.data.end());
      return out;
    }

    static DynamicArray from_table(const sol::table& table) {
      DynamicArray out;
      const size_t size = table.size();

      out.data.reserve(size);

      for (size_t i = 1; i <= size; ++i) {
        out.data.emplace_back(table.get<sol::object>(i));
      }

      return out;
    }

    sol::as_table_t<std::vector<sol::object>> to_table() const {
      return as_table(data);
    }

    bool equals(const DynamicArray& other, const sol::this_state this_state) const {
      if (data.size() != other.data.size()) {
        return false;
      }

      sol::state_view state_view(this_state);
      sol::function raw_equal = state_view["rawequal"];
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

      string_stream << "DynamicArray(";
      for (size_t i = 0; i < data.size(); ++i) {
        if (i) {
          string_stream << ", ";
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
      sol::usertype<DynamicArray> user_type = lua_state.new_usertype<DynamicArray>(name(),
        sol::constructors<DynamicArray()>(),
        "size", &DynamicArray::size,
        "capacity", &DynamicArray::capacity,
        "empty", &DynamicArray::empty,
        "clear", &DynamicArray::clear,
        "reserve", &DynamicArray::reserve,
        "shrink_to_fit", &DynamicArray::shrink_to_fit,
        "get", &DynamicArray::get,
        "set", &DynamicArray::set,
        "push_back", &DynamicArray::push_back,
        "push_front", &DynamicArray::push_front,
        "pop_front", &DynamicArray::pop_front,
        "pop_back", &DynamicArray::pop_back,
        "insert", &DynamicArray::insert,
        "erase", &DynamicArray::erase,
        "from_table", &DynamicArray::from_table,
        "to_table", &DynamicArray::to_table
      );

      user_type[sol::meta_function::length] = [](const DynamicArray& dynamic_array) {
        return dynamic_array.size();
      };

      user_type[sol::meta_function::addition] = [](const DynamicArray& first, const DynamicArray& second) {
        return first.concat(second);
      };

      user_type[sol::meta_function::equal_to] = [](const DynamicArray& first, const DynamicArray& second, const sol::this_state this_state) {
        return first.equals(second, this_state);
      };

      user_type[sol::meta_function::to_string] = [](const DynamicArray& dynamic_array, const sol::this_state this_state) {
        return dynamic_array.to_string(this_state);
      };

      user_type[sol::meta_function::index] = [](const DynamicArray& dynamic_array, const sol::stack_object key, const sol::this_state this_state) {
        if (key.is<int>()) {
          const int index = key.as<int>();
          return dynamic_array.get(index, this_state);
        }

        if (key.is<double>()) {
          const int index = static_cast<int>(key.as<double>());
          return dynamic_array.get(index, this_state);
        }

        return make_object(this_state, sol::lua_nil);
      };
    }
  };

}