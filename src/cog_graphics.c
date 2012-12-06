#include "cog_graphics.h"

#include <SDL/SDL_image.h>

#include "cog.h"
#include "cog_anim.h"
#include "cog_core.h"
#include "cog_list.h"
#include "cog_math.h"
#include "cog_sprite.h"

void cog_graphics_hwinit(void);
void cog_graphics_swinit(void);
//shaders
int cog_graphics_init_shaders(void);
GLuint cog_graphics_load_shader(char* filename, GLenum shadertype);
void cog_graphics_print_shader_error();
//file io
void cog_graphics_read_file(char* buf, char* filename);
//rendering
void cog_graphics_render();
void cog_graphics_hwrender();
void cog_graphics_swrender();
SDL_Surface* cog_graphics_load_image(const char* filename);

static cog_renderer renderer;

/*-----------------------------------------------------------------------------
 * Sprites are drawn centred at the sprite's x and y coord, as opposed to most
 * engines where they are drawn from the top left.
 *-----------------------------------------------------------------------------*/
void cog_graphics_draw_sprite(cog_sprite* sprite)
{
    glPushMatrix();
    //Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite->texid);

    //Translate
    glTranslatef(sprite->x,sprite->y, 0.0);
    glRotatef( -cog_math_radtodeg(sprite->rot), 0.0f, 0.0f, 1.0f );

    glBegin(GL_QUADS);                      // Draw A Quad
        glTexCoord2f(sprite->texx,
                sprite->texy + sprite->texh);
        glVertex2f(-1.0f*sprite->w,
                1.0f*sprite->h);              // Top Left
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
    glPopMatrix();
}

void cog_graphics_draw_text(cog_text* text)
{
    glPushMatrix();
    //Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text->texid);

    //Translate
    glTranslatef(text->x+text->w,text->y+text->h, 0.0);
    glRotatef( -cog_math_radtodeg(text->rot), 0.0f, 0.0f, 1.0f );

    glBegin(GL_QUADS);                      // Draw A Quad
        glTexCoord2f(0.0f,
                1.0f);
        glVertex2f(-1.0f*text->w,
                1.0f*text->h);              // Top Left
        glTexCoord2f(1.0f,
                1.0f);
        glVertex2f(1.0f*text->w,
                1.0f*text->h);              // Top Right
        glTexCoord2f(1.0f,
                0.0f);
        glVertex2f(1.0f*text->w,
                -1.0f*text->h);             // Bottom Right
        glTexCoord2f(0.0f,
                0.0f);
        glVertex2f(-1.0f*text->w,
                -1.0f*text->h);             // Bottom left
    glEnd();
    glPopMatrix();
}

void cog_graphics_hwrender()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();

    cog_sprite_draw();
    cog_anim_draw();
    cog_text_draw();

    SDL_GL_SwapBuffers();
}

void cog_graphics_swrender()
{
    //TODO
}

SDL_Surface* cog_graphics_load_image(const char* filename)
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

GLuint cog_graphics_upload_surface(SDL_Surface* image)
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
        fprintf(stderr, "cog_graphics_upload_surface : RGB surface creation failed.");
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

GLuint cog_graphics_load_texture(char* filename)
{
    SDL_Surface* image = cog_graphics_load_image(filename);
    GLuint texture = cog_graphics_upload_surface(image);
    SDL_FreeSurface(image);
    return texture;
}

void cog_graphics_init(void)
{
    cog_debugf("Initializing hardware rendering...");
    cog_graphics_hwinit();
}

void cog_graphics_hwinit(void)
{
    glewInit();
    if(!GLEW_VERSION_2_1)
    {
        //TODO: Revert to software rendering if not available.
        perror("Error: OpenGL 2.1 not available, bailing out.");
    }

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.3f,0.3f,0.5f,0.0f);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, cog_screenw(), cog_screenh(), 0, -1, 1 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if(! cog_graphics_init_shaders())
    {
        perror("Error creating shaders");
    }
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
    char filebuf[COG_MAX_FILE_BUF];
    memset(filebuf,0,COG_MAX_FILE_BUF);
    cog_graphics_read_file(filebuf, filename);
    GLuint id = glCreateShader(shadertype);
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

void cog_graphics_read_file(char* buf, char* filename)
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
    cog_graphics_hwrender();
}
