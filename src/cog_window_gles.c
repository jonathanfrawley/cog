#include "cog_window_gles.h"

#include <stdio.h> //TODO:Remove
#include <stdlib.h> //TODO:Remove

#include "gles_util/esUtil.h"

#include "cog_log.h"

void cog_window_gles_init(cog_config config, cog_window* window) {
    int width = config.window_w;
    int height = config.window_h;
    if(config.window_w == 0) {
        width = 320;
        height = 240;
    }
    ESContext esContext;
    esInitContext(&esContext);
    esCreateWindow(&esContext, "cog", width, height, ES_WINDOW_RGB);
}

void cog_window_gles_update(cog_window* window) {
}

void cog_window_gles_quit(cog_window* window) {
}

void cog_window_gles_toggle_fullscreen(cog_window* window) {
}

#ifdef TEST
GLuint LoadShader(GLenum type, const char* shaderSrc) {
    GLuint shader;
    GLint compiled;
    // Create the shader object
    shader = glCreateShader(type);
    if(shader == 0) {
        return 0;
    }
    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);
    // Compile the shader
    glCompileShader(shader);
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1) {
            char* infoLog = malloc(sizeof(char) * infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            esLogMessage("Error compiling shader:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint programObject;

int Init() {
    GLbyte vShaderStr[] =
        "attribute vec4 vPosition;    \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vPosition;  \n"
        "}                            \n";
    GLbyte fShaderStr[] =
        "precision mediump float;\n"\
        "void main()                                  \n"
        "{                                            \n"
        "  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
        "}                                            \n";
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;
    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);
    // Create the program object
    programObject = glCreateProgram();
    if(programObject == 0) {
        return 0;
    }
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    // Bind vPosition to attribute 0
    glBindAttribLocation(programObject, 0, "vPosition");
    // Link the program
    glLinkProgram(programObject);
    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if(!linked) {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1) {
            char* infoLog = malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            esLogMessage("Error linking program:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteProgram(programObject);
        return GL_FALSE;
    }
    // Store the program object
    programObject = programObject;
    glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
    return GL_TRUE;
}

void Draw() {
    GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                             -0.5f, -0.5f, 0.0f,
                             0.5f, -0.5f, 0.0f
                          };
    // No clientside arrays, so do this in a webgl-friendly manner
    GLuint vertexPosObject;
    glGenBuffers(1, &vertexPosObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
    glBufferData(GL_ARRAY_BUFFER, 9*4, vVertices, GL_STATIC_DRAW);
    // Set the viewport
    //TODO: Get this from window
    glViewport(0, 0, 320, 240);
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);
    // Use the program object
    glUseProgram(programObject);
    // Load the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
    glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
    glEnableVertexAttribArray(0);
    printf("yo1");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    printf("yo2");
}


int main(int argc, char* argv[]) {
    cog_window window;
    cog_window_gles_init((cog_config) {}, &window);
    //Begin test rendering code
    if(!Init()) {
        return 0;
    }
    //esRegisterDrawFunc ( &esContext, Draw );
    Draw();
    //End test rendering code
}
#endif
