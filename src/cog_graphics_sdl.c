#include "cog_graphics_sdl.h"

#include <stdlib.h>

#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>

#include "cog_log.h"
#include "cog_math.h"
#include "cog_text_freetype.h"
#include "cog_text_sdl.h" //TODO:Remove
#include "cog_window.h"

static bool is_init;

void cog_graphics_sdl_draw_sprite(cog_sprite* sprite, uint32_t idx) {
    /*
    if(!is_init) {
        glBindTexture(GL_TEXTURE_2D, sprite->tex_id);
        glBegin(GL_QUADS);
        is_init = true;
    }
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //glPushMatrix();
    glTranslatef(sprite->pos.x, -sprite->pos.y, 0.0f);
    glRotatef(-cog_math_radians_to_degrees(sprite->rot), 0.0f, 0.0f, 1.0f);
    glScalef(sprite->dim.w, sprite->dim.h, 1.0f);
    //int x_start = -1;
    //int x_end = 1;
    //int y_start = -1;
    //int y_end = 1;
    int x_start = -1;
    int x_end = 1;
    int y_start = -1;
    int y_end = 1;
    double tex_x_start = sprite->tex_pos.x;
    double tex_x_end = sprite->tex_pos.x + sprite->tex_dim.w;
    double tex_y_start = sprite->tex_pos.y;
    double tex_y_end =  sprite->tex_pos.y + sprite->tex_dim.h;
    //glBegin(GL_QUADS);
    glTexCoord2f(tex_x_start, tex_y_start);
    glVertex3f(x_start, y_start, 0);
    glTexCoord2f(tex_x_end, tex_y_start);
    glVertex3f(x_end, y_start, 0);
    glTexCoord2f(tex_x_end, tex_y_end);
    glVertex3f(x_end, y_end, 0);
    glTexCoord2f(tex_x_start, tex_y_end);
    glVertex3f(x_start, y_end, 0);
    //glEnd();
    //glPopMatrix();
    */
    GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                             -0.5f, -0.5f, 0.0f,
                             0.5f, -0.5f, 0.0f
                          };
    // No clientside arrays, so do this in a webgl-friendly manner
    GLuint vertexPosObject;
    glGenBuffers(1, &vertexPosObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
    glBufferData(GL_ARRAY_BUFFER, 9*4, vVertices, GL_STATIC_DRAW);
    // Set the viewport
    //glViewport ( 0, 0, esContext->width, esContext->height );
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    // Use the program object
    //glUseProgram ( userData->programObject );
    // Load the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
    glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void cog_graphics_sdl_init(cog_window* window) {
    glClearColor(1.0f, 0.3f, 0.3f, 0.3f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, 1.0, -1.0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
}

void cog_graphics_sdl_draw_text(cog_text* text) {
    /*
    glPushMatrix();
    //Bind texture
    cog_text_sdl* text_sdl = cog_text_sdl_get(text->id);
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text_sdl->tex_id);

    //Translate
    glTranslatef(text->pos.x+text->dim.w,text->pos.y+text->dim.h, 0.0);
    //Rotation not supported atm
    //glRotatef( -cog_math_radtodeg(text->rot), 0.0f, 0.0f, 1.0f );

    glBegin(GL_QUADS);                      // Draw A Quad
        glTexCoord2f(0.0f,
                1.0f);
        glVertex2f(-1.0f*text->dim.w,
                1.0f*text->dim.h);              // Top Left
        glTexCoord2f(1.0f,
                1.0f);
        glVertex2f(1.0f*text->dim.w,
                1.0f*text->dim.h);              // Top Right
        glTexCoord2f(1.0f,
                0.0f);
        glVertex2f(1.0f*text->dim.w,
                -1.0f*text->dim.h);             // Bottom Right
        glTexCoord2f(0.0f,
                0.0f);
        glVertex2f(-1.0f*text->dim.w,
                -1.0f*text->dim.h);             // Bottom left
    glEnd();
    glPopMatrix();
    */
    cog_text_freetype* text_ft = cog_text_freetype_get(text->id);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, text_ft->tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    const char* p;
    FT_GlyphSlot g = text_ft->face->glyph;
    double x = text->pos.x ;
    //double x = ((text->pos.x + 1.0) + * 512);
    double y = text->pos.y;
    double sx = text->scale.w;
    double sy = text->scale.h;
    double scalar_y = 0.15; //TODO : Figure out a more generic way to find scale
    double row_height = text_ft->face->descender * sy * scalar_y;
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glLoadIdentity();
    cog_color c = text->col;
    glColor4d(c.r, c.g, c.b, c.a);
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
    glBegin(GL_QUADS);
    for(p = text->str; *p; p++) {
        int new_line = x > (text->pos.x + text->dim.w) || (*p) == '\n';
        if(new_line) {
            x = text->pos.x;
            y += row_height;
            if((*p) == '\n') {
                continue;
            }
        }
        if(FT_Load_Char(text_ft->face, *p, FT_LOAD_RENDER)) {
            continue;
        }
        /*
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
        */
        double x2 = x + g->bitmap_left * sx;
        //double y2 = y + g->bitmap_top * sy;
        //double y2 = y - (g->bitmap_top * sy) + (g->bitmap_top * sy)/4.0;
        double y2 = y;
        double w = g->bitmap.width * sx;
        double h = g->bitmap.rows * sy;
        /*
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glLoadIdentity();
        cog_color c = text->col;
        glColor4d(c.r, c.g, c.b, c.a);
        */
        //glBegin(GL_QUADS);
        float z = 0.1;
        glTexCoord2f(0, 1);
        glVertex3f(x2, -y2, z);
        glTexCoord2f(1, 1);
        glVertex3f(x2 + w, -y2, z);
        glTexCoord2f(1, 0);
        glVertex3f(x2 + w, -y2 - h, z);
        glTexCoord2f(0, 0);
        glVertex3f(x2, -y2 - h, z);
        //glEnd();
        x += (g->advance.x >> 6) * sx;
        y += (g->advance.y >> 6) * sy;
    }
    glEnd();
    //glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //Restore alpha to normal
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
}

void cog_graphics_sdl_flush() {
    SDL_GL_SwapBuffers();
}

uint32_t cog_graphics_sdl_load_texture(const char* filename, int* width, int* height) {
    GLuint texture; // Texture object handle
    SDL_Surface* surface; // Gives us the information to make the texture
    if((surface = IMG_Load(filename))) {
        // Check that the image's width is a power of 2
        if((surface->w & (surface->w - 1)) != 0) {
            cog_errorf("Image's width is not a power of 2 : %s, width %d", filename, surface->w);
        }
        // Also check if the height is a power of 2
        if((surface->h & (surface->h - 1)) != 0) {
            cog_errorf("Image's height is not a power of 2: %s, height %d", filename, surface->h);
        }
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    }
    return texture;
}

void cog_graphics_sdl_clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void cog_graphics_sdl_draw() {
    glEnd();
    glPopMatrix();
}

void cog_graphics_sdl_prepare(uint32_t amount) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    is_init = false;
}

void cog_graphics_sdl_set_camera_pos(cog_pos2* pos) {
    glTranslatef(pos->x, pos->y, 0.0f);
}

int32_t _to_nearest_pow2(int32_t n) {
    int x = 1;
    while(x < n) {
        x <<= 1;
    }
    return x;
}

uint32_t cog_graphics_sdl_upload_surface(SDL_Surface* image) {
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    int w = _to_nearest_pow2(image->w);
    int h = _to_nearest_pow2(image->h);
    cog_debugf("w is %d h is %d", w, h);
    // Check that the image's width is a power of 2
    if((w & (w - 1)) != 0) {
        cog_errorf("Image's width is not a power of 2 : width %d", w);
    }
    // Also check if the height is a power of 2
    if((h & (h - 1)) != 0) {
        cog_errorf("Image's height is not a power of 2: height %d", h);
    }
    /*
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int rmask = 0xff000000;
    int gmask = 0x00ff0000;
    int bmask = 0x0000ff00;
    int amask = 0x000000ff;
    #else
    int rmask = 0x000000ff;
    int gmask = 0x0000ff00;
    int bmask = 0x00ff0000;
    int amask = 0xff000000;
    #endif
    SDL_Surface* alphaimage = SDL_CreateRGBSurface(SDL_SWSURFACE,
            w,h,32,
            rmask,gmask,bmask,amask);
    if(!alphaimage) {
        cog_errorf("cog_graphics_upload_surface : RGB surface creation failed.");
    }
    // Set up so that colorkey pixels become transparent :
    Uint32 colorkey = SDL_MapRGBA( alphaimage->format, rmask, gmask, bmask, amask );
    SDL_FillRect(alphaimage, 0, colorkey);
    colorkey = SDL_MapRGBA(image->format, rmask, gmask, bmask, amask);
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, colorkey);
    SDL_Rect area;
    SDL_SetAlpha(image, 0, amask); //http://www.gamedev.net/topic/518525-opengl--sdl--transparent-image-make-textures/
    // Copy the surface into the GL texture image :
    area.x = 0;
    area.y = 0;
    area.w = w;
    area.h = h;
    SDL_BlitSurface( image, &area, alphaimage, &area );
    // Create an OpenGL texture for the image
    GLuint textureID;
    glGenTextures( 1, &textureID );
    glBindTexture( GL_TEXTURE_2D, textureID );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D,
            0,
            GL_RGBA,
            w,
            h,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            alphaimage->pixels );
    SDL_FreeSurface(alphaimage);
    return textureID;
    */
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 w,
                 h,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 image->pixels);
    return textureID;
}
