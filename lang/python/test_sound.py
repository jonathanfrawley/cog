import cog

cog.init()
snd = cog.snd_add("media/testsnd.wav")
cog.snd_play(snd)
while not cog.hasquit():
    cog.loopstep()
cog.quit()
