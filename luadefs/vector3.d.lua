---@meta
---@diagnostic disable: missing-return

---@class Vector3 : userdata
---@field x number
---@field y number
---@field z number
Vector3 = {}

---Creates a Vector3 with the specified X, Y, and Z values.
---@overload fun(): Vector3
---@param x number
---@param y number
---@param z number
---@return Vector3
function Vector3.new(x, y, z) end

---Absolute value per component.
---@return Vector3
function Vector3:abs() end

---Ceil per component.
---@return Vector3
function Vector3:ceil() end

---Floor per component.
---@return Vector3
function Vector3:floor() end

---Component-wise max.
---@param other Vector3
---@return Vector3
function Vector3:max(other) end

---Compoent-wise min.
---@param other Vector3
---@return Vector3
function Vector3:min(other) end

---Length (magnitude)
---@return number
function Vector3:length() end

---Dot product.
---@param other Vector3
---@return number
function Vector3:dot(other) end

---Cross product.
---@param other Vector3
---@return Vector3
function Vector3:cross(other) end

---Unsigned or signed angle (with axis).
---@overload fun(other: Vector3): number
---@param other Vector3
---@param axis Vector3
---@return number
function Vector3:angle(other, axis) end

---Linear interpolation.
---@param other Vector3
---@param alpha number
---@return Vector3
function Vector3:lerp(other, alpha) end

---Fuzzy equality by epsilon.
---@param other Vector3
---@param epsilon number|nil
---@return boolean
function Vector3:fuzzy_eq(other, epsilon) end

---@operator add(Vector3): Vector3
---@operator sub(Vector3): Vector3
---@operator mul(number): Vector3
---@operator mul(Vector3): Vector3
---@operator div(number): Vector3
---@operator div(Vector3): Vector3
---@operator unm(): Vector3
---@operator len(): number