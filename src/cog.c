#include "cog.h"

#include <stdarg.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glew.h>

#include "cog_core.h"
#include "cog_map.h"
#include "cog_list.h"

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
    GLuint programid;
} cog_renderer;
static cog_renderer renderer;
/**
 * All sprites have a texture id and texture coords.
 **/
typedef struct
{
     cog_sprite_id id;
     GLuint texid;
     //These coords and dimensions are for the whole sprite in the game world.
     cog_float x;
     cog_float y;
     cog_float w;
     cog_float h;
     //These are the coords and dimensions of the sprite within the image. (Can have multiple sprites per image - anims are implemented using this, see below)
     cog_float texx;
     cog_float texy;
     cog_float texw;
     cog_float texh;
} cog_sprite;
/**
 * Anims are a collection of sprites with a specific duration between them.
 **/
typedef struct
{
     cog_anim_id id;
     cog_uint transition_millis;
     cog_uint currentframe;
     cog_list* frames;
} cog_anim;

typedef struct
{
    SDL_Surface* screen;
} cog_window;
static cog_window window;

//#prototypes
//##logging
void cog_errorf(const char* logMsg, ...);
//##main
void cog_platform_init(void);
void cog_window_init(void);
void cog_window_togglefullscreen(void);
void cog_checkkeys(void);
void cog_graphics_init(void);
//##shaders
int cog_graphics_init_shaders(void);
GLuint cog_graphics_load_shader(char* filename, GLenum shadertype);
void cog_graphics_print_shader_error();
//##file io
void cog_read_file(char* buf, char* filename);
//##rendering
void cog_render();
GLuint cog_upload_texture(SDL_Surface* image);
SDL_Surface* cog_load_image(const char* filename);
GLuint cog_texture_load(char* filename);

//global vars
static cog_sprite_id cog_spritecnt;
cog_map* sprites;
static cog_anim_id cog_animcnt;
cog_map* anims;

//implementations
void cog_init()
{
    game.finished = 0;
    cog_platform_init();
    cog_window_init();
    cog_graphics_init();
    //Init globals
    cog_map_init(sprites);
    cog_map_init(anims);
}

//This is the cog default loop, can be overrided by just using cog_loopstep instead.
void cog_mainloop()
{
    while(!game.finished)
    {
        cog_loopstep();
    }
}

//This is to allow the user to control the mainloop
void cog_loopstep()
{
    cog_checkkeys();
    cog_render();
}

void cog_quit()
{
    game.finished = 1;
}

cog_bool cog_hasquit()
{
    return game.finished;
}

//local
void cog_errorf(const char* logMsg, ...)
{
    va_list ap;
    va_start(ap, logMsg);
    char buf[COG_MAX_BUF];
    vsprintf(buf, logMsg, ap);
    printf("CRITICAL: %s \n", buf);
    cog_quit();
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
                cog_quit();
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        cog_quit();
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
    glEnable(GL_TEXTURE_2D);
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
    //texture
    int textureuniform = glGetUniformLocation(renderer.programid, "my_color_texture");
    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, cog_texture_load("../media/test0.png"));
    glBindTexture(GL_TEXTURE_2D, cog_texture_load("../media/kitten_anim.png"));
    glUniform1i(textureuniform, 0);
}

int cog_graphics_init_shaders(void)
{
    renderer.vertid = cog_graphics_load_shader("../src/simple2d.vert", GL_VERTEX_SHADER);
    renderer.fragid = cog_graphics_load_shader("../src/simple2d.frag", GL_FRAGMENT_SHADER);
    if((renderer.vertid == 0) || (renderer.fragid == 0))
    {
        return 0;
    }
    renderer.programid = glCreateProgram();
    glAttachShader(renderer.programid, renderer.vertid);
    glAttachShader(renderer.programid, renderer.fragid);
    //glBindAttribLocation(programid, 0, "position");
    glLinkProgram(renderer.programid);
    int linkedstatus;
    glGetProgramiv(renderer.programid, GL_LINK_STATUS, &linkedstatus);
    if(! linkedstatus)
    {
        cog_graphics_print_shader_error(renderer.programid);
        return 0;
    }
    else
    {
        glUseProgram(renderer.programid);
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
        cog_graphics_print_shader_error(id);
        return 0;
    }
    else
    {
        return id;
    }
}

void cog_graphics_print_shader_error(int shaderid)
{
    int maxlength;
    glGetShaderiv(shaderid, GL_INFO_LOG_LENGTH, &maxlength);
    char* errorbuf = (char*)malloc(sizeof(char)*maxlength);
    glGetShaderInfoLog(shaderid, maxlength, &maxlength, errorbuf);
    perror(errorbuf);
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
    if(filesize>COG_MAX_FILE_BUF)
    {
        perror("File too big to be read in");
        goto cleanup;
    }
    int amountread = fread(buf,
            sizeof(char),
            COG_MAX_FILE_BUF,
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
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTranslatef(100.0,100.0, 0.0);

    /*
    float scale = 100.0;
    glBegin(GL_QUADS);                      // Draw A Quad
        glTexCoord2f( 0.0f, 1.0f );
        glVertex2f(-1.0f*scale, 1.0f*scale);              // Top Left
        glTexCoord2f( 1.0f, 1.0f );
        glVertex2f( 1.0f*scale, 1.0f*scale);              // Top Right
        glTexCoord2f( 1.0f, 0.0f );
        glVertex2f( 1.0f*scale,-1.0f*scale);              // Bottom Right
        glTexCoord2f( 0.0f, 0.0f );
        glVertex2f(-1.0f*scale,-1.0f*scale);
    glEnd();
    */
    float scale = 100.0;
    float xframes = 3.0;
    float yframes = 1.0;
    //NOTE: Indexed from 1, should be from 0? No, don't think so. Should add check for 0 though.
    int framex=3;
    int framey=1;
    float frame_idx_x_start = (float)(framex-1.0f);
    float frame_idx_y_start = (float)(framey-1.0f);
    float frame_idx_x_end = (float)framex;
    float frame_idx_y_end = (float)framey;
    //Going from bottom left coord to top right.
//    float tex_startx = (framex-1.0f)/xframes;
//    float tex_starty = (framey-1.0f)/yframes;
//    float tex_endx = (framex)/xframes;
//    float tex_endy = (framey)/yframes;

    float tex_startx = (framex-1.0f)/xframes;
    float tex_starty = (framey)/yframes;
    float tex_endx = (framex)/xframes;
    float tex_endy = (framey-1.0f)/yframes;
    glBegin(GL_QUADS);                      // Draw A Quad
        glTexCoord2f(tex_startx, tex_starty);
        glVertex2f(-1.0f*scale, 1.0f*scale);              // Top Left
        //glTexCoord2f( frame_idx_x/xframes, frame_idx_y/yframes );
        glTexCoord2f(tex_endx, tex_starty);
        glVertex2f(1.0f*scale, 1.0f*scale);              // Top Right
        glTexCoord2f(tex_endx, tex_endy);
        glVertex2f(1.0f*scale,-1.0f*scale);              // Bottom Right
        glTexCoord2f(tex_startx, tex_endy);
        glVertex2f(-1.0f*scale,-1.0f*scale);
    glEnd();

    SDL_GL_SwapBuffers();
}

SDL_Surface* cog_load_image(const char* filename)
{
    //Loads an image and returns an SDL_Surface.
    SDL_Surface* tempsurface;
    SDL_Surface* result;

    tempsurface = IMG_Load(filename);
    if(!tempsurface)
    {
        fprintf(stderr, "Cannot load image file <%s> : <%s>", filename, SDL_GetError());
        return 0;
    }

    if((result = SDL_DisplayFormatAlpha(tempsurface))==NULL)
    {
        perror(SDL_GetError());
    }
    SDL_FreeSurface(tempsurface);

    return result;
}

GLuint cog_upload_texture(SDL_Surface* image)
{
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
    /* Create the target alpha surface with correct color component ordering */
    SDL_Surface* alphaimage = SDL_CreateRGBSurface(SDL_SWSURFACE,
            image->w,image->h,32,
            rmask,gmask,bmask,amask);
    if(!alphaimage)
    {
        fprintf(stderr, "cog_upload_texture : RGB surface creation failed.");
        cog_graphics_print_shader_error(renderer.programid);
    }
    // Set up so that colorkey pixels become transparent :
    Uint32 colorkey = SDL_MapRGBA( alphaimage->format, rmask, gmask, bmask, amask );
    SDL_FillRect( alphaimage, 0, colorkey );
    colorkey = SDL_MapRGBA( image->format, rmask, gmask, bmask, amask);
    SDL_SetColorKey( image, SDL_SRCCOLORKEY, colorkey );
    SDL_Rect area;
    SDL_SetAlpha(image, 0, amask); //http://www.gamedev.net/topic/518525-opengl--sdl--transparent-image-make-textures/
    // Copy the surface into the GL texture image :
    area.x = 0;
    area.y = 0;
    area.w = image->w;
    area.h = image->h;
    SDL_BlitSurface( image, &area, alphaimage, &area );
    // Create an OpenGL texture for the image
    GLuint textureID;
    glGenTextures( 1, &textureID );
    glBindTexture( GL_TEXTURE_2D, textureID );
    /* Prepare the filtering of the texture image */
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    /* Map the alpha surface to the texture */
    glTexImage2D( GL_TEXTURE_2D,
            0,
            GL_RGBA,
            w,
            h,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            alphaimage->pixels );
    SDL_FreeSurface(alphaimage);
    return textureID ;
}

GLuint cog_texture_load(char* filename)
{
    SDL_Surface* image = cog_load_image(filename);
    GLuint texture = cog_upload_texture(image);
    SDL_FreeSurface(image);
    return texture;
}

//Anim
/**
 * Allocates a sprite and puts in global map for access.
 * Returns: An id to access sprite using global map.
 * */
cog_sprite_id cog_sprite_load(char* filename,
        cog_float x,
        cog_float y,
        cog_float w,
        cog_float h,
        cog_float texx,
        cog_float texy,
        cog_float texw,
        cog_float texh)
{
    cog_sprite* sprite = COG_STRUCT_MALLOC(cog_sprite);
    sprite->id = cog_spritecnt++;
    sprite->texid = cog_texture_load(filename);
    sprite->x = x;
    sprite->y = y;
    sprite->w = w;
    sprite->h = h;
    sprite->texx = texx;
    sprite->texy = texy;
    sprite->texw = texw;
    sprite->texh = texh;
    cog_map_put(sprites, sprite->id, (void*)sprite);
    return sprite->id;
}

/**
 * Assumes animation is a single 1D animation frame.
 * */
cog_anim_id cog_add_anim(char* animimg,
        cog_uint transition_millis,
        cog_bool looped,
        cog_uint nimages,
        cog_float x,
        cog_float y,
        cog_float w,
        cog_float h, ...)
{
    cog_anim* anim = COG_STRUCT_MALLOC(cog_anim);
    anim->id = cog_animcnt++;
    anim->transition_millis = transition_millis;
    anim->currentframe = 0;

    //XXX: Find out w and h by loading SDL_Surface
    SDL_Surface* image = cog_load_image(animimg);
    int imagew = image->w;
    int imageh = image->h;
    int wanimframe = imagew / nimages;
    int hanimframe = imageh;
    //Load nimages sprites in, with offset dependant on frame number.
    cog_list_init(anim->frames);
    for(int i=0;i<nimages;i++)
    {
        cog_sprite_id sid = cog_sprite_load(animimg, 
                x,
                y,
                w,
                h,
                i*wanimframe,
                0,
                wanimframe,
                hanimframe);
        cog_list_append(anim->frames, cog_map_get(sprites, sid));
    }
    cog_map_put(anims, anim->id, (void*)anim);
    return anim->id;
}


void cog_play_anim(cog_anim_id id)
{
    //TODO:Play animation.
}

void cog_anim_update_pos(cog_anim_id id,
        cog_float x,
        cog_float y)
{
    cog_anim* anim = (cog_anim*)cog_map_get(anims, id);
    for(cog_list* frame = anim->frames;
        frame != 0;
        frame=frame->next)
    {
        cog_sprite* sprite = (cog_sprite*)frame->data;
        sprite->x = x;
        sprite->y = y;
    }
}
cog_float cog_anim_getx(cog_anim_id id)
{
    cog_anim* anim = (cog_anim*)cog_map_get(anims, id);
    return ((cog_sprite*)anim->frames->data)->x;
}
cog_float cog_anim_gety(cog_anim_id id)
{
    cog_anim* anim = (cog_anim*)cog_map_get(anims, id);
    return ((cog_sprite*)anim->frames->data)->y;
}

//sound
cog_snd_id cog_sound_load(char* fname)
{
    //TODO
}

void cog_sound_play(cog_snd_id snd)
{
    //TODO
}

int cog_sound_isfinished(cog_snd_id snd)
{
    //TODO
}
