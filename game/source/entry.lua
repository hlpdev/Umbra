-- Quick test for DynamicArray (prints only)
-- Assumes DynamicArray and Vector2 are already bound via sol.

print("=== DynamicArray: basic construction ===")
local A = DynamicArray.new()
print("A (empty)        = " .. tostring(A))
print("#A                = " .. tostring(#A))
print("A:empty()         = " .. tostring(A:empty()))
print("A:capacity()      = " .. tostring(A:capacity()))
A:reserve(16)
print("A:reserve(16); cap= " .. tostring(A:capacity()))

-- Prepare some reusable values to test reference/identity behavior
local tbl = { x = 1, y = { z = 2 } }
local v2  = Vector2.new(1.5, -2.5)
local fn  = function(x) return x * 2 end

print("\n=== DynamicArray: push_back / front with mixed types ===")
A:push_back(123)          -- number
A:push_back("hello")      -- string
A:push_back(true)         -- boolean
A:push_back(nil)          -- nil
A:push_back(tbl)          -- table (by reference)
A:push_back(v2)           -- usertype
A:push_back(fn)           -- function (by reference)
A:push_front("FIRST!")    -- push at front

print("A = " .. tostring(A))
print("#A = " .. tostring(#A))

print("\n=== DynamicArray: get / set (1-based) ===")
print("A:get(1)          = " .. tostring(A:get(1)))
print("A:get(2)          = " .. tostring(A:get(2)))
print("A:get(3)          = " .. tostring(A:get(3)))
print("A:get(4)          = " .. tostring(A:get(4)))   -- nil
print("A:get(5) (table)  = " .. tostring(A:get(5)))
print("A:get(6) (Vector2)= " .. tostring(A:get(6)))
print("A:get(7) (func)   = " .. tostring(A:get(7)))
print("A:get(999) (OOB)  = " .. tostring(A:get(999))) -- should be nil

A:set(2, "world")   -- replace "hello" with "world"
print("A after set(2,'world') = " .. tostring(A))

print("\n=== DynamicArray: insert / erase ===")
A:insert(3, "MID")        -- insert before index 3
print("A after insert(3,'MID') = " .. tostring(A))
A:erase(4)                -- erase former 'true'
print("A after erase(4)        = " .. tostring(A))

print("\n=== DynamicArray: pop_front / pop_back ===")
local pf = A:pop_front()
print("pop_front() -> " .. tostring(pf))
local pb = A:pop_back()
print("pop_back()  -> " .. tostring(pb))
print("A after pops = " .. tostring(A))

print("\n=== DynamicArray: concat and equality (rawequal per element) ===")
local B = DynamicArray.from_table{ 9, 8, 7 }
print("B = " .. tostring(B))
local C = A + B
print("C = A + B -> " .. tostring(C))
print("#C = " .. tostring(#C))

-- D replicates A's contents including the SAME references (tbl/v2/fn) for equality to pass
local D = DynamicArray.new()
-- rebuild same order as current A
for i = 1, #A do D:push_back(A:get(i)) end
print("A == D ? " .. tostring(A == D))

-- E uses deep-copied table with same contents but different identity -> should be false
local E = DynamicArray.new()
for i = 1, #A do
    local v = A:get(i)
    if i == 4 and type(v) == "table" then
        E:push_back({ x = 1, y = { z = 2 } }) -- different table identity
    else
        E:push_back(v)
    end
end
print("A == E ? " .. tostring(A == E))

print("\n=== DynamicArray: to_table / from_table round-trip ===")
local plain = A:to_table()        -- plain Lua array table
print("plain (from A) type = " .. type(plain) .. ", len = " .. tostring(#plain))
for i = 1, #plain do
    print("plain[" .. i .. "] = " .. tostring(plain[i]))
end

local F = DynamicArray.from_table(plain)
print("F (from_table(plain)) = " .. tostring(F))
print("A == F ? " .. tostring(A == F))

print("\n=== DynamicArray: clear / shrink_to_fit ===")
print("Before clear: size=" .. tostring(A:size()) .. ", cap=" .. tostring(A:capacity()))
A:clear()
print("After  clear: size=" .. tostring(A:size()) .. ", cap=" .. tostring(A:capacity()))
A:shrink_to_fit()
print("After  shrink_to_fit: cap=" .. tostring(A:capacity()))
