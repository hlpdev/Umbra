local vector = Vector3.new()


local data = StaticArray.new(9)

data[1] = 1
data[2] = 2
data[3] = 3
data[4] = 4
data[5] = 5
data[6] = 6
data[7] = 7
data[8] = 8
data[9] = 9

print("\n-- DATASET --")
print(data)

print("\n-- IPAIRS --")
for i, v in ipairs(data) do
    print("index:", i, "value:", v)
end

print("\n-- PAIRS --")
for i, v in pairs(data) do
    print("index:", i, "value:", v)
end