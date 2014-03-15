#include "cog_graphics_sdl.h"

#include <stdlib.h>

#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>

#include "cog_log.h"
#include "cog_window.h"


void cog_graphics_sdl_draw_sprite(cog_sprite* sprite, uint32_t idx) {
    //cog_debugf("Drawing sprinte <%d>", sprite->id);
    


    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Load the OpenGL texture

    GLuint texture; // Texture object handle
    SDL_Surface *surface; // Gives us the information to make the texture

    if ( (surface = IMG_Load("screenshot.png")) ) {
        // Check that the image's width is a power of 2
        if ( (surface->w & (surface->w - 1)) != 0 ) {
            printf("warning: image.bmp's width is not a power of 2\n");
        }
        // Also check if the height is a power of 2
        if ( (surface->h & (surface->h - 1)) != 0 ) {
            printf("warning: image.bmp's height is not a power of 2\n");
        }

// Have OpenGL generate a texture object handle for us
        glGenTextures( 1, &texture );

        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, texture );

        // Set the texture's stretching properties
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        //SDL_LockSurface(surface);

        // Add some greyness
        //memset(surface->pixels, 0x66, surface->w*surface->h);

        // Edit the texture object's image data using the information SDL_Surface gives us
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );

        //SDL_UnlockSurface(surface);
    }
    else {
        printf("SDL could not load image.bmp: %s\n", SDL_GetError());
        SDL_Quit();
    }

    // Free the SDL_Surface only if it was successfully created
    if ( surface ) {
        SDL_FreeSurface( surface );
    }

    // Clear the screen before drawing
    glClear( GL_COLOR_BUFFER_BIT );

    // Bind the texture to which subsequent calls refer to
    glBindTexture( GL_TEXTURE_2D, texture );

    glPushMatrix();
        //glRotatef(30, 0.0f, 0.0f, 1.0f);
        //glScalef(1.5, 1.0, 1.0);
        /*
        int x_start = 10;
        int x_end = 300;
        int y_start = 10;
        int y_end = 128;
        */
        int x_start = -1;
        int x_end = 1;
        int y_start = -1.0;
        int y_end = 1;
        glBegin( GL_QUADS );
        glTexCoord2i(0, 0); glVertex3f(x_start, y_start, 0);
        glTexCoord2i(1, 0); glVertex3f(x_end, y_start, 0);
        glTexCoord2i(1, 1); glVertex3f(x_end, y_end, 0);
        glTexCoord2i(0, 1); glVertex3f(x_start, y_end, 0);
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

    if ( (surface = IMG_Load(filename)) ) {
        // Check that the image's width is a power of 2
        if((surface->w & (surface->w - 1)) != 0 ) {
            cog_errorf("image's width is not a power of 2\n");
        }
        // Also check if the height is a power of 2
        if((surface->h & (surface->h - 1)) != 0 ) {
            cog_errorf("image's height is not a power of 2\n");
        }

        // Have OpenGL generate a texture object handle for us
        glGenTextures( 1, &texture );

        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, texture );

        // Set the texture's stretching properties
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        //SDL_LockSurface(surface);

        // Edit the texture object's image data using the information SDL_Surface gives us
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

        //SDL_UnlockSurface(surface);
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
}

