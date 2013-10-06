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

void cog_graphics_hwinit(void);
//file io
void cog_graphics_read_file(char* buf, char* filename);
//rendering
void cog_graphics_render();
GLuint cog_graphics_load_texture_png(const char* file_name, int* width, int* height);

/*-----------------------------------------------------------------------------
 * Sprites are drawn centred at the sprite's x and y coord, as opposed to most
 * engines where they are drawn from the top left.
 *-----------------------------------------------------------------------------*/
void cog_graphics_draw_sprite(cog_sprite* sprite) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sprite->tex_id);
    glLoadIdentity();
    glTranslatef(sprite->pos.x, sprite->pos.y, 0.0);
    glRotatef(cog_math_radians_to_degrees(sprite->rot), 0.0f, 0.0f, 1.0f);
    float w = sprite->dim.w;
    float h = sprite->dim.h;
    GLfloat vertices[] = {
        -1.0f * w, 1.0f * h, 0,
        1.0f * w, 1.0f * h, 0,
        1.0f * w, -1.0f * h, 0,
        -1.0f * w, -1.0f * h, 0,
    };
    GLfloat tex[] = {
        sprite->tex_pos.x, sprite->tex_pos.y + sprite->tex_dim.h,
        sprite->tex_pos.x + sprite->tex_dim.w,
        sprite->tex_pos.y + sprite->tex_dim.h,
        sprite->tex_pos.x + sprite->tex_dim.w, sprite->tex_pos.y,
        sprite->tex_pos.x, sprite->tex_pos.y
    };
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
    float w = text->dim.w;
    float h = text->dim.h;
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
}

/*-----------------------------------------------------------------------------
 * Loads a transparent png image as a texure and uploads it to the GPU.
 * Returns: A reference to the texture which can be passed to glBindTexture
 *
 * Notes:
 * Source: https://github.com/DavidEGrayson/ahrs-visualizer/blob/master/png_texture.cpp
 * License: https://github.com/DavidEGrayson/ahrs-visualizer/blob/master/LICENSE.txt
 *-----------------------------------------------------------------------------*/
GLuint cog_graphics_load_texture_png(const char* file_name, int* width, int* height) {
    png_byte header[8];
    FILE* fp = fopen(file_name, "rb");
    if(fp == 0) {
        perror(file_name);
        return 0;
    }
    // read the header
    fread(header, 1, 8, fp);
    if(png_sig_cmp(header, 0, 8)) {
        cog_errorf("error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr) {
        cog_errorf("error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }
    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        cog_errorf("error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }
    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if(!end_info) {
        cog_errorf("error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }
    // the code in this if statement gets called if libpng encounters an error
    if(setjmp(png_jmpbuf(png_ptr))) {
        cog_errorf("error from libpng\n");
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
    //printf("%s: %lux%lu %d\n", file_name, temp_width, temp_height, color_type);
    if(bit_depth != 8) {
        cog_errorf("%s: Unsupported bit depth %d. Must be 8.\n", file_name, bit_depth);
        return 0;
    }
    GLint format;
    switch(color_type) {
        case PNG_COLOR_TYPE_RGB:
            format = GL_RGB;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            format = GL_RGBA;
            break;
        default:
            cog_errorf("%s: Unknown libpng color type %d.\n", file_name, color_type);
            return 0;
    }
    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);
    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);
    // Allocate the image_data as a big block, to be given to opengl
    png_byte* image_data = (png_byte*)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if(image_data == NULL) {
        cog_errorf("error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }
    // row_pointers is for pointing to image_data for reading the png with libpng
    png_byte** row_pointers = (png_byte**)malloc(temp_height * sizeof(png_byte*));
    if(row_pointers == NULL) {
        cog_errorf("error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }
    // set the individual row_pointers to point at the correct offsets of image_data
    for(unsigned int i = 0; i < temp_height; i++) {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }
    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);
    // Generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, temp_width, temp_height, 0, format, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    return texture;
}

GLuint cog_graphics_load_texture(char* filename, int* width, int* height) {
    return cog_graphics_load_texture_png(filename, 0, 0);
}

void cog_graphics_init(void) {
    cog_graphics_hwinit();
}

void cog_graphics_hwinit(void) {
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
    glClearColor(0.3f, 0.f, 0.f, 1.f);
    //Check for error
    error = glGetError();
    if(error != GL_NO_ERROR) {
        printf("Error initializing OpenGL! %s\n", gluErrorString(error));
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    //Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //PASTE
    for(cog_int i = 0; i < COG_LAYER_MAX; i++) {
        cog_sprite_draw_layer(i);
        cog_anim_draw_layer(i);
        cog_text_draw_layer(i);
    }
}
