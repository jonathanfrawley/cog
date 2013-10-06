#include "cog.h"

#include <ft2build.h>
#include FT_FREETYPE_H

FT_Face face;
FT_GlyphSlot g;

void render_text(const char *text, float x, float y, float sx, float sy) {
  const char *p;
 
  for(p = text; *p; p++) {
    if(FT_Load_Char(face, *p, FT_LOAD_RENDER))
        continue;
 
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_ALPHA,
      g->bitmap.width,
      g->bitmap.rows,
      0,
      GL_ALPHA,
      GL_UNSIGNED_BYTE,
      g->bitmap.buffer
    );
 
/* 
    GLfloat box[4][4] = {
        {x2,     -y2    , 0, 0},
        {x2 + w, -y2    , 1, 0},
        {x2,     -y2 - h, 0, 1},
        {x2 + w, -y2 - h, 1, 1},
    };
 
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
 
*/
    float x2 = x + g->bitmap_left * sx;
    float y2 = -y - g->bitmap_top * sy;
    float w = g->bitmap.width * sx;
    float h = g->bitmap.rows * sy;

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glLoadIdentity();
    glTranslatef(x2, y2, 0.0);
    //glColor4f(1.0, 1.0, 1.0, text->alpha);

    GLfloat vertices[] = {
        -1.0f * w, 1.0f * h, 0,
        1.0f * w, 1.0f * h, 0,
        1.0f * w, -1.0f * h, 0,
        -1.0f * w, -1.0f * h, 0,
    };
    GLfloat tex[] = { 1, 0, 0, 0, 0, 1, 1, 1 };
    GLubyte indices[] = { 3, 0, 1,      // first triangle (bottom left - top left - top right)
                          3, 1, 2       // second triangle (bottom left - top right - bottom right)
                        };
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    //Restore alpha to normal
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();

    x += (g->advance.x >> 6) * sx;
    y += (g->advance.y >> 6) * sy;
  }
}

int main(void) {
    cog_init();

    //Source: http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
    FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        cog_errorf("Could not init freetype library\n");
        return 1;
    }

    if(FT_New_Face(ft, "media/font/ArcadeClassic.ttf", 0, &face)) {
        cog_errorf("Could not open font\n");
        return 1;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);
    if(FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
        cog_errorf("Could not load character 'X'\n");
        return 1;
    }
    g = face->glyph;

    GLuint tex;

    glClearColor(1, 1, 1, 1);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
     
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    while(!cog_hasquit()) {
        cog_loopstep();
        glClear(GL_COLOR_BUFFER_BIT);
        render_text("TESToiwejoiwhoiwhoiwh", 0, 0, 1.0, 1.0);
    }
    cog_quit();
    return 0;
}

