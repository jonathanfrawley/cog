

typedef unsigned cog_snd_id;

//main
void cog_init();
void cog_mainloop();
void cog_loopstep();
void cog_destroy();

//anim
typedef struct cog_sprite
{
}cog_sprite;
cog_sprite* cog_add_anim(char* animimg, int milliseconds, ...);
void cog_play_anim(cog_sprite* sprite);

//sound
cog_snd_id cog_sound_load(char* fname);
void cog_sound_play(cog_snd_id snd);
int cog_sound_isfinished(cog_snd_id snd);
