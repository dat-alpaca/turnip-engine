local player = {
    _description = "Default player",
    
    transform_c = nil,
    audio_c = nil,
    body2d_c = nil,

    _direction = vec2(0.0, 0.0)
}

function player:new(object)
    object = object or {}
    setmetatable(object, self)
    self.__index = self
    return object
end

function player:on_wake()
    self.transform_c = self.find_component("transform")
    self.audio_c = self.find_component("audio_source")
    self.body2d_c = self.find_component("body2d") 

    self.body2d_c:set_type(Physics.BodyType.DYNAMIC)
end

function player:on_update(time)
    local mouse_pos = Input.get_mouse_position()
    if Input.get_mouse_pressed(Input.MouseButton.MOUSE_LEFT) then
        self.transform_c.position.x = mouse_pos.x / _G["pixel_per_meter"]
        self.transform_c.position.y = mouse_pos.y / _G["pixel_per_meter"]
        
        local zero = vec2(0, 0)
        self.body2d_c:set_linear_velocity(zero)
    end

    if Input.get_mouse_pressed(Input.MouseButton.MOUSE_RIGHT) then
        self.audio_c:play()
    end

    self:get_direction()
end

function player:on_fixed_update()
    self:move()
end

function player:get_direction()
    if Input.get_key_pressed(Input.Key.UP) then
        self._direction = vec2(self._direction.x, -1.0)
    end

    if Input.get_key_pressed(Input.Key.DOWN) then
        self._direction = vec2(self._direction.x, 1.0)
    end

    if Input.get_key_pressed(Input.Key.LEFT) then
        self._direction = vec2(-1.0, self._direction.y)
    end

    if Input.get_key_pressed(Input.Key.RIGHT) then
        self._direction = vec2(1.0, self._direction.y)
    end
end

function player:move()
    local mag = 20.0
    local force = vec2(self._direction.x * mag, self._direction.y * mag)
    self.body2d_c:add_force(force)

    self._direction = vec2(0.0, 0.0)
end

function player:on_contact_begin(otherEntity)
    local force = vec2(0.0, 0.0)
    self.body2d_c:add_force(force)
end

return player