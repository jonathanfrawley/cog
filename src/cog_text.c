#include "cog_text.h"

#include "cog_core.h"
#include "cog_graphics.h"
#include "cog_list.h"
#include "cog_map.h"

static cog_list activetexts; //text drawn(active) at the moment
static cog_map texts;
static cog_text_id textcnt;

static TTF_Font* default_font;
//TODO:Add option to set these.
static SDL_Color default_colour = {0, 0, 0, 0};
static const char* default_path = "../media/font/04B_03__.ttf"; 
static cog_uint default_ptsize = 8;
static cog_int default_renderstyle = TTF_STYLE_NORMAL;

cog_text_id cog_text_add(char* str)
{
    cog_text* text = COG_STRUCT_MALLOC(cog_text);
    text->id = textcnt++;
    TTF_SetFontStyle(default_font, default_renderstyle);
    SDL_Surface* textsurface = TTF_RenderText_Blended(default_font, str, default_colour);
    text->texid = cog_graphics_upload_surface(textsurface);
    text->font = default_font;
    text->ptsize = default_ptsize;
    text->colour = default_colour;
    text->x = 0;
    text->y = 0;
    text->rot = 0;
    text->w = textsurface->w;
    text->h = textsurface->h;
    SDL_FreeSurface(textsurface);
    cog_map_put(&texts, text->id, (cog_dataptr)text);
    cog_list_append(&activetexts, (cog_dataptr)&(text->id));
    return text->id;
}

cog_text* cog_text_get(cog_text_id id)
{
    return (cog_text*)cog_map_get(&texts, id);
}

void cog_text_remove(cog_text_id id)
{
    COG_LIST_FOREACH(&activetexts)
    {
        if(*((cog_text_id*)curr->data) == id)
        {
            cog_list_remove(&activetexts, curr->data);
            break;
        }
    }
    cog_map_remove(&texts, id);
}

void cog_text_removeall(void)
{
    cog_list_removeall(&activetexts);
}

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
void cog_text_init(void)
{
    if(TTF_Init() < 0)
    {
        cog_errorf("Couldn't initialize TTF: %s\n", SDL_GetError());
    }

    default_font = TTF_OpenFont(default_path, default_ptsize);
    if(default_font == COG_NULL)
    {
        cog_errorf("Couldn't load %d pt font from %s: %s\n",
                default_ptsize, default_path, SDL_GetError());
    }
    cog_map_init(&texts);
    cog_list_init(&activetexts, sizeof(cog_text_id));
}

void cog_text_draw(void)
{
    COG_LIST_FOREACH(&activetexts)
    {
        cog_text* text = cog_text_get(*(cog_text_id*)curr->data);
        cog_graphics_draw_text(text);
    }
}
