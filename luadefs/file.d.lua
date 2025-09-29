---@meta
---@diagnostic disable: missing-return

---@class File : userdata
File = {}

---The size of the file
---@return number
function File:size() end

---The contents of the file as a string
---@return string
function File:as_string() end