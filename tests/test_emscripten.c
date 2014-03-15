#include "cog.h"

#include "emscripten.h"
#include "SDL/SDL_image.h" //TODO: Remove
#include "SDL/SDL_opengl.h" //TODO: Remove

#include "stdio.h" //TODO: Remove
#include "string.h" //TODO: Remove

void render(void) {
    //Begin test rendering code
    glClearColor( 0, 0, 0, 0 );

    glEnable( GL_TEXTURE_2D ); // Needed when we're using the fixed-function pipeline.

    glViewport( 0, 0, 640, 480 );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix(); // just for testing
    glLoadIdentity();

    glOrtho( 0, 640, 480, 0, -1, 1 );

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
        memset(surface->pixels, 0x66, surface->w*surface->h);

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
        glRotatef(30, 0.0f, 0.0f, 1.0f);
        glScalef(1.5, 1.0, 1.0);
        glBegin( GL_QUADS );
        glTexCoord2i( 0, 0 ); glVertex3f( 10, 10, 0 );
        glTexCoord2i( 1, 0 ); glVertex3f( 300, 10, 0 );
        glTexCoord2i( 1, 1 ); glVertex3f( 300, 128, 0 );
        glTexCoord2i( 0, 1 ); glVertex3f( 10, 128, 0 );
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    SDL_GL_SwapBuffers();
}

void main_loop(void) {
    cog_debugf("HI");
}

int main(int argc, char* argv[]) {
    cog_init();
/*
    while(!cog_hasquit()) {
        cog_loopstep();
    }
*/
    render();
    emscripten_set_main_loop(main_loop, 0, 0);
    cog_quit();
    return 0;
}
