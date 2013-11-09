local cog = require 'cog'
local ffi = require 'ffi'

cog.cog_init()
local COG_PI = 3.14

local kitten_anim_id = cog.cog_anim_add("media/kitten_anim.png", 3);
local kitten = ffi.new("cog_anim")
kitten.dim = ffi.new("cog_dim2", 0.3, 0.3)
kitten.transition_millis = 150
kitten.looped = true
kitten.ang_vel = COG_PI/14000
kitten.vel = ffi.new("cog_vec2", 0.0001, 0.0003)

cog.cog_anim_set(kitten_anim_id, kitten) 
cog.cog_anim_set_frames(kitten_anim_id, 0, 1, 2)

while not cog.cog_hasquit() do
    cog.cog_loopstep();

    local kitten = cog.cog_anim_get(kitten_anim_id);
    if ((kitten.pos.x > 1.0) or (kitten.pos.x < -1.0)) then
        kitten.vel.x = -kitten.vel.x
    end

    if ((kitten.pos.y > 1.0) or (kitten.pos.y < -1.0)) then
        kitten.vel.y = -kitten.vel.y
    end
end
cog.cog_quit();
