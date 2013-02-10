#ifndef  COG_TEXT_H
#define  COG_TEXT_H

#if !defined(HAVE_GLES)
#include <GL/glew.h>
#else
#include <GLES/gl.h>
#endif
#include <SDL/SDL_ttf.h>

#include "cog_core.h"
#include "cog_types.h"

#define COG_TEXT_MAX 255

/*-----------------------------------------------------------------------------
 *  Represents some text
 *-----------------------------------------------------------------------------*/
typedef struct
{
    cog_text_id id;
    TTF_Font* font;
    cog_uint ptsize;
    SDL_Color c;
    cog_float x;
    cog_float y;
    cog_float rot;
    cog_float w;
    cog_float h;
    GLuint texid;
    char str[COG_TEXT_MAX];
} cog_text;

cog_text_id cog_text_add(char* text);
cog_text* cog_text_get(cog_text_id id);
void cog_text_refresh(cog_text_id id);
void cog_text_remove(cog_text_id id);
void cog_text_removeall(void);
void cog_text_set_str(cog_text_id id, char* str);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_text_init(void);
void cog_text_draw(void);

#endif   // COG_TEXT_H
