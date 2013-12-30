#include "cog_graphics_opengl.h"

#include <SDL2/SDL.h>

// GLEW
#if !defined(HAVE_GLES)
//#include <GL/gl.h>
#include <GL/glew.h>
#else
#include <GL/gl.h>
#include <GLES/gl.h>
#include "eglport.h"
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include <png.h>

#include <cog_log.h>
#include <cog_math.h>
#include <cog_main.h>
#include <cog_text_freetype.h>

GLuint cog_graphics_opengl_load_texture_png(const char* file_name, int* width, int* height);
cog_window* window;

void cog_graphics_opengl_draw_sprite(cog_sprite* sprite) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sprite->tex_id);
    glLoadIdentity();
    glTranslatef(sprite->pos.x, sprite->pos.y, 0.0);
    glRotatef(cog_math_radians_to_degrees(sprite->rot), 0.0f, 0.0f, 1.0f);
    double w = sprite->dim.w;
    double h = sprite->dim.h;
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

void cog_graphics_opengl_init(cog_window* win) {
    window = win;
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

void cog_graphics_opengl_draw_text(cog_text* text) {
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
    double x = text->pos.x;
    double y = text->pos.y;
    double sx = text->scale.w;
    double sy = text->scale.h;
    double scalar_y = 0.15; //TODO : Figure out a more generic way to find scale
    double row_height = text_ft->face->descender * sy * scalar_y;
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
        double x2 = x + g->bitmap_left * sx;
        //double y2 = y + g->bitmap_top * sy;
        double y2 = -y - g->bitmap_top * sy;
        double w = g->bitmap.width * sx;
        double h = g->bitmap.rows * sy;
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glLoadIdentity();
        cog_color c = text->col;
        glColor4f(c.r, c.g, c.b, c.a);
        //glColor4f(c.r, c.g, c.b, 0.0);
        GLfloat vertices[] = {
            x2, -y2, 0, //top left
            x2 + w, -y2, 0, //top right
            x2 + w, -y2 - h, 0, //bottom right
            x2, -y2 - h, 0 //bottom left
        };
        GLfloat tex[] = {0, 0, 1, 0, 1, 1, 0, 1};
        GLubyte indices[] = { 3, 0, 1,      // first triangle (bottom left - top left - top right)
                              3, 1, 2       // second triangle (bottom left - top right - bottom right)
                            };
        glTexCoordPointer(2, GL_FLOAT, 0, tex);
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
        x += (g->advance.x >> 6) * sx;
        y += (g->advance.y >> 6) * sy;
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    //Restore alpha to normal
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
}

uint32_t cog_graphics_opengl_gen_tex_id() {
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    return tex_id;
}

/*-----------------------------------------------------------------------------
 * Loads a transparent png image as a texure and uploads it to the GPU.
 * Returns: A reference to the texture which can be passed to glBindTexture
 *
 * Notes:
 * Source: https://github.com/DavidEGrayson/ahrs-visualizer/blob/master/png_texture.cpp
 * License: https://github.com/DavidEGrayson/ahrs-visualizer/blob/master/LICENSE.txt
 *-----------------------------------------------------------------------------*/
GLuint cog_graphics_opengl_load_texture_png(const char* file_name, int* width, int* height) {
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

uint32_t cog_graphics_opengl_load_texture(const char* filename, int* width, int* height) {
    return cog_graphics_opengl_load_texture_png(filename, 0, 0);
}


void cog_graphics_opengl_clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void cog_graphics_opengl_flush() {
    SDL_GL_SwapWindow(window->window);
}
