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
