import cog

cog.init()

kitten_anim_id = cog.anim_add("media/kitten_anim.png", 3)
cog.anim_set_frames(kitten_anim_id, 0, 1, 2)
kitten = cog.anim_get(kitten_anim_id)
kitten.dim = cog.dim2()
kitten.dim.w = 0.3
kitten.dim.h = 0.3
kitten.transition_millis = 150
kitten.looped = True
kitten.ang_vel = cog.COG_PI / 14000
kitten.vel = cog.vec2()
kitten.vel.x = 0.0001
kitten.vel.y = 0.0003

while not cog.hasquit():
    cog.loopstep()
    if (kitten.pos.x > 1.0) or (kitten.pos.x < -1.0):
        kitten.vel.x = -kitten.vel.x

    if (kitten.pos.y > 1.0) or (kitten.pos.y < -1.0):
        kitten.vel.y = -kitten.vel.y
cog.quit()
