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

//data structures
typedef struct
{
    int finished;
} cog_game;
static cog_game game;

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
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClearDepth(1.0f);
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glViewport(0,0,COG_SCREEN_WIDTH,COG_SCREEN_HEIGHT);
    if(! cog_graphics_init_shaders())
    {
        perror("Error creating shaders");
    }
}

int cog_graphics_init_shaders(void)
{
    cog_graphics_load_shader("../src/simple2d.vert", GL_VERTEX_SHADER);
    cog_graphics_load_shader("../src/simple2d.frag", GL_FRAGMENT_SHADER);
}

GLuint cog_graphics_load_shader(char* filename, GLenum shadertype)
{
    char filebuf[COG_MAX_FILE_BUF];
    cog_read_file(filebuf, filename);
    printf("file is : %s\n", filebuf);
}

void cog_read_file(char* buf, char* filename)
{
    FILE *fp;
    fp=fopen(filename, "r");
    fread(buf,
           sizeof(char),
           COG_MAX_BUF,
           fp);
    fclose(fp);
}
