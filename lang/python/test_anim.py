import cog

cog.init()

x_dim = 0.1
y_dim = 0.1
kittens = []
for i in range(0, 10):
    kitten_anim_id = cog.anim_add("media/kitten_anim.png", 3)
    cog.anim_set_frames(kitten_anim_id, 0, 1, 2)
    kitten = cog.anim_get(kitten_anim_id)
    kitten.dim = cog.dim2()
    kitten.pos.x = x_dim * i - 1.0
    kitten.dim.h = y_dim * i - 1.0
    kitten.dim.w = x_dim
    kitten.dim.h = y_dim
    kitten.transition_millis = 150
    kitten.looped = True
    kitten.ang_vel = cog.COG_PI / 14000
    kitten.vel = cog.vec2()
    kitten.vel.x = 0.0001
    kitten.vel.y = 0.0003
    kittens.append(kitten_anim_id)

while not cog.hasquit():
    import time
    start = time.time()
    cog.loopstep()
    for kitten_id in kittens:
        kitten = cog.anim_get(kitten_id)
        if (kitten.pos.x > 1.0) or (kitten.pos.x < -1.0):
            kitten.vel.x = -kitten.vel.x

        if (kitten.pos.y > 1.0) or (kitten.pos.y < -1.0):
            kitten.vel.y = -kitten.vel.y
    end = time.time()
    delta = end-start
    print("FPS <{}> Loop time <{}>".format(1.0/delta, delta))
cog.quit()
