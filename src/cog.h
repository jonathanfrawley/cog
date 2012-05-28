
//types
typedef float cog_float;
typedef int cog_sprite_id;
typedef int cog_anim_id;
typedef unsigned cog_snd_id;
typedef unsigned cog_uint;
//main
void cog_init();
void cog_mainloop();
void cog_loopstep();
void cog_destroy();

//anim
cog_anim_id cog_add_anim(char* animimg, cog_uint transition_millis, ...);
void cog_play_anim(cog_anim_id id);

//sound
cog_snd_id cog_sound_load(char* fname);
void cog_sound_play(cog_snd_id snd);
int cog_sound_isfinished(cog_snd_id snd);
