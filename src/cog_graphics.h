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

#include <SDL2/SDL.h>

#include "cog_sprite.h"
#include "cog_text.h"
#include "cog_window.h"

#define COG_LAYER_MAX 16

void cog_graphics_render(cog_window* window);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_graphics_init(void);
void cog_graphics_draw_sprite(cog_sprite* sprite);
void cog_graphics_draw_text(cog_text* text);
GLuint cog_graphics_load_texture(char* filename, int* width, int* height);

#endif // COG_GRAPHICS_H
