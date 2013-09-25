#include "cog_input.h"

#include <SDL2/SDL.h>

#include "cog.h"
#include "cog_core.h"
#include "cog_log.h"
#include "cog_window.h"

static cog_uint MY_SDL_BUTTON_RIGHT_MASK = 8;

static cog_bool mouse_left_pressed;
static cog_bool mouse_right_pressed;
static cog_bool mouse_left_just_pressed = 0;
static cog_bool mouse_right_just_pressed;
static cog_float mouse_x;
static cog_float mouse_y;
static SDLKey key_pressed;
static cog_uint key_just_pressed;

void cog_input_blank() {
    key_just_pressed = COG_FALSE;
    mouse_left_just_pressed = COG_FALSE;
    mouse_right_just_pressed = COG_FALSE;
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

cog_bool cog_input_key_pressed(void) {
    return key_just_pressed;
}

SDLKey cog_input_key_code(void) {
    return key_pressed;
}

cog_bool cog_input_mouse_left_pressed() {
    return mouse_left_pressed;
}

cog_bool cog_input_mouse_left_just_pressed() {
    cog_bool just_pressed = mouse_left_just_pressed;
    //Stop mouse from sticking
    mouse_left_just_pressed = COG_FALSE;
    return just_pressed;
}

cog_bool cog_input_mouse_right_pressed() {
    return mouse_right_pressed;
}

cog_bool cog_input_mouse_right_just_pressed() {
    cog_bool just_pressed = mouse_right_just_pressed;
    //Stop mouse from sticking
    mouse_right_just_pressed = COG_FALSE;
    return just_pressed;
}

cog_float cog_input_mouse_x() {
    return mouse_x;
}

cog_float cog_input_mouse_y() {
    return mouse_y;
}

void cog_input_check_mouse(void) {
    cog_int x, y;
    cog_uint state = SDL_GetMouseState(&x, &y);
    mouse_x = (cog_float) x;
    mouse_y = (cog_float) y;
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

cog_bool cog_input_space_pressed() {
    if(cog_input_key_pressed()) {
        SDLKey key = cog_input_key_code();
        if(key == SDLK_SPACE) {
            return COG_TRUE;
        }
    }
    return COG_FALSE;
}
