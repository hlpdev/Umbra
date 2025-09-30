---@meta
---@diagnostic disable: missing-return

---@class StaticArray : userdata
StaticArray = {}

---Creates a StaticArray with the specified size.
---@overload fun(): StaticArray
---@param size number
---@return StaticArray
function StaticArray.new(size) end

---Gets the size of the static array.
---@return number
function StaticArray:size() end

---Returns whether the StaticArray is empty or not
---@return boolean
function StaticArray:empty() end

---Gets the value at the specified index.
---@param index number
---@return any|nil
function StaticArray:get(index) end

---Sets the value at the specified index.
---@param index number
---@param value any
function StaticArray:set(index, value) end

---Fills the entire StaticArray with the specified value.
---@param value any
function StaticArray:fill(value) end

---Creates a StaticArray based on an ordered table.
---@param table table
---@return StaticArray
function StaticArray.from_table(table) end

---Returns an ordered table based on the current StaticArray.
---@return table
function StaticArray:to_table() end

---@operator len(): number