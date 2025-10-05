require "base"

function on_wake()
    local name_c = find_component("name")
    if name_c ~= nil then
        Log.info("Name: " .. name_c.name)
    end

    local uuid_c = find_component("uuid")
    if uuid_c ~= nil then
        Log.info("UUID: " .. tostring(uuid_c.uuid))
    end

    local transform_c = find_component("transform")
    if transform_c ~= nil then
        local position = transform_c.transform.position
        Log.info("(" .. position.x .. ", " .. position.y .. ", " .. position.z .. ")")
    end
end

function on_update()
    local mouse_pos = Input.get_mouse_position()
    if mouse_pos == nil then
        return
    end

    local rightPressed = Input.get_mouse_pressed(MouseButton.MOUSE_RIGHT)
    if rightPressed then
        Log.info("Mouse pressed at location: " .. mouse_pos.x .. ", " .. mouse_pos.y)
    end

    local enterPressed = Input.get_key_pressed(Key.ENTER)
    if enterPressed then
        Log.info("Key ENTER pressed")
    end
end
