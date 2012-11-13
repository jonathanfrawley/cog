#ifndef  COG_TEXT_H
#define  COG_TEXT_H

#include <GL/glew.h>
#include <SDL/SDL_ttf.h>

#include "cog_types.h"

/*-----------------------------------------------------------------------------
 *  Represents some text
 *-----------------------------------------------------------------------------*/
typedef struct
{
    cog_text_id id;
    TTF_Font* font;
    cog_uint ptsize;
    SDL_Color colour;
    cog_float x;
    cog_float y;
    cog_float rot;
    cog_float w;
    cog_float h;
    GLuint texid;
} cog_text;

cog_text_id cog_text_add(char* text);
cog_text* cog_text_get(cog_text_id id);
void cog_text_remove(cog_text_id id);
void cog_text_removeall(void);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_text_init(void);
void cog_text_draw(void);

#endif   // COG_TEXT_H
