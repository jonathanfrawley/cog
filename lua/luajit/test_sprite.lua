local cog = require 'cog'
local ffi = require("ffi")

cog.init()
local COG_PI = 3.14

local test_sprite_id = cog.sprite.add("media/test0.png")
local test = cog.sprite.new()
test.dim = cog.dim2.new(1.0, 1.0)
cog.sprite.set(test_sprite_id, test)

while not cog.hasquit() do
    cog.loopstep()
end
cog.quit()
