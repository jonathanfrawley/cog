#include "cog_graphics_gles.h"

#include <stdlib.h>
#include <time.h> //TODO:Remove
#include <math.h>

#include <png.h>
#include <SDL/SDL.h>

#include "gles_util/esUtil.h"

#include "cog_log.h"
#include "cog_math.h"
#include "cog_text_freetype.h"
//#include "cog_text_gles.h" //TODO:Remove
#include "cog_window.h"

static cog_window* window;
static GLuint program_object;
static GLuint vertex_pos_object;
static GLuint index_object;
static GLuint sampler_loc;
static GLuint position_loc;
static GLuint tex_coord_loc;

static GLuint _load_shader(GLenum type, const char* shader_src);
static GLuint cog_graphics_gles_load_texture_png(const char* file_name, int* width, int* height);
double cog_graphics_gles_round_w(double n);
double cog_graphics_gles_round_h(double n);

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
    double rot = sprite->rot + COG_PI/4;
    //uint32_t offset = idx * vertex_amount; //TODO
    uint32_t offset = 0;
    //1.5 here is due to cos and sin rotation below.
    double w = sprite->dim.w * 1.415;
    double h = sprite->dim.h * 1.415;
    double x_offset = sprite->pos.x;
    double y_offset = sprite->pos.y;
    if(sprite->pixel_perfect) {
        x_offset = cog_graphics_gles_round_w(sprite->pos.x);
        y_offset = cog_graphics_gles_round_h(sprite->pos.y);
    }
/*
    GLfloat v_vertices[] = { -0.5,  0.5, 0.0,  // Position 0
        0.0,  0.0,       // TexCoord 0
        -0.5, -0.5, 0.0,  // Position 1
        0.0,  1.0,       // TexCoord 1
        0.5, -0.5, 0.0,  // Position 2
        1.0,  1.0,       // TexCoord 2
        0.5,  0.5, 0.0,  // Position 3
        1.0,  0.0        // TexCoord 3
    };
    */
    GLfloat vertices[12 + 8];
    //topleft
    vertices[offset + 0] = -1.0f * w * sin(rot) + x_offset;
    vertices[offset + 1] = 1.0f * h * cos(rot) + y_offset;
    vertices[offset + 2] = 0;
    vertices[offset + 3] = sprite->tex_pos.x;
    vertices[offset + 4] = sprite->tex_pos.y;
    //topright
    vertices[offset + 5] = 1.0f * w * cos(rot) + x_offset;
    vertices[offset + 6] = 1.0f * h * sin(rot) + y_offset;
    vertices[offset + 7] = 0;
    vertices[offset + 8] = sprite->tex_pos.x + sprite->tex_dim.w;
    vertices[offset + 9] = sprite->tex_pos.y;
    //bottom right
    vertices[offset + 10] = 1.0f * w * sin(rot) + x_offset;
    vertices[offset + 11] = -1.0f * h * cos(rot) + y_offset;
    vertices[offset + 12] = 0;
    vertices[offset + 13] = sprite->tex_pos.x + sprite->tex_dim.w;
    vertices[offset + 14] = sprite->tex_pos.y + sprite->tex_dim.h;
    //bottom left
    vertices[offset + 15] = -1.0f * w * cos(rot) + x_offset;
    vertices[offset + 16] = -1.0f * h * sin(rot)+ y_offset;
    vertices[offset + 17] = 0;
    vertices[offset + 18] = sprite->tex_pos.x;
    vertices[offset + 19] = sprite->tex_pos.y + sprite->tex_dim.h;

    GLushort indices[] = {3, 0, 1, 3, 1, 2};

    //Quad
    sampler_loc = glGetUniformLocation(program_object, "s_texture");
    position_loc = glGetAttribLocation(program_object, "a_position");
    tex_coord_loc = glGetAttribLocation(program_object, "a_texCoord");

    //1) Generate buffers
    // No clientside arrays, so do this in a webgl-friendly manner
    // vertex pos
    glGenBuffers(1, &vertex_pos_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_pos_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // vertex indices
    glGenBuffers(1, &index_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //2) Drawing phase
    // Use the program object
    glUseProgram(program_object);
    // Load the vertex position
    glBindBuffer(GL_ARRAY_BUFFER, vertex_pos_object);
    glVertexAttribPointer(position_loc, 3, GL_FLOAT,
            GL_FALSE, 5 * 4, 0 );
    // Load the texture coordinate
    glVertexAttribPointer(tex_coord_loc, 2, GL_FLOAT,
            GL_FALSE, 5 * 4, 
            3 * 4 );

    glEnableVertexAttribArray(position_loc);
    glEnableVertexAttribArray(tex_coord_loc);
    // Set the viewport
    //glViewport ( 0, 0, esContext->width, esContext->height );
    // Clear the color buffer

    //3) tex
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite->tex_id);
    glUniform1i(sampler_loc, 0);
    //4) Draw the things!
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_object);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void cog_graphics_gles_init(cog_window* win) {
    GLbyte v_shader_str[] =
        "attribute vec4 a_position;   \n"
        "attribute vec2 a_texCoord;   \n"
        "varying vec2 v_texCoord;     \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = a_position; \n"
        "   v_texCoord = a_texCoord;  \n"
        "}                            \n";
    GLbyte f_shader_str[] =
        "precision mediump float;                            \n"
        "varying vec2 v_texCoord;                            \n"
        "uniform sampler2D s_texture;                        \n"
        "void main()                                         \n"
        "{                                                   \n"
        "  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
        "}                                                   \n";
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
    //glBindAttribLocation(program_object, 0, "vPosition");
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
    glClearColor(0.2f, 0.1f, 0.0f, 0.0f);
    window = win;
}

void cog_graphics_gles_draw_text(cog_text* text) {
    glUseProgram(program_object);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    cog_text_freetype* text_ft = cog_text_freetype_get(text->id);
    const char* p;
    FT_GlyphSlot g = text_ft->face->glyph;
    double x = text->pos.x;
    //double x = ((text->pos.x + 1.0) + * 512);
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
        cog_color c = text->col;

        uint32_t offset = 0;
        GLfloat vertices[12 + 8];
        //topleft
        vertices[offset + 0] = x2;
        vertices[offset + 1] = -y2;
        vertices[offset + 2] = 0;
        vertices[offset + 3] = 0;
        vertices[offset + 4] = 0;
        //topright
        vertices[offset + 5] = x2 + w;
        vertices[offset + 6] = -y2;
        vertices[offset + 7] = 0;
        vertices[offset + 8] = 1;
        vertices[offset + 9] = 0;
        //bottom right
        vertices[offset + 10] = x2 + w;
        vertices[offset + 11] = -y2 - h;
        vertices[offset + 12] = 0;
        vertices[offset + 13] = 1;
        vertices[offset + 14] = 1;
        //bottom left
        vertices[offset + 15] = x2;
        vertices[offset + 16] = -y2 - h;
        vertices[offset + 17] = 0;
        vertices[offset + 18] = 0;
        vertices[offset + 19] = 1;

        GLushort indices[] = {3, 0, 1, 3, 1, 2};

        //TODO: Put in its own function
        sampler_loc = glGetUniformLocation(program_object, "s_texture");
        position_loc = glGetAttribLocation(program_object, "a_position");
        tex_coord_loc = glGetAttribLocation(program_object, "a_texCoord");

        //1) Generate buffers
        // No clientside arrays, so do this in a webgl-friendly manner
        // vertex pos
        glGenBuffers(1, &vertex_pos_object);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_pos_object);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // vertex indices
        glGenBuffers(1, &index_object);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        //2) Drawing phase
        // Use the program object
        glUseProgram(program_object);
        // Load the vertex position
        glBindBuffer(GL_ARRAY_BUFFER, vertex_pos_object);
        glVertexAttribPointer(position_loc, 3, GL_FLOAT,
                GL_FALSE, 5 * 4, 0 );
        // Load the texture coordinate
        glVertexAttribPointer(tex_coord_loc, 2, GL_FLOAT,
                GL_FALSE, 5 * 4, 
                3 * 4 );

        glEnableVertexAttribArray(position_loc);
        glEnableVertexAttribArray(tex_coord_loc);
        // Set the viewport
        //glViewport ( 0, 0, esContext->width, esContext->height );
        // Clear the color buffer

        //3) tex
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(sampler_loc, 0);
        //4) Draw the things!
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_object);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
        //END TODO

        x += (g->advance.x >> 6) * sx;
        y += (g->advance.y >> 6) * sy;
    }
    //Restore alpha to normal
}

void cog_graphics_gles_flush() {
}

uint32_t cog_graphics_gles_gen_tex_id() {
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    return tex_id;
}

void cog_graphics_gles_clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

void cog_graphics_gles_draw() {
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
uint32_t cog_graphics_gles_load_texture(const char* filename, int* width, int* height) {
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
    } else {
        cog_errorf("Could not load file at : %s", filename);
    }
    return texture;
}

double cog_graphics_gles_round_w(double n) {
    double w = window->w/2;
    double ret = n+1.0;
    ret = ret*w;
    ret = (ret > (floor(ret)+0.5)) ? ceil(ret) : floor(ret);
    ret = ret/w;
    return ret-1.0;
}

double cog_graphics_gles_round_h(double n) {
    double h = window->h/2;
    double ret = n+1.0;
    ret = ret*h;
    ret = (ret > (floor(ret)+0.5)) ? ceil(ret) : floor(ret);
    ret = ret/h;
    return ret-1.0;
}

