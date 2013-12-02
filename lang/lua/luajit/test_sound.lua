local cog = require 'cog'

cog.init()
snd = cog.snd.add("media/testsnd.wav")
cog.snd.play(snd)
while not cog.hasquit() do
    cog.loopstep()
end
cog.quit()
