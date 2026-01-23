local camera = {
    _description = "Default camera",
    camera_c = nil,
    _pixel_per_meter = 50,
    
    _direction = vec3(0.0, 0.0, 0.0),
    _mode = "perspective",

    _middle_pressed = false,
    _last_mouse_pos = vec2(0.0, 0.0),
    _current_angle_x = 0,
    _current_angle_y = 0
}

function mathsign(number)
    return(number > 0 and 1) or (number == 0 and 0) or -1
end

function camera:new(object)
    object = object or {}
    setmetatable(object, self)
    self.__index = self
    return object
end

function camera:on_wake()
    self.camera_c = self.find_component("camera")
    self.camera_c:set_position(vec3(0, 0, -1))
    self.camera_c:set_target(vec3(0, 0, -1))
end

function camera:get_direction()
    if Input.get_key_pressed(Input.Key.W) then
        self._direction = vec3(self._direction.x, -1.0, self._direction.z)
    end

    if Input.get_key_pressed(Input.Key.S) then
        self._direction = vec3(self._direction.x, 1.0, self._direction.z)
    end

    if Input.get_key_pressed(Input.Key.A) then
        self._direction = vec3(-1.0, self._direction.y, self._direction.z)
    end

    if Input.get_key_pressed(Input.Key.D) then
        self._direction = vec3(1.0, self._direction.y, self._direction.z)
    end

    if Input.get_key_pressed(Input.Key.Z) then
        self._direction = vec3(self._direction.x, self._direction.y, -1.0)
    end

    if Input.get_key_pressed(Input.Key.X) then
        self._direction = vec3(self._direction.x, self._direction.y,  1.0)
    end

    if Input.get_key_pressed(Input.Key.P) then
        Scene.switch(0)
    end
    if Input.get_key_pressed(Input.Key.L) then
        Scene.switch(1)
    end
end

function camera:on_update(time)
    if Input.get_key_pressed(Input.Key.L) then
        Log.info("FPS: " .. time:get_fps())
    end

    self:get_direction()

    self._middle_pressed = false
end

function camera:on_post_update(time)
    local current_mouse_pos = Input.get_mouse_position()

    if Input.get_mouse_pressed(Input.MouseButton.MOUSE_MIDDLE) then
        local delta_mouse = vec2(current_mouse_pos.x - self._last_mouse_pos.x, current_mouse_pos.y - self._last_mouse_pos.y)
        self._last_mouse_pos = vec2(current_mouse_pos.x, current_mouse_pos.y)
        
        local dx = mathsign(delta_mouse.x)
        local dy = mathsign(delta_mouse.y)

        local turning_speed = 5
        self._current_angle_x = self._current_angle_x + turning_speed * dx * time:get_delta_time()
        self._current_angle_y = self._current_angle_y + turning_speed * dy * time:get_delta_time()
        self._current_angle_y = math.min(math.max(self._current_angle_y, -0.95), 0.95)

        new_target = vec3(
            math.sin(self._current_angle_x),
            math.sin(self._current_angle_y),
            -math.cos(self._current_angle_y) * math.cos(self._current_angle_x) 
        )

        self.camera_c:set_target(vec3(new_target.x, new_target.y, new_target.z))
    end

    -- Position
    local position = self.camera_c:get_position()
    local target = self.camera_c:get_target()

    local new_position = vec3(
        position.x + self._direction.x * time:get_delta_time(), 
        position.y + self._direction.y * time:get_delta_time(),
        position.z + self._direction.z * time:get_delta_time()
    )
    
    self.camera_c:set_position(vec3(new_position.x, new_position.y, new_position.z))

    -- Log.info(new_position.x .. ' ' .. new_position.y .. ' ' .. new_position.z)

    local p = self.camera_c:get_target()
    self._direction = vec3(0.0, 0.0, 0.0)
end

function camera:on_window_resize(width, height)
    if self._mode == "ortho" then
        self.camera_c:set_ortho(0.0, (width / self._pixel_per_meter), 0.0, (height / self._pixel_per_meter), -2, 2)
    else
        self.camera_c:set_perspective(90.0, width / height, 0.1, 100)
    end
end

return camera