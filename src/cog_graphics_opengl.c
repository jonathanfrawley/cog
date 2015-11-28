#include "cog_graphics_opengl.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <png.h>

#include "cog_defs.h"
#include "cog_log.h"
#include "cog_math.h"
#include "cog_main.h"
#include "cog_text_freetype.h"
#include "cog_window_sdl2.h"

GLuint cog_graphics_opengl_load_texture_png(const char* file_name, int* width, int* height);
cog_window* window;

void cog_graphics_opengl_set_camera_pos(cog_pos2* pos) {
    glTranslatef(-pos->x, -pos->y, 0.0);
}

static GLfloat* vertices;
static GLfloat* tex;
static GLuint* indices;
static const uint32_t vertex_amount = 12;
static const uint32_t tex_amount = 8;
static const uint32_t index_amount = 6;
static uint32_t sprite_amount;

//############### Shaders
typedef enum {
    SHADER_COLOR,
    SHADER_TEXTURE,
    SHADER_TEXCOORDS,
    SHADER_TEST,
    NUM_SHADERS
} cog_shader_type;

static int32_t current_shader = SHADER_TEXTURE;

typedef struct {
    GLhandleARB program;
    GLhandleARB vert_shader;
    GLhandleARB frag_shader;
    const char* vert_source;
    const char* frag_source;
} cog_shader_data;

static cog_shader_data shaders[NUM_SHADERS] = {
    /* SHADER_COLOR */
    {
        0, 0, 0,
        /* vertex shader */
        "varying vec4 v_color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
        "    v_color = gl_Color;\n"
        "}",
        /* fragment shader */
        "varying vec4 v_color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = v_color;\n"
        "}"
    },

    /* SHADER_TEXTURE */
    {
        0, 0, 0,
        /* vertex shader */
        "varying vec4 v_color;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
        "    v_color = gl_Color;\n"
        "    v_texCoord = vec2(gl_MultiTexCoord0);\n"
        "}",
        /* fragment shader */
        "varying vec4 v_color;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D tex0;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = texture2D(tex0, v_texCoord) * v_color;\n"
        "}"
    },

    /* SHADER_TEXCOORDS */
    {
        0, 0, 0,
        /* vertex shader */
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
        "    v_texCoord = vec2(gl_MultiTexCoord0);\n"
        "}",
        /* fragment shader */
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec4 color;\n"
        "    vec2 delta;\n"
        "    float dist;\n"
        "\n"
        "    delta = vec2(0.5, 0.5) - v_texCoord;\n"
        "    dist = dot(delta, delta);\n"
        "\n"
        "    color.r = v_texCoord.x;\n"
        "    color.g = v_texCoord.x * v_texCoord.y;\n"
        "    color.b = v_texCoord.y;\n"
        "    color.a = 1.0 - (dist * 4.0);\n"
        "    gl_FragColor = color;\n"
        "}"
    },

    {
        0, 0, 0,
        "varying vec2 uv;\n"
        "\n"
        "void main() {\n"
        "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
        "  uv = vec2(gl_MultiTexCoord0);\n"
        "}",
        "varying vec2 uv;\n"
        "void main() {\n"
        "    float border = 0.11;\n"
        "    float radius = 0.55;\n"
        "    vec4 color0 = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "    vec4 color1 = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "    vec2 m = uv - vec2(0.5, 0.5);\n"
        "    float dist = radius - sqrt(m.x * m.x + m.y * m.y);\n"
        "    float t = 0.0;\n"
        "    if (dist > border) {\n"
        "      t = 1.0;\n"
        "    } else if (dist > 0.0) {\n"
        "      t = dist / border;\n"
        "    }\n"
        "    gl_FragColor = mix(color0, color1, t);\n"
        "}"
    }
};
#ifdef USE_LEGACY_SDL
static bool shaders_supported = false;
static bool cog_graphics_opengl_init_shaders() {
    return true;
}
#else
static bool shaders_supported = true;


static PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
static PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
static PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
static PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
static PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
static PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
static PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
static PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
static PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
static PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
static PFNGLUNIFORM1IARBPROC glUniform1iARB;
static PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;

static bool cog_graphics_opengl_compile_shader(GLhandleARB shader, const char* source) {
    GLint status;
    glShaderSourceARB(shader, 1, &source, NULL);
    glCompileShaderARB(shader);
    glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
    if(status == 0) {
        GLint length;
        char* info;
        glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
        info = SDL_stack_alloc(char, length+1);
        glGetInfoLogARB(shader, length, NULL, info);
        cog_errorf("Failed to compile shader:\n%s\n%s", source, info);
        SDL_stack_free(info);
        return false;
    } else {
        return true;
    }
}

static bool cog_graphics_opengl_compile_shaderProgram(cog_shader_data* data) {
    const int num_tmus_bound = 4;
    int i;
    GLint location;
    glGetError();
    /* Create one program object to rule them all */
    data->program = glCreateProgramObjectARB();
    /* Create the vertex shader */
    data->vert_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    if(!cog_graphics_opengl_compile_shader(data->vert_shader, data->vert_source)) {
        return false;
    }
    /* Create the fragment shader */
    data->frag_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    if(!cog_graphics_opengl_compile_shader(data->frag_shader, data->frag_source)) {
        return false;
    }
    /* ... and in the darkness bind them */
    glAttachObjectARB(data->program, data->vert_shader);
    glAttachObjectARB(data->program, data->frag_shader);
    glLinkProgramARB(data->program);
    /* Set up some uniform variables */
    glUseProgramObjectARB(data->program);
    for(i = 0; i < num_tmus_bound; ++i) {
        char tex_name[5];
        SDL_snprintf(tex_name, SDL_arraysize(tex_name), "tex%d", i);
        location = glGetUniformLocationARB(data->program, tex_name);
        if(location >= 0) {
            glUniform1iARB(location, i);
        }
    }
    glUseProgramObjectARB(0);
    return (glGetError() == GL_NO_ERROR);
}

static void cog_graphics_destroy_shader_program(cog_shader_data* data) {
    if(shaders_supported) {
        glDeleteObjectARB(data->vert_shader);
        glDeleteObjectARB(data->frag_shader);
        glDeleteObjectARB(data->program);
    }
}

static bool cog_graphics_opengl_init_shaders() {
    int i;
    /* Check for shader support */
    shaders_supported = false;
    if(SDL_GL_ExtensionSupported("GL_ARB_shader_objects") &&
            SDL_GL_ExtensionSupported("GL_ARB_shading_language_100") &&
            SDL_GL_ExtensionSupported("GL_ARB_vertex_shader") &&
            SDL_GL_ExtensionSupported("GL_ARB_fragment_shader")) {
        glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) SDL_GL_GetProcAddress("glAttachObjectARB");
        glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) SDL_GL_GetProcAddress("glCompileShaderARB");
        glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glCreateProgramObjectARB");
        glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) SDL_GL_GetProcAddress("glCreateShaderObjectARB");
        glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC) SDL_GL_GetProcAddress("glDeleteObjectARB");
        glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) SDL_GL_GetProcAddress("glGetInfoLogARB");
        glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterivARB");
        glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC) SDL_GL_GetProcAddress("glGetUniformLocationARB");
        glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) SDL_GL_GetProcAddress("glLinkProgramARB");
        glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) SDL_GL_GetProcAddress("glShaderSourceARB");
        glUniform1iARB = (PFNGLUNIFORM1IARBPROC) SDL_GL_GetProcAddress("glUniform1iARB");
        glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glUseProgramObjectARB");
        if(glAttachObjectARB &&
                glCompileShaderARB &&
                glCreateProgramObjectARB &&
                glCreateShaderObjectARB &&
                glDeleteObjectARB &&
                glGetInfoLogARB &&
                glGetObjectParameterivARB &&
                glGetUniformLocationARB &&
                glLinkProgramARB &&
                glShaderSourceARB &&
                glUniform1iARB &&
                glUseProgramObjectARB) {
            shaders_supported = true;
        }
    }
    if(!shaders_supported) {
        cog_debugf("shaders not supported...");
        return false;
    }
    cog_debugf("shaders supported :)");
    /* Compile all the shaders */
    for(i = 0; i < NUM_SHADERS; ++i) {
        if(!cog_graphics_opengl_compile_shaderProgram(&shaders[i])) {
            cog_errorf("Unable to compile shader!\n");
            return false;
        }
    }
    /* We're done! */
    return true;
}

static void cog_graphics_opengl_quit_shaders() {
    int i;
    for(i = 0; i < NUM_SHADERS; ++i) {
        cog_graphics_destroy_shader_program(&shaders[i]);
    }
}
#endif

//############### OpenGL

void cog_graphics_opengl_prepare(uint32_t amount) {
    vertices = (GLfloat*)cog_malloc(sizeof(GLfloat) * vertex_amount * amount);
    tex = (GLfloat*)cog_malloc(sizeof(GLfloat) * tex_amount * amount);
    indices = (GLuint*)cog_malloc(sizeof(GLuint) * index_amount * amount);
    sprite_amount = amount;
}

//TODO: Refactor these into one common func
double cog_graphics_opengl_round_w(double n) {
    double w = window->w/2;
    double ret = n+1.0;
    ret = ret*w;
    ret = (ret > (floor(ret)+0.5)) ? ceil(ret) : floor(ret);
    ret = ret/w;
    return ret-1.0;
}

double cog_graphics_opengl_round_h(double n) {
    double h = window->h/2;
    double ret = n+1.0;
    ret = ret*h;
    ret = (ret > (floor(ret)+0.5)) ? ceil(ret) : floor(ret);
    ret = ret/h;
    return ret-1.0;
}

void cog_graphics_opengl_draw_rect(cog_rect* rect) {
    if(shaders_supported) {
				glUseProgramObjectARB(shaders[SHADER_COLOR].program);
		}
    //1.5 here is due to cos and sin rotation below.
    double w = rect->dim.w * 1.415;
    double h = rect->dim.h * 1.415;
    double x_offset = rect->pos.x;
    double y_offset = rect->pos.y;
    if(rect->pixel_perfect) {
        x_offset = cog_graphics_opengl_round_w(rect->pos.x);
        y_offset = cog_graphics_opengl_round_h(rect->pos.y);
    }
    //Do rotation and transformation ourselves.
    //Rotate by PI/4 because..
    //TODO: Looks like the quads are getting stretched on rotation. Fix this somehow
    double rot = rect->rot - COG_PI/4;

		GLfloat vertices[vertex_amount];
		GLuint indices[index_amount];
    vertices[0] = -1.0f * w * sin(rot) + x_offset;
    vertices[1] = 1.0f * h * cos(rot) + y_offset;
    vertices[2] = 0;
    vertices[3] = 1.0f * w * cos(rot) + x_offset;
    vertices[4] = 1.0f * h * sin(rot) + y_offset;
    vertices[5] = 0;
    vertices[6] = 1.0f * w * sin(rot) + x_offset;
    vertices[7] = -1.0f * h * cos(rot) + y_offset;
    vertices[8] = 0;
    vertices[9] = -1.0f * w * cos(rot) + x_offset;
    vertices[10] = -1.0f * h * sin(rot)+ y_offset;
    vertices[11] = 0;
    indices[0] = 3;
    indices[1] = 0;
    indices[2] = 1;
    indices[3] = 3;
    indices[4] = 1;
    indices[5] = 2;

    cog_color c = rect->col;
    glColor4d(c.r, c.g, c.b, c.a);

    glPushMatrix();
    glLoadIdentity();
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();

    //Restore alpha to normal
    glColor4f(1.0, 1.0, 1.0, 1.0);
}

void cog_graphics_opengl_draw_sprite(cog_sprite* sprite, uint32_t idx) {
    glBindTexture(GL_TEXTURE_2D, sprite->tex_id);
    uint32_t offset = idx * vertex_amount;
    //1.5 here is due to cos and sin rotation below.
    double w = sprite->dim.w * 1.415;
    double h = sprite->dim.h * 1.415;
    double x_offset = sprite->pos.x;
    double y_offset = sprite->pos.y;
    if(sprite->pixel_perfect) {
        x_offset = cog_graphics_opengl_round_w(sprite->pos.x);
        y_offset = cog_graphics_opengl_round_h(sprite->pos.y);
    }
    //Do rotation and transformation ourselves.
    //Rotate by PI/4 because..
    //TODO: Looks like the quads are getting stretched on rotation. Fix this somehow
    double rot = sprite->rot + COG_PI/4;
    vertices[offset + 0] = -1.0f * w * sin(rot) + x_offset;
    vertices[offset + 1] = 1.0f * h * cos(rot) + y_offset;
    vertices[offset + 2] = 0;
    vertices[offset + 3] = 1.0f * w * cos(rot) + x_offset;
    vertices[offset + 4] = 1.0f * h * sin(rot) + y_offset;
    vertices[offset + 5] = 0;
    vertices[offset + 6] = 1.0f * w * sin(rot) + x_offset;
    vertices[offset + 7] = -1.0f * h * cos(rot) + y_offset;
    vertices[offset + 8] = 0;
    vertices[offset + 9] = -1.0f * w * cos(rot) + x_offset;
    vertices[offset + 10] = -1.0f * h * sin(rot)+ y_offset;
    vertices[offset + 11] = 0;
    offset = idx * tex_amount;
    tex[offset + 0] = sprite->tex_pos.x;
    tex[offset + 1] = sprite->tex_pos.y + sprite->tex_dim.h;
    tex[offset + 2] = sprite->tex_pos.x + sprite->tex_dim.w;
    tex[offset + 3] = sprite->tex_pos.y + sprite->tex_dim.h;
    tex[offset + 4] = sprite->tex_pos.x + sprite->tex_dim.w;
    tex[offset + 5] = sprite->tex_pos.y;
    tex[offset + 6] = sprite->tex_pos.x;
    tex[offset + 7] = sprite->tex_pos.y;
    offset = idx * index_amount;
    uint32_t idx_offset = idx * 4;
    indices[offset + 0] = idx_offset + 3;
    indices[offset + 1] = idx_offset + 0;
    indices[offset + 2] = idx_offset + 1;
    indices[offset + 3] = idx_offset + 3;
    indices[offset + 4] = idx_offset + 1;
    indices[offset + 5] = idx_offset + 2;
}

void cog_graphics_opengl_init(cog_window* win) {
    if(shaders_supported) {
        cog_graphics_opengl_init_shaders();
    }
    window = win;
    GLenum error = GL_NO_ERROR;
    //Initialize Projection Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //Initialize Modelview Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //Initialize clear color
    glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
}

void cog_graphics_opengl_draw_text(cog_text* text) {
    cog_shader_type old_program = current_shader;
    if(shaders_supported) {
#ifndef USE_LEGACY_SDL
        //Disable shader for text
        glUseProgramObjectARB(0);
#else
        glUseProgramObjectARB(shaders[SHADER_COLOR].program);
#endif
    }
    cog_text_freetype* text_ft = cog_text_freetype_get(text->id);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, text_ft->tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    const char* p;
    FT_GlyphSlot g = text_ft->face->glyph;
    double x = text->pos.x ;
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
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glLoadIdentity();
        cog_color c = text->col;
        glColor4d(c.r, c.g, c.b, c.a);
        GLfloat vertices[] = {
            x2, -y2, 0, //top left
            x2 + w, -y2, 0, //top right
            x2 + w, -y2 - h, 0, //bottom right
            x2, -y2 - h, 0 //bottom left
        };
        GLfloat tex[] = {0, 0, 1, 0, 1, 1, 0, 1};
        GLubyte indices[] = { 3, 0, 1,      // first triangle (bottom left - top left - top right)
                              3, 1, 2       // second triangle (bottom left - top right - bottom right)
                            };
        glTexCoordPointer(2, GL_FLOAT, 0, tex);
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
        x += (g->advance.x >> 6) * sx;
        y += (g->advance.y >> 6) * sy;
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //Restore alpha to normal
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glPopMatrix();
}

uint32_t cog_graphics_opengl_gen_tex_id() {
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    return tex_id;
}

/*-----------------------------------------------------------------------------
 * Loads a transparent png image as a texure and uploads it to the GPU.
 * Returns: A reference to the texture which can be passed to glBindTexture
 *
 * Notes:
 * Source: https://github.com/DavidEGrayson/ahrs-visualizer/blob/master/png_texture.cpp
 * License: https://github.com/DavidEGrayson/ahrs-visualizer/blob/master/LICENSE.txt
 *-----------------------------------------------------------------------------*/
GLuint cog_graphics_opengl_load_texture_png(const char* file_name, int* width, int* height) {
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

uint32_t cog_graphics_opengl_load_texture(const char* filename, int* width, int* height) {
    return cog_graphics_opengl_load_texture_png(filename, 0, 0);
}

void cog_graphics_opengl_clear() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    if(shaders_supported) {
#ifndef USE_LEGACY_SDL
				// Texture is the default shader
        glUseProgramObjectARB(shaders[SHADER_TEXTURE].program);
#endif
    }
}

void cog_graphics_opengl_draw() {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, tex);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_TRIANGLES, 6 * sprite_amount, GL_UNSIGNED_INT, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    cog_free(vertices);
    cog_free(tex);
    cog_free(indices);
}

void cog_graphics_opengl_flush() {
    if(shaders_supported) {
#ifndef USE_LEGACY_SDL
        glUseProgramObjectARB(0);
#endif
    }
#ifdef USE_LEGACY_SDL
    SDL_GL_SwapBuffers();
#else
    SDL_GL_SwapWindow(cog_window_sdl2_get_window());
#endif
}
