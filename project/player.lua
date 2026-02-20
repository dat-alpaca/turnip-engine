local player = {
    _description = "Default player",
    
    transform_c = nil,
    audio_c = nil,
    body2d_c = nil,

    _direction = vec2(0.0, 0.0)
}

camera = nil

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

    camera = self.get_entity(4):find_component("camera")
end

function player:on_update(time)
    local mouse_pos = Input.get_mouse_position()

    if Input.get_mouse_pressed(Input.MouseButton.MOUSE_LEFT) then
        local world_position = camera:get_world_position(vec3(mouse_pos.x, mouse_pos.y, 1))
        -- self.transform_c.position.x = world_position.x
        -- self.transform_c.position.y = world_position.y

        local zero = vec2(0, 0)
        self.body2d_c:set_linear_velocity(zero)
    end

    if Input.get_key_pressed(Input.Key.H) then
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