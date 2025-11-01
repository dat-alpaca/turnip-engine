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

-- * -- * -- * Audio -- * -- * -- *
---@class AudioSourceComponent
AudioSourceComponent = {}

function AudioSourceComponent.play() end

-- * -- * -- * Logging -- * -- * -- *
---@class Log
Log = {}

---@param message string
---Outputs a fatal error message to stderr and calls the system breakpoint
function Log.critical(message) end

---@param message string
---Outputs an error message to stderr
function Log.error(message) end

---@param message string
---Outputs a warning message to stdout
function Log.warn(message) end

---@param message string
---Outputs a debug message to stdout
function Log.debug(message) end

---@param message string
---Outputs a tracing message to stdout
function Log.trace(message) end

---@param message string
---Outputs an information message to stdout
function Log.info(message) end

-- * -- * -- * Registry -- * -- * -- *
---@param componentName string
---@return UUIDComponent | NameComponent | TransformComponent | AudioSourceComponent | nil
---Searches componentName in the current script entity's registry
function find_component(componentName) end

-- * -- * -- * Input -- * -- * -- *
---@class input
Input = {}

---@class MouseButton
---@field MOUSE_UNKNOWN     MouseButton
---@field MOUSE_1           MouseButton
---@field MOUSE_2           MouseButton
---@field MOUSE_3           MouseButton
---@field MOUSE_4           MouseButton
---@field MOUSE_5           MouseButton
---@field MOUSE_6           MouseButton
---@field MOUSE_7           MouseButton
---@field MOUSE_8           MouseButton
---@field MOUSE_LEFT        MouseButton
---@field MOUSE_RIGHT       MouseButton
MouseButton = {}

---@class Key
---@field MOUSE_UNKNOWN        Key
---@field SPACE                Key
---@field APOSTROPHE           Key
---@field COMMA                Key
---@field MINUS                Key
---@field PERIOD               Key
---@field SLASH                Key
---@field K0                   Key
---@field K1                   Key
---@field K2                   Key
---@field K3                   Key
---@field K4                   Key
---@field K5                   Key
---@field K6                   Key
---@field K7                   Key
---@field K8                   Key
---@field K9                   Key
---@field SEMICOLON            Key
---@field EQUAL                Key
---@field A                    Key
---@field B                    Key
---@field C                    Key
---@field D                    Key
---@field E                    Key
---@field F                    Key
---@field G                    Key
---@field H                    Key
---@field I                    Key
---@field J                    Key
---@field K                    Key
---@field L                    Key
---@field M                    Key
---@field N                    Key
---@field O                    Key
---@field P                    Key
---@field Q                    Key
---@field R                    Key
---@field S                    Key
---@field T                    Key
---@field U                    Key
---@field V                    Key
---@field W                    Key
---@field X                    Key
---@field Y                    Key
---@field Z                    Key
---@field LEFT_BRACKET         Key
---@field BACKSLASH            Key
---@field RIGHT_BRACKET        Key
---@field GRAVE_ACCENT         Key
---@field ESCAPE               Key
---@field ENTER                Key
---@field TAB                  Key
---@field BACKSPACE            Key
---@field INSERT               Key
---@field DELETE               Key
---@field RIGHT                Key
---@field LEFT                 Key
---@field DOWN                 Key
---@field UP                   Key
---@field PAGE_UP              Key
---@field PAGE_DOWN            Key
---@field HOME                 Key
---@field END                  Key
---@field CAPS_LOCK            Key
---@field SCROLL_LOCK          Key
---@field NUM_LOCK             Key
---@field PRINT_SCREEN         Key
---@field PAUSE                Key
---@field F1                   Key
---@field F2                   Key
---@field F3                   Key
---@field F4                   Key
---@field F5                   Key
---@field F6                   Key
---@field F7                   Key
---@field F8                   Key
---@field F9                   Key
---@field F10                  Key
---@field F11                  Key
---@field F12                  Key
---@field F13                  Key
---@field F14                  Key
---@field F15                  Key
---@field F16                  Key
---@field F17                  Key
---@field F18                  Key
---@field F19                  Key
---@field F20                  Key
---@field F21                  Key
---@field F22                  Key
---@field F23                  Key
---@field F24                  Key
---@field KP_0                 Key
---@field KP_1                 Key
---@field KP_2                 Key
---@field KP_3                 Key
---@field KP_4                 Key
---@field KP_5                 Key
---@field KP_6                 Key
---@field KP_7                 Key
---@field KP_8                 Key
---@field KP_9                 Key
---@field KP_DECIMAL           Key
---@field KP_DIVIDE            Key
---@field KP_MULTIPLY          Key
---@field KP_SUBTRACT          Key
---@field KP_ADD               Key
---@field KP_ENTER             Key
---@field KP_EQUAL             Key
---@field LEFT_SHIFT           Key
---@field LEFT_CONTROL         Key
---@field LEFT_ALT             Key
---@field LEFT_SUPER           Key
---@field RIGHT_SHIFT          Key
---@field RIGHT_CONTROL        Key
---@field RIGHT_ALT            Key
---@field RIGHT_SUPER          Key
---@field MENU                 Key
Key = {}


---@return vec2 | nil
---Uses the input polling mechanism to fetch the current mouse position
function Input.get_mouse_position() end

---@param mouseButton MouseButton
---@return boolean
---Uses the input polling mechanism to fetch whether mouseButton is pressed
function Input.get_mouse_pressed(mouseButton) end

---@param keyIdentifier Key
---@return boolean
---Uses the input polling mechanism to fetch whether keyIdentifier is pressed
function Input.get_key_pressed(keyIdentifier) end
