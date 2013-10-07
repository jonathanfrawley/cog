#ifndef  COG_TEXT_H
#define  COG_TEXT_H

#if !defined(HAVE_GLES)
#include <GL/glew.h>
#else
#include <GLES/gl.h>
#endif

#include <ft2build.h>
#include FT_FREETYPE_H


#include "cog_core.h"
#include "cog_types.h"

#define COG_TEXT_LAYER 3

/*-----------------------------------------------------------------------------
 *  Represents some text
 *-----------------------------------------------------------------------------*/
typedef struct {
    cog_text_id id;
    GLuint tex_id;
    FT_Face face;
    cog_uint layer;

    //User attributes
    cog_pos2 pos;
    cog_dim2 dim;
    cog_float rot;
    cog_color col;
    cog_char str[COG_TEXT_MAX];
} cog_text;

cog_text_id cog_text_add();
cog_text* cog_text_get(cog_text_id id);
void cog_text_remove(cog_text_id id);
void cog_text_removeall(void);
void cog_text_set(cog_text_id id, cog_text src);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_text_init(void);
void cog_text_draw_layer(cog_uint layer);

#endif // COG_TEXT_H
