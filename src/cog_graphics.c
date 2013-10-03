#include "cog_graphics.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_video.h>

#include "cog.h"
#include "cog_anim.h"
#include "cog_core.h"
#include "cog_list.h"
#include "cog_math.h"
#include "cog_sprite.h"

void cog_graphics_hw_set_viewport(cog_int w, cog_int h);
void cog_graphics_hwinit(void);
void cog_graphics_swinit(void);
//file io
void cog_graphics_read_file(char* buf, char* filename);
//rendering
void cog_graphics_render();
SDL_Surface* cog_graphics_load_image(const char* filename);

/*-----------------------------------------------------------------------------
 * Sprites are drawn centred at the sprite's x and y coord, as opposed to most
 * engines where they are drawn from the top left.
 *-----------------------------------------------------------------------------*/
void cog_graphics_draw_sprite(cog_sprite* sprite) {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sprite->tex_id);
    glLoadIdentity();
    glTranslatef(sprite->pos.x, sprite->pos.y, 0.0);
    glRotatef(cog_math_radians_to_degrees(sprite->rot), 0.0f, 0.0f, 1.0f);
    float W = sprite->dim.w;
    float H = sprite->dim.h;
    GLfloat vertices[] = {
        -1.0f * W, 1.0f * H, 0,
        1.0f * W, 1.0f * H, 0,
        1.0f * W, -1.0f * H, 0,
        -1.0f * W, -1.0f * H, 0,
    };
    GLfloat tex[] = {
        sprite->tex_pos.x, sprite->tex_pos.y + sprite->tex_dim.h,
        sprite->tex_pos.x + sprite->tex_dim.w,
        sprite->tex_pos.y + sprite->tex_dim.h,
        sprite->tex_pos.x + sprite->tex_dim.w, sprite->tex_pos.y,
        sprite->tex_pos.x, sprite->tex_pos.y
    };
    GLubyte indices[] = { 3, 0, 1,      // first triangle (bottom left - top left - top right)
                          3, 1, 2
                        };                          // second triangle (bottom left - top right - bottom right)
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void cog_graphics_draw_text(cog_text* text) {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, text->tex_id);
    glLoadIdentity();
    glTranslatef(text->pos.x + text->dim.w, text->pos.y + text->dim.h, 0.0);
    glRotatef(cog_math_radians_to_degrees(text->rot), 0.0f, 0.0f, 1.0f);
    glColor4f(1.0, 1.0, 1.0, text->alpha);
    float W = text->dim.w;
    float H = text->dim.h;
    GLfloat vertices[] = {
        -1.0f * W, 1.0f * H, 0,
        1.0f * W, 1.0f * H, 0,
        1.0f * W, -1.0f * H, 0,
        -1.0f * W, -1.0f * H, 0,
    };
    GLfloat tex[] = { 1, 0, 0, 0, 0, 1, 1, 1 };
    GLubyte indices[] = { 3, 0, 1,      // first triangle (bottom left - top left - top right)
                          3, 1, 2
                        };                          // second triangle (bottom left - top right - bottom right)
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
}

SDL_Surface* cog_graphics_load_image(const char* filename) {
    //Loads an image and returns an SDL_Surface.
    SDL_Surface* tempsurface;
    //SDL_Surface* result;
    tempsurface = IMG_Load(filename);
    if(!tempsurface) {
        fprintf(stderr, "Cannot load image file <%s> : <%s>", filename,
                SDL_GetError());
        return 0;
    }
    /*
    if((result = SDL_ConvertSurface(tempsurface, ->format, NULL)) == NULL) {
        perror(SDL_GetError());
    }
    */
    //SDL_FreeSurface(tempsurface);
    //return result;
    return tempsurface;
}

GLuint cog_graphics_upload_surface(SDL_Surface* image) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    int w = image->w;
    int h = image->h;
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
    // Create the target alpha surface with correct colour component ordering
    SDL_Surface* alphaimage = SDL_CreateRGBSurface(SDL_SWSURFACE,
                              image->w, image->h, 32,
                              rmask, gmask, bmask,
                              amask);
    if(alphaimage == COG_NULL) {
        cog_errorf("cog_graphics_upload_surface : "
                   "RGB surface creation failed.");
        return -1;
    }
    // Set up so that colorkey pixels become transparent :
    Uint32 colorkey =
        SDL_MapRGBA(alphaimage->format, rmask, gmask, bmask, amask);
    SDL_FillRect(alphaimage, 0, colorkey);
    //colorkey = SDL_MapRGBA(image->format, rmask, gmask, bmask, amask);
    colorkey = SDL_MapRGBA(image->format, rmask, gmask, bmask, amask);
    SDL_SetColorKey(image, SDL_TRUE, colorkey);
    SDL_Rect area;
    //SDL_SetAlpha(image, 0, amask);      //http://www.gamedev.net/topic/518525-opengl--sdl--transparent-image-make-textures/
    //TODO: This function has been updated for SDL2.
    SDL_SetSurfaceAlphaMod(image, amask);
    // Copy the surface into the GL texture image :
    area.x = 0;
    area.y = 0;
    area.w = image->w;
    area.h = image->h;
    SDL_BlitSurface(image, &area, alphaimage, &area);
    // Create an OpenGL texture for the image
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Prepare the filtering of the texture image
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Map the alpha surface to the texture
    /*
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, alphaimage->pixels);
                 */
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, alphaimage->pixels);
    SDL_FreeSurface(alphaimage);
    return textureID;
}

GLuint cog_graphics_load_texture(char* filename) {
    SDL_Surface* image = cog_graphics_load_image(filename);
    GLuint texture = cog_graphics_upload_surface(image);
    SDL_FreeSurface(image);
    return texture;
}

void cog_graphics_init(void) {
    cog_graphics_hwinit();
//    cog_graphics_hw_set_viewport(cog_screenw(), cog_screenh());
}

void cog_graphics_hw_set_viewport(cog_int w, cog_int h) {
    /*  Height / width ration */
    /*
    GLfloat ratio;
    ratio = (GLfloat)w / (GLfloat)h;
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    */
}

int GLUINT = 0; //XXX REMOVE

void cog_graphics_hwinit(void) {
    /*
#ifdef HAVE_GLES
    //GLES
    cog_graphics_init_shaders();
    glUseProgram(shader_program);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, cog_screenw(), cog_screenh(), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#else
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, cog_screenw(), cog_screenh(), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    */
/*
        glEnable(GL_TEXTURE_2D);
        glShadeModel( GL_SMOOTH );
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        glClearDepth( 1.0f );
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LEQUAL );
        glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
        */
//#endif
	GLenum error = GL_NO_ERROR;

	//Initialize Projection Matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	
	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
	}

	//Initialize Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
	}
	
	//Initialize clear color
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	
	//Check for error
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
	}

    SDL_Surface* surface = cog_graphics_load_image("media/test0.png");
    GLUINT = cog_graphics_upload_surface(surface);
}

void cog_graphics_read_file(char* buf, char* filename) {
    FILE* fp;
    fp = fopen(filename, "r");
    if(fp == 0) {
        fprintf(stderr, "Can't open file %s", filename);
        perror("Exiting");
    }
    fseek(fp, 0, SEEK_END);
    int filesize = ftell(fp);
    rewind(fp);
    if(filesize > COG_MAX_FILE_BUF) {
        perror("File too big to be read in");
        goto cleanup;
    }
    int amountread = fread(buf,
                           sizeof(char),
                           COG_MAX_FILE_BUF,
                           fp);
    if(amountread != filesize) {
        perror("Error reading in file.");
        goto cleanup;
    }
cleanup:
    fclose(fp);
}

void cog_graphics_render() {
    /*
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    //PASTE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-1.5f, 0.0f, -6.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glEnd();
    glTranslatef(3.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glEnd();
    */
	//Clear color buffer
	glClear( GL_COLOR_BUFFER_BIT );
    //PASTE
    for(cog_int i = 0; i < COG_LAYER_MAX; i++) {
        cog_sprite_draw_layer(i);
        cog_anim_draw_layer(i);
        cog_text_draw_layer(i);
    }

	GLenum error = GL_NO_ERROR;
	error = glGetError();
	if( error != GL_NO_ERROR )
	{
		cog_errorf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
	}
	
	//Render quad
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, GLUINT);
    glColor4f(1.0, 1.0, 1.0, 1.0); // reset gl color
    glBegin( GL_QUADS );
    glVertex2f( -0.5f, -0.5f );
    glVertex2f( 0.5f, -0.5f );
    glVertex2f( 0.5f, 0.5f );
    glVertex2f( -0.5f, 0.5f );
    glEnd();
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    //Restore alpha to normal
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
#if defined(HAVE_GLES)
    EGL_SwapBuffers();
#endif
}
