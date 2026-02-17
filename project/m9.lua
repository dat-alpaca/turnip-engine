local m9 = {
    _description = "M9",
    transform_c = nil
}

camera = nil

function m9:new(object)
    object = object or {}
    setmetatable(object, self)
    self.__index = self
    return object
end

function m9:on_wake()
    self.transform_c = self.find_component("transform")
    camera = self.get_entity(4):find_component("camera")
end

function m9:on_update(time)
    local mouse_pos = Input.get_mouse_position()
    if Input.get_mouse_pressed(Input.MouseButton.MOUSE_LEFT) then
        local world_position = camera:get_world_position(vec3(mouse_pos.x, mouse_pos.y, 1))
        -- self.transform_c.position.x = world_position.x
        -- self.transform_c.position.y = world_position.y
    end

    if Input.get_key_pressed(Input.Key.KP_4) then
        self.transform_c.rotation.x = self.transform_c.rotation.x + 50 * time:get_delta_time()
    end

    if Input.get_key_pressed(Input.Key.KP_6) then
        self.transform_c.rotation.x = self.transform_c.rotation.x - 50 * time:get_delta_time()
    end

     if Input.get_key_pressed(Input.Key.KP_8) then
        self.transform_c.rotation.y = self.transform_c.rotation.y + 50 * time:get_delta_time()
    end
end

return m9