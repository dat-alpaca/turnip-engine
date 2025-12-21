require "logging"
require "audio"
require "input"

-- * -- * -- * Base Types -- * -- * -- *
---@class UUID
---@field uuid number

---@class Name
---@field name string

---@class vec2
---@field x number
---@field y number

---@class vec3
---@field x number
---@field y number
---@field z number

---@class vec4
---@field x number
---@field y number
---@field z number
---@field w number

---@class Transform
---@field position vec3
---@field rotation vec3
---@field scale vec3

---@alias PossibleComponent UUID | Name | Transform | AudioSourceComponent

-- * -- * -- * Registry -- * -- * -- *
---@param name string
---@return PossibleComponent | nil
---Returns a component by name
function find_component(name) end

-- * -- * -- * Time -- * -- * -- *
Time = {}

---@return number
---Returns the previous frame's delta time
function Time:get_delta_time() end

---@return number
---Returns the previous frame's frames per second
function Time:get_fps() end

---@return number
---Returns the application's accumulated time in seconds
function Time:get_accumulated() end

---@return number
---Returns the application's accumulated time in milliseconds.
function Time:get_accumulated_ms() end

return {}
