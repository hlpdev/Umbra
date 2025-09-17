---@meta
---@diagnostic disable: missing-return

---@class SinglyLinkedList : userdata
SinglyLinkedList = {}

---Creats a SinglyLinkedList
---@return SinglyLinkedList
function SinglyLinkedList.new() end

---Gets the current size of the SinglyLinkedList.
---@return number
function SinglyLinkedList:size() end

---Returns whether the SinglyLinkedList is empty or not.
---@return boolean
function SinglyLinkedList:empty() end

---Clears the SinglyLinkedList
function SinglyLinkedList:clear() end

---Adds an item to the front of the SinglyLinkedList.
---@param value any
function SinglyLinkedList:push_front(value) end

---Adds an item to the back of the SinglyLinkedList.
---@param value any
function SinglyLinkedList:push_back(value) end

---Removes the first item in the SinglyLinkedList and returns it.
---@return any
function SinglyLinkedList:pop_front() end

---Removes the last item in the SinglyLinkedList and returns it.
---@return any
function SinglyLinkedList:pop_back() end

---Gets the value at the specified index.
---@param index number
---@return any|nil
function SinglyLinkedList:get(index) end

---Sets the value at the specified index.
---@param index number
---@param value any
function SinglyLinkedList:set(index, value) end

---Inserts the value at the specified index.
---@param index number
---@param value any
function SinglyLinkedList:insert(index, value) end

---Erases the value at the specified index.
---@param index number
function SinglyLinkedList:erase(index) end

---Concatenates the current SinglyLinkedList with the given.
---@param other SinglyLinkedList
---@return SinglyLinkedList
function SinglyLinkedList:concat(other) end

---Creates a SinglyLinkedList from a table
---@param table table
---@return SinglyLinkedList
function SinglyLinkedList.from_table(table) end

---Creates an ordered table from the SinglyLinkedList.
---@return table
function SinglyLinkedList:to_table() end

---@operator len(): number
---@operator add(SinglyLinkedList): SinglyLinkedList
---@operator concat(SinglyLinkedList): SinglyLinkedList