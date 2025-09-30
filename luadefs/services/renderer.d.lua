---@meta
---@diagnostic disable: missing-return

---@class Renderer : userdata
Renderer = {}

---Shows the game window.
function Renderer:show_window() end

---Hides the game window.
function Renderer:hide_window() end

---Sets the window's title.
---@param title string
function Renderer:set_title(title) end

---Resizes the window.
---@param new_size Vector2
function Renderer:resize(new_size) end

---Sets the window size limits.
---@param minimum Vector2
---@param maximum Vector2
function Renderer:set_size_limit(minimum, maximum) end

---Gets the current window size.
---@return Vector2
function Renderer:get_size() end

---Moves the window's position.
---@param new_position Vector2
function Renderer:move(new_position) end

