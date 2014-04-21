#include "cog_graphics_gles.h"

#include <stdlib.h>
#include <time.h> //TODO:Remove

#include "gles_util/esUtil.h"

#include "cog_log.h"
#include "cog_math.h"
#include "cog_text_freetype.h"
//#include "cog_text_gles.h" //TODO:Remove
#include "cog_window.h"

GLuint program_object;
GLuint index_object;

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
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };


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

uint32_t cog_graphics_gles_load_texture(const char* filename, int* width, int* height) {
}

void cog_graphics_gles_clear() {
}

void cog_graphics_gles_draw() {
}

void cog_graphics_gles_prepare(uint32_t amount) {
}

void cog_graphics_gles_set_camera_pos(cog_pos2* pos) {
}
