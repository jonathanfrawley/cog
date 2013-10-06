#include "cog_graphics.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_video.h>

#include <png.h>

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
//SDL_Surface* cog_graphics_load_image(const char* filename);

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
    //TODO: Remove this maybe
    //SDL_Surface* result;
    tempsurface = IMG_Load(filename);
    if(!tempsurface) {
        fprintf(stderr, "Cannot load image file <%s> : <%s>", filename,
                SDL_GetError());
        return 0;
    }
    //if((result = SDL_ConvertSurface(tempsurface, ->format, NULL)) == NULL) {
    //    perror(SDL_GetError());
    //}
    //SDL_FreeSurface(tempsurface);
    //return result;
    return tempsurface;
}

GLuint cog_graphics_png_texture_load(const char* filename, int* width, int*  height) {
    png_byte header[8];

    FILE *fp = fopen(filename, "rb");
    if(fp == 0) {
        perror(filename);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if(png_sig_cmp(header, 0, 8)) {
        fprintf(stderr, "error: %s is not a PNG.\n", filename);
        fclose(fp);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if(!end_info) {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // the code in this if statement gets called if libpng encounters an error
    if(setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);

    if(width) {  
        *width = temp_width; 
    }
    if(height) { 
        *height = temp_height; 
    }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte * image_data;
    image_data = malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if(image_data == NULL) {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep * row_pointers = malloc(temp_height * sizeof(png_bytep));
    if(row_pointers == NULL) {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    int i;
    for (i = 0; i < temp_height; i++) {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // Generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_width, temp_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    return texture;
}

GLuint cog_graphics_load_texture(char* filename) {
    GLuint texture = cog_graphics_png_texture_load(filename, NULL, NULL);
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Check for error
    error = glGetError();
    if(error != GL_NO_ERROR) {
        printf("Error initializing OpenGL! %s\n", gluErrorString(error));
    }
    //Initialize Modelview Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Check for error
    error = glGetError();
    if(error != GL_NO_ERROR) {
        printf("Error initializing OpenGL! %s\n", gluErrorString(error));
    }
    //Initialize clear color
    glClearColor(0.3f, 0.1f, 0.1f, 1.f);
    //Check for error
    error = glGetError();
    if(error != GL_NO_ERROR) {
        printf("Error initializing OpenGL! %s\n", gluErrorString(error));
    }
    //SDL_Surface* surface = cog_graphics_load_image("media/test0.png");
    //GLUINT = cog_graphics_upload_surface(surface);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
/*
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
*/

    GLUINT = cog_graphics_load_texture("media/kitten_anim.png");
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

void cog_graphics_render(cog_window* window) {
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
    glClear(GL_COLOR_BUFFER_BIT);
    //PASTE
    for(cog_int i = 0; i < COG_LAYER_MAX; i++) {
        cog_sprite_draw_layer(i);
        cog_anim_draw_layer(i);
        cog_text_draw_layer(i);
    }
    GLenum error = GL_NO_ERROR;
    error = glGetError();
    if(error != GL_NO_ERROR) {
        cog_errorf("Error initializing OpenGL! %s\n", gluErrorString(error));
    }
    //Render quad
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, GLUINT);
    //glColor4f(1.0, 1.0, 1.0, 1.0); // reset gl color
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(0.5f, 0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.5f, 0.5f);
    glEnd();
    glLoadIdentity();
    glDisable(GL_TEXTURE_2D);
    //Restore alpha to normal
    //glColor4f(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
#if defined(HAVE_GLES)
    EGL_SwapBuffers();
#endif
    /*
    //Software
    SDL_Surface* surface = cog_graphics_load_image("media/test0.png");
    SDL_BlitSurface(surface, NULL, window->screen_surface, NULL);
    SDL_FreeSurface(surface);
    */
}
