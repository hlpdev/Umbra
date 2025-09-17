local data = SinglyLinkedList.new()

data:push_back(5)
data:push_back(6)
data:push_back(7)
data:push_front(4)

print(data)

local front_item = data:pop_front()
local back_item = data:pop_back()

print(front_item)
print(back_item)

print(data)