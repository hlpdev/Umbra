---@meta
---@diagnostic disable: missing-return

---@class DynamicArray : userdata
DynamicArray = {}

---Creates a DynamicArray.
---@return DynamicArray
function DynamicArray.new() end

---Gets the current size of the DynamicArray.
---@return number
function DynamicArray:size() end

---Gets the current capacity of the DynamicArray.
---@return number
function DynamicArray:capacity() end

---Returns whether the DynamicArray is empty or not.
---@return boolean
function DynamicArray:empty() end

---Removes all elements from the DynamicArray.
function DynamicArray:clear() end

---Reserves memory for the specified amount of objects in the DynamicArray.
---@param quantity number
function DynamicArray:reserve(quantity) end

---Reduces the DynamicArray's capacity to the quantity of objects currently stored.
function DynamicArray:shrink_to_fit() end

---Gets the object at the specified index.
---@param index number
---@return any|nil
function DynamicArray:get(index) end

---Sets the value at the specified index.
---@param index number
---@param value any
function DynamicArray:set(index, value) end

---Adds the value at the end of the DynamicArray.
---@param value any
function DynamicArray:push_back(value) end

---Adds the value at the beginning of the DynamicArray.
---@param value any
function DynamicArray:push_front(value) end

---Removes the value at the beginning of the DynamicArray and returns it.
---@return any|nil
function DynamicArray:pop_front() end

---Removes the value at the end of the DynamicArray and returns it.
---@return any|nil
function DynamicArray:pop_back() end

---Inserts the value at the specified index.
---@param index number
---@param value any
function DynamicArray:insert(index, value) end

---Erases the value at the specified index.
---@param index number
function DynamicArray:erase(index) end

---Creates a DynamicArray based on the given table.
---@param table table
---@return DynamicArray
function DynamicArray.from_table(table) end

---Creates an ordered table based on the DynamicArray.
---@return table
function DynamicArray:to_table() end

---@operator len(): number
---@operator add(DynamicArray): DynamicArray