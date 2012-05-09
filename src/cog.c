#include "cog.h"
#include <stdarg.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <GL/glew.h>

//constants
static int COG_MAX_BUF = 255;
static int COG_MAX_FILE_BUF = 4080;
//TODO: Get these from config.
static int COG_SCREEN_WIDTH = 640;
static int COG_SCREEN_HEIGHT = 480;
static GLfloat spritevertices[] = {
    1.0,  1.0,  1.0, 1.0,
    -1.0,  1.0,  1.0, 1.0,
    -1.0, -1.0,  1.0, 1.0,
    1.0, -1.0,  1.0, 1.0,
};
static GLushort spriteverticesorder[] = {
    0,1,2,3
};

//data structures
typedef struct
{
    int finished;
} cog_game;
static cog_game game;
typedef struct
{
    GLuint vertid;
    GLuint fragid;
    GLuint vertbuffid;
    GLuint vertorderbuffid;
} cog_renderer;
static cog_renderer renderer;


typedef struct
{
    SDL_Surface* screen;
} cog_window;
static cog_window window;

//prototypes
void cog_errorf(const char* logMsg, ...);
void cog_platform_init(void);
void cog_window_init(void);
void cog_window_togglefullscreen(void);
void cog_checkkeys(void);
void cog_graphics_init(void);
GLuint cog_graphics_load_shader(char* filename, GLenum shadertype);
void cog_read_file(char* buf, char* filename);
void cog_render();

//implementations
void cog_init()
{
    game.finished = 0;
    cog_platform_init();
    cog_window_init();
    cog_graphics_init();
}

void cog_mainloop()
{
    while(!game.finished)
    {
        cog_checkkeys();
        cog_render();
    }
}

void cog_destroy()
{
    game.finished = 1;
}

//local
void cog_errorf(const char* logMsg, ...)
{
    va_list ap;
    va_start(ap, logMsg);
    char buf[COG_MAX_BUF];
    vsprintf(buf, logMsg, ap);
    printf("CRITICAL: %s \n", buf);
    cog_destroy();
    assert(0);
}

//platform
void cog_platform_init(void)
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    {
        cog_errorf(SDL_GetError());
    }
}

//window
void cog_window_init(void)
{
    //TODO:Get from yaml conf.
    int width = 800;
    int height = 600;
    int bpp = 32;
    int flags = SDL_OPENGL | SDL_DOUBLEBUF;
    if( (window.screen = SDL_SetVideoMode( width, height, bpp, flags )) == 0 )
    {
        cog_errorf("cog_window_init failed when creating SDL window <%s> \n", SDL_GetError());
    }

    SDL_WM_SetCaption( "cog game", NULL );
}

void cog_window_togglefullscreen(void)
{
    SDL_WM_ToggleFullScreen(window.screen);
}

//keys
void cog_checkkeys(void)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                cog_destroy();
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        cog_destroy();
                        break;
                    case SDLK_f:
                        cog_window_togglefullscreen();
                        break;
                }
        }
    }
}

//graphics
void cog_graphics_init(void)
{
    glewInit();
    if(!GLEW_VERSION_2_1)
    {
        //TODO: Revert to software rendering if not available.
        perror("Error: OpenGL 2.1 not available, bailing out.");
    }
    /*
    glClearColor(0.3f,0.3f,0.5f,0.0f);
    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glViewport(0,0,COG_SCREEN_WIDTH,COG_SCREEN_HEIGHT);
    glShadeModel(GL_SMOOTH);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluPerspective(40, 1, 0.0001, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //
    */

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //    glBlendFunc(GL_DST_COLOR,GL_ZERO);
    //glClearColor( 0, 0, 0, 0 );
    //glClearColor( 0, 0, 0, 0 );
    glClearColor(0.3f,0.3f,0.5f,0.0f);
    //glClearColor( 1, 1, 1, 1 );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, COG_SCREEN_WIDTH, COG_SCREEN_HEIGHT, 0, -1, 1 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if(! cog_graphics_init_shaders())
    {
        perror("Error creating shaders");
    }
    /*
    glGenBuffers(1, &(renderer.vertbuffid));
    glBindBuffer(GL_ARRAY_BUFFER, renderer.vertbuffid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spritevertices), spritevertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    // Index array of vertex / color data
    glGenBuffers(1, &(renderer.vertorderbuffid));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.vertorderbuffid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(spriteverticesorder), spriteverticesorder, GL_STATIC_DRAW);
    */
}

int cog_graphics_init_shaders(void)
{
    renderer.vertid = cog_graphics_load_shader("../src/simple2d.vert", GL_VERTEX_SHADER);
    renderer.fragid = cog_graphics_load_shader("../src/simple2d.frag", GL_FRAGMENT_SHADER);
    if((renderer.vertid == 0) || (renderer.fragid == 0))
    {
        return 0;
    }
    int programid = glCreateProgram();
    glAttachShader(programid, renderer.vertid);
    glAttachShader(programid, renderer.fragid);
    //glBindAttribLocation(programid, 0, "position");
    glLinkProgram(programid);
    int linkedstatus;
    glGetProgramiv(programid, GL_LINK_STATUS, &linkedstatus);
    if(! linkedstatus) {
        int maxlength;
        glGetShaderiv(programid, GL_INFO_LOG_LENGTH, &maxlength);
        char* errorbuf = (char*)malloc(sizeof(char)*maxlength);
        glGetShaderInfoLog(programid, maxlength, &maxlength, errorbuf);
        perror(errorbuf);
        return 0;
    } else {
        glUseProgram(programid);
        return 1;
    }
}

GLuint cog_graphics_load_shader(char* filename, GLenum shadertype)
{
    //char filebuf[COG_MAX_FILE_BUF];
    char filebuf[COG_MAX_FILE_BUF];
    memset(filebuf,0,COG_MAX_FILE_BUF);
    cog_read_file(filebuf, filename);
    printf("File is \n---\n%s---\n", filebuf);
    GLuint id = glCreateShader(shadertype);
    //glShaderSource(id, 1, (const GLchar **)&filebuf, 0);
    char* buf = &(filebuf[0]);
    glShaderSource(id, 1, (const GLchar **)&buf, 0);
    glCompileShader(id);
    int compilestatus;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compilestatus);
    if(! compilestatus)
    {
        int maxlength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxlength);
        char* errorbuf = (char*)malloc(sizeof(char)*maxlength);
        glGetShaderInfoLog(id, maxlength, &maxlength, errorbuf);
        perror(errorbuf);
        return 0;
    }
    else
    {
        return id;
    }
}

void cog_read_file(char* buf, char* filename)
{
    FILE *fp;
    fp=fopen(filename, "r");
    if(fp==0)
    {
        fprintf(stderr, "Can't open file %s", filename);
        perror("Exiting");
    }
    fseek (fp , 0 , SEEK_END);
    int filesize = ftell(fp);
    rewind(fp);
    if(filesize>COG_MAX_BUF)
    {
        perror("File too big to be read in");
        goto cleanup;
    }
    int amountread = fread(buf,
            sizeof(char),
            COG_MAX_BUF,
            fp);
    if(amountread!=filesize)
    {
        perror("Error reading in file.");
        goto cleanup;
    }
cleanup:
    fclose(fp);
}

void cog_render()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();
    /*
    //glTranslatef(0.0, 0.0, 150.0);
    //glRotatef(-45.0, 1.0, 0.0, 0.0);

    glBindBuffer(GL_ARRAY_BUFFER, renderer.vertbuffid);
    glVertexAttribPointer(
            0, // attribute
            4, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            (void*)0 // array buffer offset
            );
//    glEnableVertexAttribArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, renderer.vertorderbuffid);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.vertorderbuffid);
    glDrawElements(
            GL_QUADS, // Mode
            4, // Count
            GL_UNSIGNED_SHORT, // Data type
            (void*)0 // Offset
            );
            */
    /*
    GLfloat vVertices[] = {0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f};
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    */
    //glTranslatef(0.0, 0.0, -250.0);
    //glTranslatef(10.0, 10.0, 0.0);
    //glRotatef(-45.0, 1.0, 0.0, 0.0);
    //float scale = 0.01;
    float scale = 100.0;
    glBegin(GL_QUADS);                      // Draw A Quad
        glVertex2f(-1.0f*scale, 1.0f*scale);              // Top Left
        glVertex2f( 1.0f*scale, 1.0f*scale);              // Top Right
        glVertex2f( 1.0f*scale,-1.0f*scale);              // Bottom Right
        glVertex2f(-1.0f*scale,-1.0f*scale);              // Bottom Left
    glEnd();

    SDL_GL_SwapBuffers();
}
