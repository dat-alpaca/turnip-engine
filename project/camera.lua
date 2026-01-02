local camera = {
    _description = "Default camera",
    camera_c = nil,
    _pixel_per_meter = 50,
    
    _direction = vec2(0.0, 0.0)
}

function camera:new(object)
    object = object or {}
    setmetatable(object, self)
    self.__index = self
    return object
end

function camera:on_wake()
    self.camera_c = self.find_component("camera")
    Log.info("Camera ID: " .. self._entity:id())
end

function camera:get_direction()
    if Input.get_key_pressed(Input.Key.W) then
        self._direction = vec2(self._direction.x, -1.0)
    end

    if Input.get_key_pressed(Input.Key.S) then
        self._direction = vec2(self._direction.x, 1.0)
    end

    if Input.get_key_pressed(Input.Key.A) then
        self._direction = vec2(-1.0, self._direction.y)
    end

    if Input.get_key_pressed(Input.Key.D) then
        self._direction = vec2(1.0, self._direction.y)
    end
end

function camera:on_update()
    self:get_direction()
end

function camera:on_post_update(time)
    local position = self.camera_c:get_position()

    local new_position = vec3(
        position.x + self._direction.x * time:get_delta_time(), 
        position.y + self._direction.y * time:get_delta_time(),
        position.z
    )
    
    self.camera_c:set_position(new_position)
    self.camera_c:set_target(vec3(new_position.x, new_position.y, -1))

    self._direction = vec2(0.0, 0.0)
end

function camera:on_window_resize(width, height)
    self.camera_c:set_ortho(0.0, (width / self._pixel_per_meter), 0.0, (height / self._pixel_per_meter), -1, 1)
end

return camera