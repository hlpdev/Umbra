#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <sol/sol.hpp>

#include "Umbra/types.hpp"

namespace umbra {

  struct UMBRA_API Vector3 final : IType {
    double x;
    double y;
    double z;

    const char* name() override { return "Vector3"; }

    Vector3(const double x, const double y, const double z) noexcept : x(x), y(y), z(z) {}
    Vector3() noexcept : x(0), y(0), z(0) {}

    Vector3 abs() const noexcept {
      return { std::abs(x), std::abs(y), std::abs(z) };
    }

    Vector3 ceil() const noexcept {
      return { std::ceil(x), std::ceil(y), std::ceil(z) };
    }

    Vector3 floor() const noexcept {
      return { std::floor(x), std::floor(y), std::floor(z) };
    }

    Vector3 max(const Vector3& other) const noexcept {
      return { std::max(x, other.x), std::max(y, other.y), std::max(z, other.z) };
    }

    Vector3 min(const Vector3& other) const noexcept {
      return { std::min(x, other.x), std::min(y, other.y), std::min(z, other.z) };
    }

    double length() const noexcept {
      return std::sqrt(x * x + y * y + z * z);
    }

    Vector3 cross(const Vector3& other) const noexcept {
      return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
      };
    }

    Vector3 sign() const noexcept {
      auto sgn = [](const double v) -> double { return (v > 0.0f) - (v < 0.0f); };
      return { sgn(x), sgn(y), sgn(z) };
    }

    double dot(const Vector3& other) const noexcept {
      return x * other.x + y * other.y + z * other.z;
    }

    double angle(const Vector3& other) const noexcept {
      const double d  = dot(other);
      const double la = length();
      const double lb = other.length();
      if (la == 0.0 || lb == 0.0) return 0.0;

      const Vector3 c = cross(other);
      const double cl = std::sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
      return std::atan2(cl, d);
    }

    double angle(const Vector3& other, const Vector3& axis) const noexcept {
      const double base = angle(other);
      const double axis_len2 = axis.x*axis.x + axis.y*axis.y + axis.z*axis.z;
      if (axis_len2 == 0.0) return base;

      const Vector3 c = cross(other);
      const double sign_dot = axis.x * c.x + axis.y * c.y + axis.z * c.z;
      const double sgn = (sign_dot > 0.0) - (sign_dot < 0.0);
      return sgn >= 0.0 ? base : -base;
    }

    Vector3 lerp(const Vector3& other, const double alpha) const noexcept {
      const double inv = 1.0 - alpha;
      return { inv * x + alpha * other.x, inv * y + alpha * other.y, inv * z + alpha * other.z };
    }

    bool fuzzy_eq(const Vector3& other, const double epsilon = 0.00001) const noexcept {
      const double dx = x - other.x;
      const double dy = y - other.y;
      const double dz = z - other.z;
      const double dist2 = dx * dx + dy * dy + dz * dz;
      const double e2 = epsilon * epsilon;
      return dist2 <= e2;
    }

    Vector3 operator+(const Vector3& rhs) const noexcept { return { x + rhs.x, y + rhs.y, z + rhs.z }; }
    Vector3 operator-(const Vector3& rhs) const noexcept { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
    Vector3 operator*(const Vector3& rhs) const noexcept { return { x * rhs.x, y * rhs.y, z * rhs.z }; }
    Vector3 operator/(const Vector3& rhs) const noexcept { return { x / rhs.x, y / rhs.y, z / rhs.z }; }

    Vector3 operator*(const double rhs) const noexcept { return { x * rhs, y * rhs, z * rhs }; }
    Vector3 operator/(const double rhs) const noexcept { return { x / rhs, y / rhs, z / rhs }; }

    Vector3& operator+=(const Vector3& rhs) noexcept { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    Vector3& operator-=(const Vector3& rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    Vector3& operator*=(const double rhs) noexcept { x *= rhs; y *= rhs; z *= rhs; return *this; }
    Vector3& operator/=(const double rhs) noexcept { x /= rhs; y /= rhs; z /= rhs; return *this; }

    void bind(sol::state& lua_state) {
      sol::usertype<Vector3> user_type = lua_state.new_usertype<Vector3>(name(),
        sol::constructors<Vector3(), Vector3(double, double, double)>(),
        "x", &Vector3::x,
        "y", &Vector3::y,
        "z", &Vector3::z,

        "abs", &Vector3::abs,
        "ceil", &Vector3::ceil,
        "floor", &Vector3::floor,
        "max", &Vector3::max,
        "min", &Vector3::min,
        "length", &Vector3::length,
        "cross", &Vector3::cross,
        "sign", &Vector3::sign,
        "dot", &Vector3::dot,
        "lerp", &Vector3::lerp,
        "fuzzy_eq", &Vector3::fuzzy_eq
      );

      user_type.set_function("angle", sol::overload(
        [](const Vector3& self, const Vector3& other) {
          return self.angle(other);
        },
        [](const Vector3& self, const Vector3& other, const Vector3& axis) {
          return self.angle(other, axis);
        }
      ));

      user_type[sol::meta_function::addition] = [](const Vector3& lhs, const Vector3& rhs) { return lhs + rhs; };
      user_type[sol::meta_function::subtraction] = [](const Vector3& lhs, const Vector3& rhs) { return lhs - rhs; };

      user_type[sol::meta_function::multiplication] = sol::overload(
        [](const Vector3& lhs, const double rhs) { return lhs * rhs; },
        [](const double lhs, const Vector3& rhs) { return rhs * lhs; },
        [](const Vector3& lhs, const Vector3& rhs) { return lhs * rhs; }
      );

      user_type[sol::meta_function::division] = sol::overload(
        [](const Vector3& lhs, const double rhs) { return lhs / rhs; },
        [](const Vector3& lhs, const Vector3& rhs) { return lhs / rhs; }
      );

      user_type[sol::meta_function::unary_minus] = [](const Vector3& rhs) { return Vector3(-rhs.x, -rhs.y, -rhs.z); };

      user_type[sol::meta_function::equal_to] = [](const Vector3& lhs, const Vector3& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
      };

      user_type[sol::meta_function::to_string] = [](const Vector3& v) {
        return "Vector3(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
      };
    }
  };

}