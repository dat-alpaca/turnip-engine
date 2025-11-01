require "logging"
require "audio"

-- * -- * -- * Base Types -- * -- * -- *
---@class UUID
---@field uuid number

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

-- * -- * -- * Component Types -- * -- * -- *
---@class UUIDComponent
---@field uuid UUID

---@class NameComponent
---@field name string

---@class TransformComponent
---@field transform Transform

---@alias PossibleComponent UUIDComponent | NameComponent | TransformComponent | AudioSourceComponent

-- * -- * -- * Registry -- * -- * -- *
---@param name string
---@return PossibleComponent | nil
---Returns a component by name
function find_component(name) end
