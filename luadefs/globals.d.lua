---@meta
---@diagnostic disable: missing-return

---@class umbra : userdata
umbra = {}

---@alias ServiceNames "Renderer"

---@generic T : ServiceNames
---@param service_name T
---@return T == "Renderer" and Renderer or nil
function umbra.get_service(service_name)
    if service_name == "Renderer" then
        return Renderer
    end

    return nil
end

local test = umbra.get_service("Renderer")