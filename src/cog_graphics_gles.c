#include "cog_graphics_gles.h"

#include <stdlib.h>
#include <time.h> //TODO:Remove

#include <png.h>

#include "gles_util/esUtil.h"

#include "cog_log.h"
#include "cog_math.h"
#include "cog_text_freetype.h"
//#include "cog_text_gles.h" //TODO:Remove
#include "cog_window.h"

GLuint program_object;
GLuint index_object;

static GLuint _load_shader(GLenum type, const char* shader_src);
static GLuint cog_graphics_gles_load_texture_png(const char* file_name, int* width, int* height);

//-------------Public functions
void cog_graphics_gles_draw_sprite(cog_sprite* sprite, uint32_t idx) {
    /*
    clock_t begin, end;
    double time_spent;

    begin = clock();
    for(int i=0;i<50;i++) {
        GLfloat v_vertices[] = {0.0f,  0.5f, 0.0f,
                                -0.5f, -0.5f, 0.0f,
                                0.5f, -0.5f, 0.0f};
        // No clientside arrays, so do this in a webgl-friendly manner
        GLuint vertex_pos_object;
        glGenBuffers(1, &vertex_pos_object);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_pos_object);
        glBufferData(GL_ARRAY_BUFFER, 9*4, v_vertices, GL_STATIC_DRAW);
        // Set the viewport
        //glViewport ( 0, 0, esContext->width, esContext->height );
        // Clear the color buffer
        glClear(GL_COLOR_BUFFER_BIT);
        // Use the program object
        glUseProgram(program_object);
        // Load the vertex data
        glBindBuffer(GL_ARRAY_BUFFER, vertex_pos_object);
        glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    cog_debugf("time_spent %lf", time_spent);
    //cog_debugf(" fps %lf", 1.0/time_spent);
    */

    //Quad
    GLfloat v_vertices[] = { -0.5,  0.5, 0.0,  // Position 0
                           -0.5, -0.5, 0.0,  // Position 1
                            0.5, -0.5, 0.0,  // Position 2
                            0.5,  0.5, 0.0,  // Position 3
    };
    GLushort indices[] = {0, 1, 2, 0, 2, 3};

    // No clientside arrays, so do this in a webgl-friendly manner
    // vertex pos
    GLuint vertex_pos_object;
    glGenBuffers(1, &vertex_pos_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_pos_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v_vertices), v_vertices, GL_STATIC_DRAW);
    // vertex indices
    glGenBuffers(1, &index_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set the viewport
    //glViewport ( 0, 0, esContext->width, esContext->height );
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    // Use the program object
    glUseProgram(program_object);
    // Load the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vertex_pos_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_object);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(0);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void cog_graphics_gles_init(cog_window* window) {
    GLbyte v_shader_str[] =
        "attribute vec4 vPosition;    \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vPosition;  \n"
        "}                            \n";
    GLbyte f_shader_str[] =
        "precision mediump float;\n"\
        "void main()                                  \n"
        "{                                            \n"
        "  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
        "}                                            \n";
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLint linked;
    // Load the vertex/fragment shaders
    vertex_shader = _load_shader(GL_VERTEX_SHADER, v_shader_str);
    fragment_shader = _load_shader(GL_FRAGMENT_SHADER, f_shader_str);
    // Create the program object
    program_object = glCreateProgram();
    glAttachShader(program_object, vertex_shader);
    glAttachShader(program_object, fragment_shader);
    // Bind vPosition to attribute 0
    glBindAttribLocation(program_object, 0, "vPosition");
    // Link the program
    glLinkProgram(program_object);
    // Check the link status
    glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
    if(!linked) {
        GLint info_len = 0;
        glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &info_len);
        if(info_len > 1) {
            char* info_log = malloc(sizeof(char) * info_len);
            glGetProgramInfoLog(program_object, info_len, NULL, info_log);
            esLogMessage("Error linking program:\n%s\n", info_log);
            free(info_log);
        }
        glDeleteProgram(program_object);
    }
    // Store the program object
    program_object = program_object;
    glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
}

void cog_graphics_gles_draw_text(cog_text* text) {
}

void cog_graphics_gles_flush() {
}

void cog_graphics_gles_clear() {
}

void cog_graphics_gles_draw() {
}

uint32_t cog_graphics_gles_load_texture(const char* filename, int* width, int* height) {
    return cog_graphics_gles_load_texture_png(filename, 0, 0); //TODO: Don't duplicate
}

void cog_graphics_gles_prepare(uint32_t amount) {
}

void cog_graphics_gles_set_camera_pos(cog_pos2* pos) {
}

//-------------Private functions
static GLuint _load_shader(GLenum type, const char* shader_src) {
    GLuint shader;
    GLint compiled;
    // Create the shader object
    shader = glCreateShader(type);
    if(shader == 0) {
        return 0;
    }
    // Load the shader source
    glShaderSource(shader, 1, &shader_src, NULL);
    // Compile the shader
    glCompileShader(shader);
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint info_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        if(info_len > 1) {
            char* info_log = malloc(sizeof(char) * info_len);
            glGetShaderInfoLog(shader, info_len, NULL, info_log);
            esLogMessage("Error compiling shader:\n%s\n", info_log);
            free(info_log);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

//TODO:Don't duplicate this in 2 places
/*-----------------------------------------------------------------------------
 * Loads a transparent png image as a texure and uploads it to the GPU.
 * Returns: A reference to the texture which can be passed to glBindTexture
 *
 * Notes:
 * Source: https://github.com/DavidEGrayson/ahrs-visualizer/blob/master/png_texture.cpp
 * License: https://github.com/DavidEGrayson/ahrs-visualizer/blob/master/LICENSE.txt
 *-----------------------------------------------------------------------------*/
static GLuint cog_graphics_gles_load_texture_png(const char* file_name, int* width, int* height) {
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
