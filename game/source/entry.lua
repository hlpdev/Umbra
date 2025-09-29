local renderer = umbra.get_service("Renderer")
local vfs = umbra.get_service("VirtualFileSystem")

local icon = vfs:read("assets://window_icon.png")
renderer:set_icon(icon)

while not renderer:should_close() do
    renderer:begin_render()

    -- here is where the game begins

    renderer:end_render()
end