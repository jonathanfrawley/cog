import cog

cog.init()

test_sprite_id = cog.sprite_add("media/test0.png")
test = cog.sprite()
test.dim = cog.dim2()
test.dim.w = 1.0
test.dim.h = 1.0
cog.sprite_set(test_sprite_id, test)

while not cog.hasquit():
    cog.loopstep()
cog.quit()
