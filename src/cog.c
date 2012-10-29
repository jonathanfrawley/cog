#include "cog.h"

#include <math.h>
#include <stdarg.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <AL/alut.h>
#include <SDL/SDL_ttf.h>
#include <GL/glew.h>

#include "cog_core.h"
#include "cog_map.h"
#include "cog_list.h"

//constants
#define FRAMES_PER_SECOND 40
static cog_uint FRAME_TIME = 1000 / FRAMES_PER_SECOND;

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
    cog_float rot;
    //These are the coords and dimensions of the sprite within the image. (Can have multiple sprites per image - anims are implemented using this, see below)
    cog_float texx;
    cog_float texy;
    cog_float texw;
    cog_float texh;
    //physics
    cog_float xvel;
    cog_float yvel;
} cog_sprite;
/**
 * Anims are a collection of sprites with a specific duration between them.
 **/
typedef struct
{
    cog_anim_id id;
    cog_uint transition_millis;
    cog_uint currentframe;
    cog_uint currentframe_millis;
    cog_bool looped;
    cog_list frames;
    cog_uint nframes;
    cog_bool paused;
} cog_anim;
typedef struct
{
    cog_snd_id id;
    ALuint buffer;
    ALuint source;
} cog_snd;

typedef struct
{
    SDL_Surface* screen;
} cog_window;
static cog_window window;

//#prototypes
//##main
void cog_platform_init(void);
void cog_window_init(void);
void cog_window_togglefullscreen(void);
void cog_graphics_init(void);
void cog_audio_init(void);
void cog_graphics_hwinit(void);
void cog_graphics_swinit(void);
void cog_update_anims(cog_uint deltamillis);
//##shaders
int cog_graphics_init_shaders(void);
GLuint cog_graphics_load_shader(char* filename, GLenum shadertype);
void cog_graphics_print_shader_error();
//##file io
void cog_read_file(char* buf, char* filename);
//##rendering
void cog_graphics_render();
void cog_graphics_hwrender();
void cog_graphics_swrender();
GLuint cog_upload_texture(SDL_Surface* image);
SDL_Surface* cog_load_image(const char* filename);
GLuint cog_texture_load(char* filename);
cog_float cog_math_radtodeg(cog_float rad);
//##input
//###mouse
void cog_input_checkmouse(void);
//##keys
void cog_input_checkkeys(void);
//##text
void cog_text_init();
void cog_text_openfont();

//
//#global vars
static cog_sprite_id cog_spritecnt;
static cog_map sprites;
static cog_anim_id cog_animcnt;
static cog_map anims;
static cog_int configmask;
static cog_list activesprites; //sprites drawn(active) at the moment
static cog_list activeanims; //anims drawn(active) at the moment
static cog_list activesnds; //snds playing at the moment
static cog_snd_id cog_sndcnt;
static cog_map snds;
//##timing
static cog_uint now;
static cog_uint timedelta;
static cog_uint starttime;
static cog_uint lastframetime;
static cog_uint frametimecounter;
static cog_uint framedrawcounter;
static cog_uint frameupdatecounter;
//##input
static cog_bool mouseleftpressed;
static cog_bool mouserightpressed;
static cog_bool mouseleftjustpressed = 0;
static cog_bool mouserightjustpressed;
static cog_float mousex;
static cog_float mousey;
//##text
static const char* fontpath = "../media/font/04B_03__.ttf"; //TODO:Add option to set this
static TTF_Font* font;
static cog_uint fontptsize = 8;

//implementations
void cog_init(cog_int config)
{
    configmask = config;
    //Init globals
    cog_map_init(&sprites);
    cog_map_init(&anims);
    cog_map_init(&snds);
    cog_list_init(&activesprites, sizeof(cog_sprite_id));
    cog_list_init(&activeanims, sizeof(cog_sprite_id));
    cog_list_init(&activesnds, sizeof(cog_sprite_id));
    //Init cog
    game.finished = 0;
    cog_platform_init();
    cog_window_init();
    cog_graphics_init();
    cog_audio_init();
    cog_text_init();
    starttime = SDL_GetTicks();

    //init rng
    srand(2);
}

//This is the cog default loop, can be overrided by just using cog_loopstep instead.
void cog_mainloop()
{
    while(!game.finished)
    {
        cog_loopstep();
    }
}

/* *
 * This will be return true when the user should call another
 * cog_update()
 * */
cog_bool cog_updateready()
{
    now = SDL_GetTicks();
    timedelta = now - starttime;
    return (timedelta > FRAME_TIME);
}

void cog_sleep(cog_uint millis)
{
    SDL_Delay(millis);
}

void cog_sleepuntilupdate()
{
    now = SDL_GetTicks();
    timedelta = now - starttime;
    if(timedelta<FRAME_TIME)
    {
        cog_sleep(FRAME_TIME-timedelta);
    }
}

void cog_update()
{
    now = SDL_GetTicks();
    timedelta = now - starttime;

    cog_input_checkkeys();
    cog_input_checkmouse();
    cog_update_anims(timedelta);
    cog_update_physics(timedelta);

    //performance timing
    frameupdatecounter++;
    lastframetime = SDL_GetTicks() - starttime;
    starttime = SDL_GetTicks();
#ifdef DEBUG
    //Useful logging every second.
    frametimecounter += lastframetime;
    if(frametimecounter >= 1000)
    {
        cog_debugf("nupdates <%d>, ndraws <%d>", frameupdatecounter, framedrawcounter);
        frametimecounter = 0;
        framedrawcounter = 0;
        frameupdatecounter = 0;
    }
#endif //DEBUG
}

//This is to allow the user to control the mainloop
void cog_loopstep()
{
    cog_update();
    cog_graphics_render();
}

void cog_quit()
{
    game.finished = 1;
    //TODO:Add more cleanup here.
    alutExit();
}

cog_bool cog_hasquit()
{
    return game.finished;
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
    int width = COG_SCREEN_WIDTH;
    int height = COG_SCREEN_HEIGHT;
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

void cog_update_anims(cog_uint deltamillis)
{
    COG_LIST_FOREACH(&activeanims)
    {
        //Draw current sprite
        cog_anim_id id = *((cog_anim_id*)curr->data);
        cog_anim* thisanim = (cog_anim*)cog_map_get(&anims,id);

        if(thisanim->paused)
        {
            continue;
        }
        thisanim->currentframe_millis += deltamillis;
        if(thisanim->currentframe_millis >= thisanim->transition_millis)
        {
            thisanim->currentframe++;
            thisanim->currentframe_millis = thisanim->currentframe_millis - thisanim->transition_millis; //Diff
            if(thisanim->currentframe >= thisanim->nframes)
            {
                if(thisanim->looped)
                {
                    thisanim->currentframe = 0;
                }
                else
                {
                    thisanim->paused = 0;
                    cog_anim_remove(id);
                }
            }
        }
    }
}

//graphics
void cog_graphics_init(void)
{
    if(configmask & COG_CONFIG_HWRENDER)
    {
        cog_debugf("Initializing hardware rendering...");
        cog_graphics_hwinit();
    }
    else
    {
        cog_debugf("Initializing software rendering...");
        cog_graphics_swinit();
    }

}

void cog_audio_init(void)
{
    /*
    int audio_rate = 22050;
    Uint16 audio_format = AUDIO_S16SYS;
    int audio_channels = 2;
    int audio_buffers = 4096;

    if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
    {
        cog_errorf("Unable to initialize audio: %s\n", Mix_GetError());
        exit(1);
    }
    */
    int argc = 1;
    char* argv = "cog";
    alutInit(&argc, &argv);
}

void cog_graphics_hwinit(void)
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
    /*
    //XXX:TEX STUFF HERE
    //texture
    int textureuniform = glGetUniformLocation(renderer.programid, "my_color_texture");
    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, cog_texture_load("../media/test0.png"));
    glBindTexture(GL_TEXTURE_2D, cog_texture_load("../media/kitten_anim.png"));
    glUniform1i(textureuniform, 0);
    */
}

void cog_graphics_swinit(void)
{
    //TODO
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

void cog_graphics_render()
{
    framedrawcounter++;
    if(configmask & COG_CONFIG_HWRENDER)
    {
        cog_graphics_hwrender();
    }
    else
    {
        cog_graphics_swrender();
    }
}

cog_uint cog_gettimedelta()
{
    return timedelta;
}

/**
 * Sprites are drawn centred at the sprite's x and y coord, as opposed to most
 * engines where they are drawn from the top left.
 * */
void cog_graphics_draw_sprite(cog_sprite* sprite)
{
    glPushMatrix();
    //Bind texture
    int textureuniform = glGetUniformLocation(renderer.programid, "my_color_texture");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite->texid);
    //glUniform1i(textureuniform, sprite->texid);

    //Translate
    //glTranslatef(sprite->x - (sprite->w*0.5),sprite->y - (sprite->h*0.5), 0.0);
    //glTranslatef(sprite->x + (sprite->w),sprite->y + (sprite->h), 0.0);
    glTranslatef(sprite->x,sprite->y, 0.0);
    glRotatef( -cog_math_radtodeg(sprite->rot), 0.0f, 0.0f, 1.0f );

    glBegin(GL_QUADS);                      // Draw A Quad
        glTexCoord2f(sprite->texx,
                sprite->texy + sprite->texh);
        glVertex2f(-1.0f*sprite->w,
                1.0f*sprite->h);              // Top Left
        //glTexCoord2f( frame_idx_x/xframes, frame_idx_y/yframes );
        glTexCoord2f(sprite->texx + sprite->texw,
                sprite->texy + sprite->texh);
        glVertex2f(1.0f*sprite->w,
                1.0f*sprite->h);              // Top Right
        glTexCoord2f(sprite->texx + sprite->texw,
                sprite->texy);
        glVertex2f(1.0f*sprite->w,
                -1.0f*sprite->h);             // Bottom Right
        glTexCoord2f(sprite->texx,
                sprite->texy);
        glVertex2f(-1.0f*sprite->w,
                -1.0f*sprite->h);             // Bottom left
    glEnd();
/*
    float verts[] = {
        -1.0f*sprite->w,
        1.0f*sprite->h,              // Top Left
        1.0f*sprite->w,
        1.0f*sprite->h,              // Top Right
        1.0f*sprite->w,
        -1.0f*sprite->h,             // Bottom Right
        -1.0f*sprite->w,
        -1.0f*sprite->h             // Bottom left
    };

    const float texverts[] = {
        tx, ty,
        tx + tw, ty,
        tx + tw, ty + th,
        tx, ty + th
    };

    // ... Bind the texture, enable the proper arrays

    glVertexPointer(2, GL_FLOAT, verts);
    glTextureVertexPointer(2, GL_FLOAT, texVerts);
    glDrawArrays(GL_TRI_STRIP, 0, 4);
    */

    glPopMatrix();
}

void cog_graphics_hwrender()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();

    //Draw sprites
    COG_LIST_FOREACH(&activesprites)
    {
        //draw current sprite
        cog_sprite* thissprite = (cog_sprite*)cog_map_get(&sprites,*((cog_sprite_id*)curr->data));
        cog_graphics_draw_sprite(thissprite);
    }
    //Draw anims
    COG_LIST_FOREACH(&activeanims)
    {
        //draw current sprite
        cog_anim* thisanim = (cog_anim*)cog_map_get(&anims,*((cog_anim_id*)curr->data));
        if(thisanim->paused)
        {
            continue;
        }
        //find active frame to render
        cog_list* frame = (thisanim->frames.next);
        cog_uint i=0;
        while(i<thisanim->currentframe)
        {
            frame = frame->next;
            i++;
        }
        //cog_graphics_draw_sprite((cog_sprite*)cog_map_get(&sprites,*((cog_uint*)frame->data)));
        cog_graphics_draw_sprite((cog_sprite*)frame->data);
    }

    SDL_GL_SwapBuffers();
}

void cog_graphics_swrender()
{

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

    glPixelStorei(GL_UNPACK_ALIGNMENT,4);

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
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    return textureID;
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
        cog_float rot,
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
    sprite->rot = rot;
    sprite->texx = texx;
    sprite->texy = texy;
    sprite->texw = texw;
    sprite->texh = texh;
    //physics always 0
    sprite->xvel = 0.0f;
    sprite->yvel = 0.0f;
    cog_map_put(&sprites, sprite->id, (void*)sprite);
    return sprite->id;
}

/**
 * Loads sprite and readies for drawing
 */
cog_sprite_id cog_sprite_add(char* filename,
        cog_float x,
        cog_float y,
        cog_float w,
        cog_float h,
        cog_float rot,
        cog_float texx,
        cog_float texy,
        cog_float texw,
        cog_float texh)
{
    cog_sprite_id id = cog_sprite_load(
        filename,
        x,
        y,
        w,
        h,
        rot,
        texx,
        texy,
        texw,
        texh);
    //Need to malloc a copy of the id to ensure it is still around
    //when the list entry is recalled.
    cog_sprite_id* idcopy = COG_STRUCT_MALLOC(cog_sprite_id);
    (*idcopy) = id;
    cog_list_append(&activesprites, idcopy);
    return id;
}

cog_sprite_id cog_sprite_simple_add(char* filename,
        cog_float x,
        cog_float y,
        cog_float w,
        cog_float h,
        cog_float rot)
{
    return cog_sprite_add(filename,
            x,
            y,
            w,
            h,
            rot,
            0,
            0,
            1.0,
            1.0);
}

void cog_sprite_update_pos(cog_sprite_id id,
        cog_float x,
        cog_float y)
{
    cog_sprite* sprite = (cog_sprite*)cog_map_get(&sprites, id);
    sprite->x = x;
    sprite->y = y;
}

cog_float cog_sprite_getx(cog_sprite_id id)
{
    cog_sprite* sprite = (cog_sprite*)cog_map_get(&sprites, id);
    return sprite->x;
}

cog_float cog_sprite_gety(cog_sprite_id id)
{
    cog_sprite* sprite = (cog_sprite*)cog_map_get(&sprites, id);
    return sprite->y;
}

cog_float cog_sprite_getw(cog_sprite_id id)
{
    cog_sprite* sprite = (cog_sprite*)cog_map_get(&sprites, id);
    return sprite->w;
}

cog_float cog_sprite_geth(cog_sprite_id id)
{
    cog_sprite* sprite = (cog_sprite*)cog_map_get(&sprites, id);
    return sprite->w;
}

cog_float cog_sprite_getrot(cog_sprite_id id)
{
    cog_sprite* sprite = (cog_sprite*)cog_map_get(&sprites, id);
    return sprite->rot;
}

cog_float cog_sprite_update_rot(cog_sprite_id id, cog_float rot)
{
    cog_sprite* sprite = (cog_sprite*)cog_map_get(&sprites, id);
    sprite->rot = rot;
}

cog_float cog_sprite_update_xvel(cog_sprite_id id, cog_float xvel)
{
    cog_sprite* sprite = (cog_sprite*)cog_map_get(&sprites, id);
    sprite->xvel = xvel;
}

cog_float cog_sprite_update_yvel(cog_sprite_id id, cog_float yvel)
{
    cog_sprite* sprite = (cog_sprite*)cog_map_get(&sprites, id);
    sprite->yvel = yvel;
}

void cog_sprite_remove(cog_sprite_id id)
{
    COG_LIST_FOREACH(&activesprites)
    {
        if(*((cog_sprite_id*)curr->data) == id)
        {
            cog_list_remove(&activesprites, curr->data);
            break;
        }
    }
    cog_map_remove(&sprites, id);
}

void cog_sprite_removeall(void)
{
    cog_list_removeall(&activesprites);
}

/**
 * Assumes animation is a single 1D animation frame.
 * */
cog_anim_id cog_anim_add(
        char* animimg,
        cog_uint transition_millis,
        cog_bool looped,
        cog_uint nimages,
        cog_float x,
        cog_float y,
        cog_float w,
        cog_float h,
        cog_float rot, ...)
{
    cog_anim* anim = COG_STRUCT_MALLOC(cog_anim);
    anim->id = cog_animcnt++;
    cog_list_init(&anim->frames, sizeof(cog_sprite));
    anim->transition_millis = transition_millis;
    anim->looped = looped;
    anim->currentframe = 0;
    anim->currentframe_millis = 0;
    anim->paused = COG_FALSE;
    anim->nframes = 0;

    cog_float wanimframe = ((cog_float)1.0 / nimages);
    cog_float hanimframe = 1.0;
    //Load nimages sprites in, with offset dependant on frame number.
    for(int i=0;i<nimages;i++)
    {
        cog_sprite_id sid = cog_sprite_load(animimg,
                x,
                y,
                w,
                h,
                rot,
                i*wanimframe,
                0,
                wanimframe,
                hanimframe);
        cog_list_append(&(anim->frames), cog_map_get(&sprites, sid));
        anim->nframes++;
    }
    cog_map_put(&anims, anim->id, (void*)anim);

    //Need to malloc a copy of the id to ensure it is still around
    //when the list entry is recalled.
    cog_anim_id* idcopy = COG_STRUCT_MALLOC(cog_anim_id);
    (*idcopy) = anim->id;
    cog_list_append(&activeanims, idcopy);

    return anim->id;
}

void cog_anim_play(cog_anim_id id)
{
    //TODO:Play animation.
    cog_anim* anim = (cog_anim*)cog_map_get(&anims, id);
    if(anim == COG_NULL)
    {
        cog_errorf("anim is null in cog_anim_play");
    }
    anim->paused = COG_FALSE;
}

void cog_anim_update_pos(cog_anim_id id,
        cog_float x,
        cog_float y)
{
    cog_anim* anim = (cog_anim*)cog_map_get(&anims, id);
    if(anim == COG_NULL)
    {
        cog_errorf("anim is null in cog_anim_update_pos");
    }
    //Update the positions of all sprites in anim.
    COG_LIST_FOREACH(&anim->frames)
    {
        cog_sprite* sprite = (cog_sprite*)(curr->data);

        sprite->x = x;
        sprite->y = y;
    }
}

cog_float cog_anim_getx(cog_anim_id id)
{
    cog_anim* anim = (cog_anim*)cog_map_get(&anims, id);
    return ((cog_sprite*)anim->frames.next->data)->x;
}

cog_float cog_anim_gety(cog_anim_id id)
{
    cog_anim* anim = (cog_anim*)cog_map_get(&anims, id);
    return ((cog_sprite*)anim->frames.next->data)->y;
}

cog_float cog_anim_getw(cog_anim_id id)
{
    cog_anim* anim = (cog_anim*)cog_map_get(&anims, id);
    return ((cog_sprite*)anim->frames.next->data)->w;
}

cog_float cog_anim_geth(cog_anim_id id)
{
    cog_anim* anim = (cog_anim*)cog_map_get(&anims, id);
    return ((cog_sprite*)anim->frames.next->data)->h;
}

cog_float cog_anim_getrot(cog_anim_id id)
{
    cog_anim* anim = (cog_anim*)cog_map_get(&anims, id);
    return ((cog_sprite*)anim->frames.next->data)->rot;
}

cog_float cog_anim_update_rot(cog_anim_id id, cog_float rot)
{
    cog_anim* anim = (cog_anim*)cog_map_get(&anims, id);
    if(anim == COG_NULL)
    {
        cog_errorf("anim is null in cog_anim_update_rot");
    }
    //Update the positions of all sprites in anim.
    COG_LIST_FOREACH(&anim->frames)
    {
        cog_sprite* sprite = (cog_sprite*)(curr->data);
        sprite->rot = rot;
    }
}

cog_bool cog_anim_isfinished(cog_anim_id id)
{
    COG_LIST_FOREACH(&activeanims)
    {
        if(*((cog_anim_id*)curr->data) == id)
        {
            return COG_FALSE;
        }
    }
    return COG_TRUE;
}

void cog_anim_remove(cog_anim_id id)
{
    COG_LIST_FOREACH(&activeanims)
    {
        if(*((cog_anim_id*)curr->data) == id)
        {
            cog_list_remove(&activeanims, curr->data);
            break;
        }
    }
    cog_map_remove(&anims, id);
}

void cog_anim_removeall(void)
{
    cog_list_removeall(&activeanims);
}

//sound
cog_snd_id cog_snd_load(char* fname)
{
    cog_snd* snd = COG_STRUCT_MALLOC(cog_snd);
    snd->id = cog_sndcnt++;
    snd->buffer = alutCreateBufferFromFile(fname);
    alGenSources(1, &snd->source);
    alSourcei(snd->source, AL_BUFFER, snd->buffer);
    cog_map_put(&snds, snd->id, (void*)snd);
    return snd->id;
}

//TODO:Remove channel arg.
void cog_snd_play(cog_snd_id id)
{
    cog_snd* snd = (cog_snd*)cog_map_get(&snds, id);
    alSourcePlay(snd->source);
    cog_list_append(&activesnds, (cog_dataptr)snd);
}

void cog_snd_play_sfx(cog_snd_id id)
{
    cog_snd_play(id);
}

void cog_snd_play_music(cog_snd_id id)
{
    cog_snd* snd = (cog_snd*)cog_map_get(&snds, id);
    alSourcei(snd->source,AL_LOOPING,AL_TRUE);
    cog_snd_play(id);
}

cog_snd_id cog_snd_stop(cog_snd_id id)
{
    cog_snd* snd = (cog_snd*)cog_map_get(&snds, id);
    alSourceStop(snd->source);
    cog_list_remove(&activesnds, (cog_dataptr)snd);
}

cog_snd_id cog_snd_stop_all()
{
    COG_LIST_FOREACH(&activesnds)
    {
        cog_snd* snd = (cog_snd*)curr->data;
        cog_snd_stop(snd->id);
    }
}

cog_float cog_math_radtodeg(cog_float rad)
{
    return (rad * (180.0f/COG_PI));
}

cog_float cog_math_sqrt(cog_float x)
{
    return sqrt(x);
}

cog_uint cog_get_screenw()
{
    return COG_SCREEN_WIDTH;
}

cog_uint cog_get_screenh()
{
    return COG_SCREEN_HEIGHT;
}

cog_uint cog_nextrand()
{
    return rand();
}

cog_float cog_sprite_dist(cog_sprite_id a, cog_sprite_id b)
{
    cog_sprite* asprite = (cog_sprite*)cog_map_get(&sprites, a);
    cog_sprite* bsprite = (cog_sprite*)cog_map_get(&sprites, b);
    //TODO
    return cog_math_sqrt((asprite->x - bsprite->x) * (asprite->x - bsprite->x) +
        (asprite->y - bsprite->y) * (asprite->y - bsprite->y));
}

cog_float cog_sprite_anim_dist(cog_sprite_id a, cog_anim_id b)
{
    cog_sprite* asprite = (cog_sprite*)cog_map_get(&sprites, a);
    cog_anim* banim = (cog_anim*)cog_map_get(&anims, b);
    cog_sprite* bsprite = (cog_sprite*)banim->frames.next->data;
    //TODO
    return cog_math_sqrt((asprite->x - bsprite->x) * (asprite->x - bsprite->x) +
        (asprite->y - bsprite->y) * (asprite->y - bsprite->y));
}

void cog_update_physics(cog_float timedelta)
{
    COG_LIST_FOREACH(&activesprites)
    {
        cog_sprite* thissprite = (cog_sprite*)cog_map_get(&sprites,*((cog_sprite_id*)curr->data));
        //do physics update for current sprite
        thissprite->x += timedelta * thissprite->xvel;
        thissprite->y += timedelta * thissprite->yvel;
    }
    COG_LIST_FOREACH(&activeanims)
    {
        cog_anim* thisanim = (cog_anim*)cog_map_get(&anims,*((cog_anim_id*)curr->data));
        COG_LIST_FOREACH2(&thisanim->frames)
        {
            cog_sprite* thissprite = (cog_sprite*)(curr2->data);
            //do physics update for current sprite
            thissprite->x += timedelta * thissprite->xvel;
            thissprite->y += timedelta * thissprite->yvel;
        }
    }
}

//#input
//##mouse
cog_bool cog_input_mouseleftjustpressed()
{
    return mouseleftjustpressed;
}

cog_bool cog_input_mouserightjustpressed()
{
    return mouserightjustpressed;
}

cog_float cog_input_mousex()
{
    return mousex;
}

cog_float cog_input_mousey()
{
    return mousey;
}

void cog_input_checkmouse(void)
{
    cog_uint x,y;
    cog_uint state = SDL_GetMouseState(&x, &y);
    mousex = (cog_float)x;
    mousey = (cog_float)y;
    if(SDL_BUTTON_LEFT == SDL_BUTTON(state))
    {
        if(!mouseleftpressed)
        {
            mouseleftjustpressed = COG_TRUE;
        }
        else
        {
            mouseleftjustpressed = COG_FALSE;
        }
        mouseleftpressed = COG_TRUE;
    }
    else
    {
        mouseleftpressed = COG_FALSE;
    }

    if(SDL_BUTTON_LEFT == SDL_BUTTON(state))
    {
        if(!mouserightpressed)
        {
            mouserightjustpressed = COG_TRUE;
        }
        else
        {
            mouserightjustpressed = COG_FALSE;
        }
        mouserightpressed = COG_TRUE;
    }
    else
    {
        mouserightpressed = COG_FALSE;
    }
}

//##keys
void cog_input_checkkeys(void)
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

//#text
void cog_text_init()
{
    if(TTF_Init() < 0)
    {
        cog_errorf("Couldn't initialize TTF: %s\n",SDL_GetError());
    }
    cog_text_openfont();
}

void cog_text_openfont()
{
    font = TTF_OpenFont(fontpath, fontptsize);

    if(font == COG_NULL)
    {
        cog_errorf("Couldn't load %d pt font from %s: %s\n",
                fontptsize, fontpath, SDL_GetError());
    }
}

void cog_text_load_font(const char* fpath, cog_uint fsize)
{
    fontpath = fpath;
    fontptsize = fsize;
    cog_text_openfont();
}

cog_sprite_id cog_text_create_sprite(const char* text,
        cog_text_colour c,
        cog_float x,
        cog_float y,
        cog_float rot,
        cog_float texx,
        cog_float texy,
        cog_float texw,
        cog_float texh)
{
    cog_int renderstyle = TTF_STYLE_NORMAL;
    SDL_Color forecol;
    forecol.r = c.r;
    forecol.g = c.g;
    forecol.b = c.b;

    TTF_SetFontStyle(font, renderstyle);
    SDL_Surface* textsurface = TTF_RenderText_Blended(font, text, forecol);

    cog_sprite* sprite = COG_STRUCT_MALLOC(cog_sprite);
    sprite->id = cog_spritecnt++;
    sprite->texid = cog_upload_texture(textsurface);
    //sprite->w = w;
    //sprite->h = h;
    sprite->w = textsurface->w;
    sprite->h = textsurface->h;
    //XXX: This is unusual for fonts, we want them relative to top left,
    //otherwise I become very angry.
    sprite->x = x + sprite->w;
    sprite->y = y + sprite->h;
    sprite->rot = rot;
    sprite->texx = texx;
    sprite->texy = texy;
    sprite->texw = texw;
    sprite->texh = texh;
    //physics always 0
    sprite->xvel = 0.0f;
    sprite->yvel = 0.0f;
    cog_map_put(&sprites, sprite->id, (void*)sprite);

    SDL_FreeSurface(textsurface);

    //Need to malloc a copy of the id to ensure it is still around
    //when the list entry is recalled.
    cog_sprite_id* idcopy = COG_STRUCT_MALLOC(cog_sprite_id);
    (*idcopy) = sprite->id;
    cog_list_append(&activesprites, idcopy);

    return sprite->id;
}
cog_sprite_id cog_text_create(const char* text,
        cog_text_colour c,
        cog_float x,
        cog_float y)
{
    return cog_text_create_sprite(text,
            c,
            x,
            y,
            0,
            0,
            0,
            1,
            1);
}

cog_sprite_id cog_text_create_xcentred(const char* text,
        cog_text_colour c,
        cog_float x,
        cog_float y)
{
    cog_sprite_id id = cog_text_create_sprite(text,
            c,
            x,
            y,
            0,
            0,
            0,
            1,
            1);
    cog_sprite* sprite = (cog_sprite*)cog_map_get(&sprites, id);
    sprite->x -= sprite->w;
    return id;
}
