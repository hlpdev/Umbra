-- Quick test for Vector2 and Vector3 (prints only)

-- ===== Vector2 =====
local a2 = Vector2.new(1.2, -3.4)
local b2 = Vector2.new(-5.6, 7.8)

print("=== Vector2 ===")
print("a2 = " .. tostring(a2))
print("b2 = " .. tostring(b2))

print("abs(a2) = " .. tostring(a2:abs()))
print("ceil(a2) = " .. tostring(a2:ceil()))
print("floor(a2) = " .. tostring(a2:floor()))
print("max(a2,b2) = " .. tostring(a2:max(b2)))
print("min(a2,b2) = " .. tostring(a2:min(b2)))

print("length(a2) = " .. a2:length())
print("dot(a2,b2) = " .. a2:dot(b2))
print("cross(a2,b2) = " .. a2:cross(b2))
print("sign(a2) = " .. tostring(a2:sign()))

print("angle(a2,b2 unsigned) = " .. a2:angle(b2, false))
print("angle(a2,b2 signed)   = " .. a2:angle(b2, true))

print("lerp(a2,b2, 0.25) = " .. tostring(a2:lerp(b2, 0.25)))
print("fuzzy_eq(a2, a2+tiny, 1e-3) = " ..
        tostring(a2:fuzzy_eq(a2 + Vector2.new(1e-4, 0), 1e-3)))

print("a2 + b2 = " .. tostring(a2 + b2))
print("a2 - b2 = " .. tostring(a2 - b2))
print("a2 * 2  = " .. tostring(a2 * 2))
print("2 * a2  = " .. tostring(2 * a2))
print("a2 * b2 (component) = " .. tostring(a2 * b2))
print("a2 / 2  = " .. tostring(a2 / 2))
print("a2 / b2 (component) = " .. tostring(a2 / b2))
print("-a2 = " .. tostring(-a2))
print("a2 == a2 ? " .. tostring(a2 == a2))
print("a2 == b2 ? " .. tostring(a2 == b2))

-- ===== Vector3 =====
local a3 = Vector3.new(1, 0, 0)
local b3 = Vector3.new(0, 1, 0)
local axis = Vector3.new(0, 0, 1) -- +Z

print("\n=== Vector3 ===")
print("a3 = " .. tostring(a3))
print("b3 = " .. tostring(b3))

print("abs(a3)   = " .. tostring(a3:abs()))
print("ceil(b3)  = " .. tostring(b3:ceil()))
print("floor(b3) = " .. tostring(b3:floor()))
print("max(a3,b3) = " .. tostring(a3:max(b3)))
print("min(a3,b3) = " .. tostring(a3:min(b3)))

print("length(a3) = " .. a3:length())
print("dot(a3,b3) = " .. a3:dot(b3))
print("cross(a3,b3) = " .. tostring(a3:cross(b3)))
print("sign(b3) = " .. tostring(b3:sign()))

print("angle(a3,b3) unsigned = " .. a3:angle(b3))
print("angle(a3,b3, +Z) signed = " .. a3:angle(b3, axis))
print("angle(b3,a3, +Z) signed = " .. b3:angle(a3, axis))

print("lerp(a3,b3, 0.5) = " .. tostring(a3:lerp(b3, 0.5)))
print("fuzzy_eq(a3, a3+tiny, 1e-3) = " ..
        tostring(a3:fuzzy_eq(a3 + Vector3.new(0, 0, 5e-4), 1e-3)))

print("a3 + b3 = " .. tostring(a3 + b3))
print("a3 - b3 = " .. tostring(a3 - b3))
print("a3 * 2  = " .. tostring(a3 * 2))
print("2 * a3  = " .. tostring(2 * a3))
print("a3 * b3 (component) = " .. tostring(a3 * b3))
print("a3 / 2  = " .. tostring(a3 / 2))
print("a3 / b3 (component) = " .. tostring(a3 / b3))
print("-a3 = " .. tostring(-a3))
print("a3 == a3 ? " .. tostring(a3 == a3))
print("a3 == b3 ? " .. tostring(a3 == b3))
