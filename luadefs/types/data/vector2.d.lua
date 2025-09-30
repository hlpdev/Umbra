---@meta
---@diagnostic disable: missing-return

---@class Vector2 : userdata
---@field x number
---@field y number
Vector2 = {}

---Creates a Vector2 with the specified X and Y values.
---@overload fun(): Vector2
---@param x number
---@param y number
---@return Vector2
function Vector2.new(x, y) end

---Absolute value per component.
---@return Vector2
function Vector2:abs() end

---Ceil per component.
---@return Vector2
function Vector2:ceil() end

---Floor per component.
---@return Vector2
function Vector2:floor() end

---Component-wise max.
---@param other Vector2
---@return Vector2
function Vector2:max(other) end

---Component-wise min.
---@param other Vector2
---@return Vector2
function Vector2:min(other) end

---Length (magnitude)
---@return number
function Vector2:length() end

---2D cross product (scalar z component)
---@param other Vector2
---@return number
function Vector2:cross(other) end

---Component-wise sign (-1, 0, +1)
---@return Vector2
function Vector2:sign() end

---Dot product.
---@param other Vector2
---@return number
function Vector2:dot(other) end

---Angle to other vector.
---@param other Vector2
---@param is_signed boolean # if true, signed angle via atan2
---@return number
function Vector2:angle(other, is_signed) end

---Linear interpolation.
---@param other Vector2
---@param alpha number
---@return Vector2
function Vector2:lerp(other, alpha) end

---Fuzzy equality by epsilon.
---@param other Vector2
---@param epsilon number|nil
---@return boolean
function Vector2:fuzzy_eq(other, epsilon) end

---@operator add(Vector2): Vector2
---@operator sub(Vector2): Vector2
---@operator mul(number): Vector2
---@operator mul(Vector2): Vector2
---@operator div(number): Vector2
---@operator div(Vector2): Vector2
---@operator unm(): Vector2