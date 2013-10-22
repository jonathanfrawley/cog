#include "cog_input.h"

#include <SDL2/SDL.h>

#include "cog.h"
#include "cog_core.h"
#include "cog_log.h"

static uint32_t MY_SDL_BUTTON_RIGHT_MASK = 8;

static bool mouse_left_pressed;
static bool mouse_right_pressed;
static bool mouse_left_just_pressed = 0;
static bool mouse_right_just_pressed;
static double mouse_x;
static double mouse_y;
static double window_w;
static double window_h;
static uint32_t key_pressed;
static uint32_t key_just_pressed;

void cog_input_blank() {
    key_just_pressed = COG_FALSE;
    mouse_left_just_pressed = COG_FALSE;
    mouse_right_just_pressed = COG_FALSE;
}

void cog_input_init(cog_window* win) {
    //Grab width and height from window
    window_w = win->w;
    window_h = win->h;
}

void cog_input_check_keys(void) {
    SDL_Event event;
    key_just_pressed = COG_FALSE;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                cog_quit();
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        cog_quit();
                        break;
                    default:
                        key_pressed = event.key.keysym.sym;
                        key_just_pressed = COG_TRUE;
                        cog_debugf("WARNING: Unhandled keypress <%d>",
                                   (event.key.keysym.sym));
                        break;
                }
        }
    }
}

bool cog_input_key_pressed(void) {
    return key_just_pressed;
}

uint32_t cog_input_key_code(void) {
    return key_pressed;
}

bool cog_input_mouse_left_pressed() {
    return mouse_left_pressed;
}

bool cog_input_mouse_left_just_pressed() {
    bool just_pressed = mouse_left_just_pressed;
    //Stop mouse from sticking
    mouse_left_just_pressed = COG_FALSE;
    return just_pressed;
}

bool cog_input_mouse_right_pressed() {
    return mouse_right_pressed;
}

bool cog_input_mouse_right_just_pressed() {
    bool just_pressed = mouse_right_just_pressed;
    //Stop mouse from sticking
    mouse_right_just_pressed = COG_FALSE;
    return just_pressed;
}

double cog_input_mouse_x() {
    return ((mouse_x/window_w)*2.0) - 1.0;
}

double cog_input_mouse_y() {
    return (((window_h - mouse_y) / window_h) * 2.0) - 1.0;
}

void cog_input_check_mouse(void) {
    int x, y;
    uint32_t state = SDL_GetMouseState(&x, &y);
    mouse_x = (double) x;
    mouse_y = (double) y;
    if(SDL_BUTTON_LEFT == SDL_BUTTON(state)) {
        if(!mouse_left_pressed) {
            mouse_left_just_pressed = COG_TRUE;
        } else {
            mouse_left_just_pressed = COG_FALSE;
        }
        mouse_left_pressed = COG_TRUE;
    } else {
        mouse_left_pressed = COG_FALSE;
    }
    if(MY_SDL_BUTTON_RIGHT_MASK == SDL_BUTTON(state)) {
        if(!mouse_right_pressed) {
            mouse_right_just_pressed = COG_TRUE;
        } else {
            mouse_right_just_pressed = COG_FALSE;
        }
        mouse_right_pressed = COG_TRUE;
    } else {
        mouse_right_pressed = COG_FALSE;
    }
}

bool cog_input_space_pressed() {
    if(cog_input_key_pressed()) {
        SDL_Keycode key = cog_input_key_code();
        if(key == SDLK_SPACE) {
            return COG_TRUE;
        }
    }
    return COG_FALSE;
}
