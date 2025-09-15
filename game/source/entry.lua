-- Quick test for StaticArray (prints only)

print("=== StaticArray: construct & basics ===")
local S = StaticArray.new(5)  -- 5 slots (prefilled with nil)
print("S = " .. tostring(S))
print("#S = " .. tostring(#S))
print("S:empty() = " .. tostring(S:empty()))

print("\n=== StaticArray: set/get (1-based) ===")
local t = { k = 42 }
local v = Vector2.new(3, -1)
local f = function(x) return x + 1 end

S:set(1, 123)        -- number
S:set(2, "hi")       -- string
S:set(3, true)       -- boolean
S:set(4, t)          -- table (reference)
S:set(5, v)          -- usertype

print("S:get(1) = " .. tostring(S:get(1)))
print("S:get(2) = " .. tostring(S:get(2)))
print("S:get(3) = " .. tostring(S:get(3)))
print("S:get(4) = " .. tostring(S:get(4)))
print("S:get(5) = " .. tostring(S:get(5)))
print("S:get(6) (OOB) = " .. tostring(S:get(6)))  -- nil

print("\n=== StaticArray: fill ===")
S:fill("x")
print("S after fill('x') = " .. tostring(S))

print("\n=== StaticArray: identity behavior ===")
S:set(4, t)
S:set(5, f)
print("S (with table+func) = " .. tostring(S))

local S2 = StaticArray.new(5)
S2:set(1, "x"); S2:set(2, "x"); S2:set(3, "x"); S2:set(4, t); S2:set(5, f)
print("S == S2 ? " .. tostring(S == S2))  -- true (rawequal per slot)

local S3 = StaticArray.new(5)
S3:set(1, "x"); S3:set(2, "x"); S3:set(3, "x"); S3:set(4, { k = 42 }); S3:set(5, f)
print("S == S3 ? " .. tostring(S == S3))  -- false (different table identity)

print("\n=== StaticArray: to_table / from_table ===")
local plain = S:to_table()
print("plain type = " .. type(plain) .. ", #plain = " .. tostring(#plain)) -- stops at first nil; here none at 1..5
for i = 1, #plain do
    print("plain[" .. i .. "] = " .. tostring(plain[i]))
end

local S4 = StaticArray.from_table({ 1, nil, 3 }) -- size = 1 (Lua length stops at nil)
print("S4 (from_table) = " .. tostring(S4))

print("\n=== StaticArray: bounds safety ===")
S:set(999, "nope")  -- no-op
print("S:get(999) = " .. tostring(S:get(999))) -- nil
