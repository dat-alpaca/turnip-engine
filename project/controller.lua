local controller = {
}

function controller:new(object)
    object = object or {}
    setmetatable(object, self)
    self.__index = self
    return object
end

function player:on_wake()
    
end

return controller