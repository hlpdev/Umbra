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
  private:
    std::unique_ptr<sol::object[]> data_;
    int size_ = 0;

  public:
    const char* name() override { return "StaticArray"; }

    StaticArray() noexcept = default;
    StaticArray(const StaticArray&) = delete;
    StaticArray& operator=(const StaticArray&) = delete;
    StaticArray(StaticArray&&) noexcept = default;
    StaticArray& operator=(StaticArray&&) noexcept = default;

    StaticArray(const int count, const sol::this_state this_state) {
      size_ = std::max(0, count);
      if (size_ == 0) {
        data_.reset();
        return;
      }

      data_ = std::unique_ptr<sol::object[]>(new sol::object[static_cast<size_t>(size_)]);
      for (int i = 0; i < size_; ++i) {
        data_[i] = make_object(this_state, sol::lua_nil);
      }
    }

    int size() const noexcept { return size_; }
    bool empty() const noexcept { return size() == 0; }

    sol::object get(const int index, const sol::this_state this_state) const noexcept {
      if (index < 1 || index > size_) {
        return make_object(this_state, sol::lua_nil);
      }

      return data_[index - 1];
    }

    void set(const int index, const sol::object& value) noexcept {
      if (index < 1 || index > size_) {
        return;
      }

      data_[index - 1] = value;
    }

    void fill(const sol::object &value) {
      for (int i = 0; i < size_; ++i) {
        data_[i] = value;
      }
    }

    static StaticArray from_table(const sol::table& table) {
      const int len = static_cast<int>(table.size());

      StaticArray out(len, sol::this_state{ table.lua_state() });
      for (int i = 1; i <= len; ++i) {
        out.data_[i - 1] = table.get<sol::object>(i);
      }

      return std::move(out);
    }

    sol::as_table_t<std::vector<sol::object>> to_table() const {
      std::vector<sol::object> vector;
      vector.reserve(size_);

      for (int i = 0; i < size_; ++i) {
        vector.emplace_back(data_[i]);
      }

      return as_table(std::move(vector));
    }

    bool equals(const StaticArray& other, const sol::this_state this_state) const {
      if (size_ != other.size_) {
        return false;
      }

      sol::state_view state_view(this_state);
      const sol::function raw_equal = state_view["rawequal"];

      for (int i = 0; i < size_; ++i) {
        if (raw_equal.valid()) {
          if (!raw_equal(data_[i], other.data_[i])) {
            return false;
          }
        } else {
          if (data_[i] != other.data_[i]) {
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
      for (size_t i = 0; i < size_; ++i) {
        if (i) {
          string_stream << ", ";
        }

        if (data_[i].is<std::string>()) {
          string_stream << '\'' << data_[i].as<std::string>() << '\'';
          continue;
        }

        if (to_string.valid()) {
          sol::object value = to_string(data_[i]);
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
        auto to_int = [](const sol::stack_object k) -> std::optional<int> {
          if (k.is<int>()) {
            return k.as<int>();
          }
          if (k.is<double>()) {
            return static_cast<int>(k.as<double>());
          }
          return std::nullopt;
        };

        if (const std::optional<int> optional_index = to_int(key)) {
          static_array.set(*optional_index, sol::object(value));
        }
      };

      user_type[sol::meta_function::ipairs] = [](StaticArray& static_array, sol::this_state this_state) {
        auto iter = [this_state](void* static_array_anonymous, const int i) -> std::tuple<sol::object, sol::object> {
          const auto* static_array_to_iter = static_cast<StaticArray*>(static_array_anonymous);

          const int next = i + 1;
          if (next > static_array_to_iter->size()) {
            sol::object nil = make_object(this_state, sol::lua_nil);
            return { nil, nil };
          }

          sol::object value = static_array_to_iter->get(next, this_state);
          if (value.is<sol::lua_nil_t>()) {
            sol::object nil = make_object(this_state, sol::lua_nil);
            return { nil, nil };
          }

          return { make_object(this_state, next), value };
        };

        return std::make_tuple(iter, sol::light(&static_array), 0);
      };

      user_type[sol::meta_function::pairs] = [](StaticArray& static_array, sol::this_state this_state) {
        auto iter = [this_state](void* static_array_anonymous, const int i) -> std::tuple<sol::object, sol::object> {
          const auto* static_array_to_iter = static_cast<StaticArray*>(static_array_anonymous);

          const int next = i + 1;
          if (next > static_array_to_iter->size()) {
            sol::object nil = make_object(this_state, sol::lua_nil);
            return { nil, nil };
          }

          return { make_object(this_state, next), static_array_to_iter->get(next, this_state) };
        };

        return std::make_tuple(iter, sol::light(&static_array), 0);
      };
    }

  };

}