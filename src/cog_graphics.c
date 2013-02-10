#include "cog_graphics.h"

#include <SDL/SDL_image.h>

#include "cog.h"
#include "cog_anim.h"
#include "cog_core.h"
#include "cog_list.h"
#include "cog_log.h"
#include "cog_math.h"
#include "cog_sprite.h"

void cog_graphics_hwinit(void);
void cog_graphics_print_shader_error(int shaderid, char* type);
void cog_graphics_swinit(void);
//file io
void cog_graphics_read_file(char* buf, char* filename);
//rendering
void cog_graphics_render();
void cog_graphics_hwrender();
SDL_Surface* cog_graphics_load_image(const char* filename);

static char vertex_shader_str[] = ""
"attribute vec4 position;\n"
"attribute vec2 tex_coord;\n"
//"uniform mat4 modelview;\n"
//"uniform mat4 projection;\n"
"uniform mat4 transformation;\n"
"varying vec2 tex_coord_var;\n"
"void main()\n"
"{\n"
    //"vec4 p = modelview * position;\n"
    //"gl_Position = projection * p;\n"
    "gl_Position = transformation * position;\n"
    "tex_coord_var = tex_coord;\n"
"}\n";

static char fragment_shader_str[] = ""
"uniform sampler2D texture;\n"
"varying vec2 tex_coord_var;\n"
"void main()\n"
"{\n"
    "gl_FragColor = texture2D(texture, tex_coord_var);\n"
"}\n";

GLuint vertex_shader;
GLuint fragment_shader;
GLuint shader_program;
// uniforms
//GLuint modelview_uniform;
//GLuint projection_uniform;
GLuint transformation_uniform;
GLuint texture_uniform;
// attributes
GLuint position_attrib;
GLuint tex_coord_attrib;

static void cog_graphics_init_shaders(void)
{
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER); 
    char* buf = &(vertex_shader_str[0]);
    glShaderSource(vertex_shader, 1, (const GLchar**)&buf, NULL);
    buf = &(fragment_shader_str[0]);
    glShaderSource(fragment_shader, 1, (const GLchar**)&buf, NULL);
    glCompileShader(vertex_shader);
    cog_graphics_print_shader_error(vertex_shader, "Vertex");
    glCompileShader(fragment_shader);
    cog_graphics_print_shader_error(fragment_shader, "Fragment");
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    int link_status;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &link_status);
    if(! link_status)
    {
        cog_graphics_print_shader_error(shader_program, "Program");
    }
    //Set uniforms
    //modelview_uniform = glGetUniformLocation(shader_program, "modelview");
    //projection_uniform = glGetUniformLocation(shader_program, "projection");
    transformation_uniform = glGetUniformLocation(shader_program, "transformation");
    texture_uniform = glGetUniformLocation(shader_program, "texture");
    position_attrib = glGetAttribLocation(shader_program, "position");
    tex_coord_attrib = glGetAttribLocation(shader_program, "tex_coord");
}


/*-----------------------------------------------------------------------------
 * Sprites are drawn centred at the sprite's x and y coord, as opposed to most
 * engines where they are drawn from the top left.
 *-----------------------------------------------------------------------------*/
void cog_graphics_draw_sprite(cog_sprite* sprite)
{
//    glEnable(GL_TEXTURE_2D);
//    glPushMatrix();
    //Bind texture
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, sprite->texid);

    //Translate
//    glTranslatef(sprite->x,sprite->y, 0.0);
//    glRotatef(-cog_math_radtodeg(sprite->rot), 0.0f, 0.0f, 1.0f);

    /*
    GLfloat vertices[] = {
        -1.0f*sprite->w,1.0f*sprite->h,
        1.0f*sprite->w,1.0f*sprite->h,
        1.0f*sprite->w,-1.0f*sprite->h,
        -1.0f*sprite->w,-1.0f*sprite->h
    };
    GLfloat tex[] = {
        sprite->tex_pos.x, sprite->tex_pos.y + sprite->tex_dim.h,
        sprite->tex_pos.x + sprite->tex_dim.w, sprite->tex_pos.y + sprite->tex_dim.h,
        sprite->tex_pos.x + sprite->tex_dim.w, sprite->tex_pos.y,
        sprite->tex_pos.x, sprite->tex_pos.y};
        */

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glLoadIdentity();
    //glRotatef(-cog_math_radtodeg(sprite->rot), 0.0f, 0.0f, 1.0f);
    //glTranslatef(sprite->x, sprite->y, 0.0);

    glBindTexture(GL_TEXTURE_2D, sprite->texid);
    glUniform1i(texture_uniform, 0);

/*
    GLfloat transformation_matrix[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
//        -0.5f, -0.5f, 0.0f, 1.0f,
    };
*/
    GLfloat transformation_matrix[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        sprite->pos.x, sprite->pos.y, 0.0f, 1.0f,
//        -0.5f, -0.5f, 0.0f, 1.0f,
    };
    glUniformMatrix4fv(transformation_uniform, 1, GL_FALSE, transformation_matrix);

    //float X = sprite->pos.x;
    //float Y = sprite->pos.y;
    float X = 0.0f;
    float Y = 0.0f;
    float Z = 0.0f;
    //float Z = 0.0f;
    //float W = sprite->dim.w / 10.0f;
    //float H = sprite->dim.h / 10.0f;
    float W = 1.0f;
    float H = 1.0f;
    cog_debugf("x is %f\n", X);
    cog_debugf("y is %f\n", Y);
    cog_debugf("w is %f\n", W);
    cog_debugf("h is %f\n", H);
    GLfloat vertices[] = { 
        X,  Y+H, Z, //top left corner
        X+W,  Y+H, Z, //top right corner
        X+W, Y, Z, // bottom right rocner
        X, Y, Z}; //bottom left corner
    /*
    GLfloat vertices[] = {
        -1.0f*sprite->w, 1.0f*sprite->h,
        1.0f*sprite->w, 1.0f*sprite->h,
        1.0f*sprite->w, -1.0f*sprite->h,
        -1.0f*sprite->w, -1.0f*sprite->h
    };
    */
    //GLfloat tex[] = {1,0, 0,0, 0,1, 1,1};
    GLfloat tex[] = {
        sprite->tex_pos.x, sprite->tex_pos.y + sprite->tex_dim.h,
        sprite->tex_pos.x + sprite->tex_dim.w, sprite->tex_pos.y + sprite->tex_dim.h,
        sprite->tex_pos.x + sprite->tex_dim.w, sprite->tex_pos.y,
        sprite->tex_pos.x, sprite->tex_pos.y};
    GLubyte indices[] = {3,0,1, // first triangle (bottom left - top left - top right)
        3,1,2}; // second triangle (bottom left - top right - bottom right)


    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);


    // positions
    glEnableVertexAttribArray(position_attrib);
    //glVertexPointer(4, GL_FLOAT, 0, vertices);
    glVertexAttribPointer(position_attrib, 3, GL_FLOAT, GL_FALSE, 0, vertices);
 
    // enable and send vertex texture coordinate attribute data
    //TEXTURES
    //glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glEnableVertexAttribArray(tex_coord_attrib);
    glVertexAttribPointer(tex_coord_attrib, 2, GL_FLOAT, GL_FALSE, 0, tex);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    /*
    glPushMatrix();
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();
    float X = 100.f;
    float Y = 100.f;
    float Z = 0.0f;
    float W = 1000.0f;
    float H = 1000.0f;
    GLfloat vertices[] = { 
        X,  Y+H, 0, //top left corner
        X+W,  Y+H, 0, //top right corner
        X+W, Y, 0, // bottom right rocner
        X, Y, 0}; //bottom left corner
    GLfloat tex[] = {1,0, 0,0, 0,1, 1,1};
    GLubyte indices[] = {3,0,1, // first triangle (bottom left - top left - top right)
        3,1,2}; // second triangle (bottom left - top right - bottom right)


    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glPopMatrix();
    */

    /*

    GLubyte indices[] = {3,0,1, // first triangle (bottom left - top left - top right)
        3,1,2}; // second triangle (bottom left - top right - bottom right)

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();
    */
//    glDisable(GL_TEXTURE_2D);
}

void cog_graphics_draw_text(cog_text* text)
{
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    //Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, text->texid);

    //Translate
    glTranslatef(text->x+text->w,text->y+text->h, 0.0);
    glRotatef(-cog_math_radtodeg(text->rot), 0.0f, 0.0f, 1.0f);

    GLfloat vertices[] = { 
        -1.0f*text->w,1.0f*text->h,
        1.0f*text->w,1.0f*text->h,
        1.0f*text->w,-1.0f*text->h,
        -1.0f*text->w,-1.0f*text->h
    };
    GLfloat tex[] = {1,0, 0,0, 0,1, 1,1};
    GLubyte indices[] = {3,0,1, // first triangle (bottom left - top left - top right)
        3,1,2}; // second triangle (bottom left - top right - bottom right)

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void cog_graphics_hwrender()
{
    glClear( GL_COLOR_BUFFER_BIT );
    glLoadIdentity();

    cog_sprite_draw();
    cog_anim_draw();
    cog_text_draw();

/*
    glBegin(GL_TRIANGLES);

    glVertex3f(100,0,0);
    glVertex3f(0,100,0);
    glVertex3f(100,100,0);

    glEnd();
*/

#if !defined(HAVE_GLES)
    SDL_GL_SwapBuffers();
#else
    EGL_SwapBuffers();
#endif
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
    // Create the target alpha surface with correct color component ordering
    SDL_Surface* alphaimage = SDL_CreateRGBSurface(SDL_SWSURFACE,
            image->w,image->h,32,
            rmask,gmask,bmask,amask);
    if(!alphaimage)
    {
        fprintf(stderr, "cog_graphics_upload_surface : RGB surface creation failed.");
    }
    // Set up so that colorkey pixels become transparent :
    Uint32 colorkey = SDL_MapRGBA(alphaimage->format, rmask, gmask, bmask, amask);
    SDL_FillRect(alphaimage, 0, colorkey);
    colorkey = SDL_MapRGBA(image->format, rmask, gmask, bmask, amask);
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, colorkey);
    SDL_Rect area;
    SDL_SetAlpha(image, 0, amask); //http://www.gamedev.net/topic/518525-opengl--sdl--transparent-image-make-textures/
    // Copy the surface into the GL texture image :
    area.x = 0;
    area.y = 0;
    area.w = image->w;
    area.h = image->h;
    SDL_BlitSurface(image, &area, alphaimage, &area);
    // Create an OpenGL texture for the image
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Prepare the filtering of the texture image
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Map the alpha surface to the texture
    glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            w,
            h,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            alphaimage->pixels);
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
    cog_graphics_hwinit();
}

void cog_graphics_hwinit(void)
{
#ifdef HAVE_GLES
    //GLES
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.3f,0.3f,0.5f,0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0, cog_screenw(), cog_screenh(), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
#else
    glewInit();
    if(!GLEW_VERSION_2_1)
    {
        cog_errorf("Error: OpenGL 2.1 not available, bailing out.");
    }
    cog_graphics_init_shaders();
    glUseProgram(shader_program);
    //GLES
    glClearColor(0.3f,0.3f,0.5f,0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, cog_screenw(), cog_screenh(), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
}

void cog_graphics_print_shader_error(int shaderid, char* type)
{
    int maxlength;
    glGetShaderiv(shaderid, GL_INFO_LOG_LENGTH, &maxlength);
    char* errorbuf = (char*)malloc(sizeof(char)*maxlength);
    glGetShaderInfoLog(shaderid, maxlength, &maxlength, errorbuf);
    cog_debugf("Shader %s compilation:", type);
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
