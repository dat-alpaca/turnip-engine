local controller = {
    camera = nil
}

function controller:new(object)
    object = object or {}
    setmetatable(object, self)
    self.__index = self
    return object
end

function controller:on_wake()
    Log.info("on_wake")
end

function controller:on_camera_switched()
    Log.info("on_camera_switched")
    self.camera = self.get_main_camera()
end

function controller:on_update()
    self.camera = self.get_main_camera()

    local position = self.camera:get_position()
    -- Log.info("camera position: ( " .. position.x .. ", " .. position.y .. ", " .. position.z .. " )")

    local mouse_pos = Input.get_mouse_position()
    local world_pos = self.camera:get_world_position(vec3(mouse_pos.x, mouse_pos.y, 1.0))

    Log.info("world: ( " .. world_pos.x .. " )")
end

return controller