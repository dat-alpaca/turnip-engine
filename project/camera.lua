local camera = {
    _description = "Default camera",
    camera_c = nil,
    _pixel_per_meter = 64
}

_G["pixel_per_meter"] = camera._pixel_per_meter

function camera:new(object)
    object = object or {}
    setmetatable(object, self)
    self.__index = self
    return object
end

function camera:on_wake()
    self.camera_c = self.find_component("camera")
end

function camera:on_update()
end

function camera:on_window_resize(width, height)
    self.camera_c:set_ortho(0.0, (width / self._pixel_per_meter), 0.0, (height / self._pixel_per_meter), -1, 1)
end

return camera