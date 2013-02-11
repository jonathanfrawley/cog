#ifndef  COG_GRAPHICS_H
#define  COG_GRAPHICS_H

// GLEW
#if !defined(HAVE_GLES)
//#include <GL/gl.h>
#include <GL/glew.h>
#else
#include <GL/gl.h>
#include <GLES/gl.h>
#include "eglport.h"
#endif

#include <SDL/SDL.h>

#include "cog_sprite.h"
#include "cog_text.h"

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
