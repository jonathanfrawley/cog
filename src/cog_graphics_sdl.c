#include "cog_graphics_sdl.h"

#include <stdlib.h>

#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>

#include "cog_log.h"
#include "cog_math.h"
#include "cog_window.h"


void cog_graphics_sdl_draw_sprite(cog_sprite* sprite, uint32_t idx) {
    cog_debugf("Drawing sprite <%d>", sprite->id);
    
    glBindTexture(GL_TEXTURE_2D, sprite->tex_id);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
        glTranslatef(sprite->pos.x, sprite->pos.y, 0.0f);
        glRotatef(-cog_math_radians_to_degrees(sprite->rot), 0.0f, 0.0f, 1.0f);
        glScalef(sprite->dim.w, sprite->dim.h, 1.0f);
        int x_start = -1;
        int x_end = 1;
        int y_start = -1.0;
        int y_end = 1;
        double tex_x_start = sprite->tex_pos.x;
        double tex_x_end = sprite->tex_pos.x + sprite->tex_dim.w;
        double tex_y_start = sprite->tex_pos.y;
        double tex_y_end = sprite->tex_pos.y + sprite->tex_dim.h;
        glBegin(GL_QUADS);
        glTexCoord2i(tex_x_start, tex_y_start); glVertex3f(x_start, y_start, 0);
        glTexCoord2i(tex_x_end, tex_y_start); glVertex3f(x_end, y_start, 0);
        glTexCoord2i(tex_x_end, tex_y_end); glVertex3f(x_end, y_end, 0);
        glTexCoord2i(tex_x_start, tex_y_end); glVertex3f(x_start, y_end, 0);
        glEnd();
    glPopMatrix();
}

void cog_graphics_sdl_init(cog_window* window) {
    glClearColor(1.0f, 0.3f, 0.3f, 0.3f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, 1.0, -1.0, -1, 1);
    //glOrtho( 0, 640, 480, 0, -1, 1 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
}

void cog_graphics_sdl_draw_text(cog_text* text) {
}

void cog_graphics_sdl_flush() {
    SDL_GL_SwapBuffers();
}

uint32_t cog_graphics_sdl_load_texture(const char* filename, int* width, int* height) {
    GLuint texture; // Texture object handle
    SDL_Surface *surface; // Gives us the information to make the texture
    if ((surface = IMG_Load(filename))) {
        // Check that the image's width is a power of 2
        if((surface->w & (surface->w - 1)) != 0 ) {
            cog_errorf("image's width is not a power of 2\n");
        }
        // Also check if the height is a power of 2
        if((surface->h & (surface->h - 1)) != 0 ) {
            cog_errorf("image's height is not a power of 2\n");
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
}

void cog_graphics_sdl_prepare(uint32_t amount) {
}

void cog_graphics_sdl_set_camera_pos(cog_pos2* pos) {
    glTranslatef(pos->x, pos->y, 0.0f);
}

