#ifndef  COG_GRAPHICS_H
#define  COG_GRAPHICS_H

#include <GL/glew.h>
#include <SDL/SDL.h>

#include "cog_sprite.h"
#include "cog_text.h"

#define COG_LAYER_MAX 4

typedef struct
{
    GLuint vertid;
    GLuint fragid;
    GLuint vertbuffid;
    GLuint vertorderbuffid;
    GLuint programid;
} cog_renderer;

void cog_graphics_render();

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_graphics_init(void);
void cog_graphics_draw_sprite(cog_sprite* sprite);
void cog_graphics_draw_text(cog_text* text);
GLuint cog_graphics_load_texture(char* filename);
GLuint cog_graphics_upload_surface(SDL_Surface* image);

#endif   // COG_GRAPHICS_H
