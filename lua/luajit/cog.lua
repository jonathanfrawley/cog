local ffi = require("ffi")
local C = ffi.load('./libcogshared.so')
ffi.cdef[[
void cog_init();
void cog_mainloop();
void cog_quit();
]]
return C

