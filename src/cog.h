
//main
void cog_init();
void cog_mainloop();
void cog_destroy();

//anim
typedef struct cog_sprite
{
}cog_sprite;
cog_sprite* cog_add_anim(char* animimg, int milliseconds, ...);
void cog_play_anim(cog_sprite* sprite);
