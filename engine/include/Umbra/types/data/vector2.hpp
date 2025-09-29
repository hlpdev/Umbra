#pragma once

#include "Umbra/types.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <sol/sol.hpp>

namespace umbra {

  struct UMBRA_API Vector2 final : IType {
    double x;
    double y;

    const char* name() override  { return "Vector2"; }

    Vector2(const double x, const double y) noexcept : x(x), y(y) {}
    Vector2() noexcept : x(0), y(0) {}

    Vector2 abs() const noexcept {
      return { std::abs(x), std::abs(y) };
    }

    Vector2 ceil() const noexcept {
      return { std::ceil(x), std::ceil(y) };
    }

    Vector2 floor() const noexcept {
      return { std::floor(x), std::floor(y) };
    }

    Vector2 max(const Vector2& other) const noexcept {
      return { std::max(x, other.x), std::max(y, other.y) };
    }

    Vector2 min(const Vector2& other) const noexcept {
      return { std::min(x, other.x), std::min(y, other.y) };
    }

    double length() const noexcept {
      return std::sqrt(x * x + y * y);
    }

    double cross(const Vector2& other) const noexcept {
      return x * other.y - y * other.x;
    }

    Vector2 sign() const noexcept {
      auto sgn = [](const double v) -> double { return (v > 0.0f) - (v < 0.0f); };
      return { sgn(x), sgn(y) };
    }

    double dot(const Vector2& other) const noexcept {
      return x * other.x + y * other.y;
    }

    double angle(const Vector2& other, bool is_signed) const noexcept {
      const double other_dot = dot(other);
      const double this_length = length();
      const double other_length = other.length();

      if (this_length == 0.0 || other_length == 0.0) {
        return 0.0;
      }

      if (is_signed) {
        return std::atan2(cross(other), other_dot);
      }

      const double c = std::clamp(other_dot / (this_length * other_length), -1.0, 1.0);\
      return std::acos(c);
    }

    Vector2 lerp(const Vector2& other, const double alpha) const noexcept {
      const double inv = 1.0 - alpha;
      return { inv * x + alpha * other.x, inv * y + alpha * other.y };
    }

    bool fuzzy_eq(const Vector2& other, const double epsilon = 0.00001) const noexcept {
      const double dx = x - other.x;
      const double dy = y - other.y;
      const double dist2 = dx * dx + dy * dy;
      const double e2 = epsilon * epsilon;
      return dist2 <= e2;
    }

    Vector2 operator+(const Vector2& rhs) const noexcept { return { x + rhs.x, y + rhs.y }; }
    Vector2 operator-(const Vector2& rhs) const noexcept { return { x - rhs.x, y - rhs.y }; }
    Vector2 operator*(const Vector2& rhs) const noexcept { return { x * rhs.x, y * rhs.y }; }
    Vector2 operator/(const Vector2& rhs) const noexcept { return { x / rhs.x, y / rhs.y }; }

    Vector2 operator*(const double rhs) const noexcept { return { x * rhs, y * rhs }; }
    Vector2 operator/(const double rhs) const noexcept { return { x / rhs, y / rhs }; }

    Vector2& operator+=(const Vector2& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
    Vector2& operator-=(const Vector2& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
    Vector2& operator*=(const double rhs) noexcept { x *= rhs; y *= rhs; return *this; }
    Vector2& operator/=(const double rhs) noexcept { x /= rhs; y /= rhs; return *this; }

    void bind(sol::state& lua_state) {
      sol::usertype<Vector2> user_type = lua_state.new_usertype<Vector2>(name(),
        sol::constructors<Vector2(), Vector2(double, double)>(),
        "x", &Vector2::x,
        "y", &Vector2::y,

        "abs", &Vector2::abs,
        "ceil", &Vector2::ceil,
        "floor", &Vector2::floor,
        "max", &Vector2::max,
        "min", &Vector2::min,
        "length", &Vector2::length,
        "cross", &Vector2::cross,
        "sign", &Vector2::sign,
        "dot", &Vector2::dot,
        "angle", &Vector2::angle,
        "lerp", &Vector2::lerp,
        "fuzzy_eq", &Vector2::fuzzy_eq
      );

      user_type[sol::meta_function::addition] = [](const Vector2& lhs, const Vector2& rhs) { return lhs + rhs; };
      user_type[sol::meta_function::subtraction] = [](const Vector2& lhs, const Vector2& rhs) { return lhs - rhs; };

      user_type[sol::meta_function::multiplication] = sol::overload(
        [](const Vector2& lhs, const double rhs) { return lhs * rhs; },
        [](const double lhs, const Vector2& rhs) { return rhs * lhs; },
        [](const Vector2& lhs, const Vector2& rhs) { return lhs * rhs; }
      );

      user_type[sol::meta_function::division] = sol::overload(
        [](const Vector2& lhs, const double rhs) { return lhs / rhs; },
        [](const Vector2& lhs, const Vector2& rhs) { return lhs / rhs; }
      );

      user_type[sol::meta_function::unary_minus] = [](const Vector2& rhs) { return Vector2(-rhs.x, -rhs.y); };

      user_type[sol::meta_function::equal_to] = [](const Vector2& lhs, const Vector2& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
      };

      user_type[sol::meta_function::to_string] = [](const Vector2& vector) {
        return "Vector2(" + std::to_string(vector.x) + ", " + std::to_string(vector.y) + ")";
      };
    }
  };

}