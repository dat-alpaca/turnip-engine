require "base"

transform_c = nil
audio_c = nil

function on_wake()
    transform_c = find_component("transform")
    if transform_c ~= nil then
        local position = transform_c.transform.position
        Log.info("(" .. position.x .. ", " .. position.y .. ", " .. position.z .. ")")
    end

    audio_c = find_component("audio_source")
    if audio_c == nil then
        Log.info("Failed to find audio source")
    end
end

function on_update()
    local mouse_pos = Input.get_mouse_position()
    if mouse_pos == nil then
        return
    end

    local leftPressed = Input.get_mouse_pressed(MouseButton.MOUSE_LEFT)
    if leftPressed then
        audio_c:play()
    end

    local rightPressed = Input.get_mouse_pressed(MouseButton.MOUSE_RIGHT)
    if rightPressed then
        Log.info("Mouse pressed at location: " .. mouse_pos.x .. ", " .. mouse_pos.y)

        if transform_c ~= nil then
            transform_c.transform.position.x = mouse_pos.x
            transform_c.transform.position.y = mouse_pos.y
        end
    end

    local enterPressed = Input.get_key_pressed(Key.ENTER)
    if enterPressed then
        Log.info("Key ENTER pressed")

        transform_c.transform.rotation.z = transform_c.transform.rotation.z + 0.9
    end
end
