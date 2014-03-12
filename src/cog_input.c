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
static uint32_t key_depressed;
static uint32_t key_just_pressed;
static uint32_t key_just_depressed;
static bool joystick_x_pressed;
static bool joystick_x_depressed;
static bool joystick_y_pressed;
static bool joystick_y_depressed;
static double joystick_x_value;
static double joystick_y_value;
static SDL_Joystick* sdl_joystick_0 = COG_NULL;

void cog_input_blank() {
    key_just_pressed = COG_FALSE;
    key_just_depressed = COG_FALSE;
    mouse_left_just_pressed = COG_FALSE;
    mouse_right_just_pressed = COG_FALSE;
}

void cog_input_init(cog_window* win) {
    //Grab width and height from window
    window_w = win->w;
    window_h = win->h;
    //Check if there's any joysticks
    if(SDL_NumJoysticks() >= 1) {
        sdl_joystick_0 = SDL_JoystickOpen(0);
        if(sdl_joystick_0 == COG_NULL) {
            cog_errorf("Error initialising joystick.");
        }
    }
}

void cog_input_check_keys(void) {
    SDL_Event event;
    key_just_pressed = COG_FALSE;
    key_just_depressed = COG_FALSE;
    joystick_x_pressed = false;
    joystick_y_pressed = false;
    joystick_x_depressed = false;
    joystick_y_depressed = false;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            cog_quit();
        }
        if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.sym == SDLK_ESCAPE) {
                cog_quit();
            } else {
                key_pressed = event.key.keysym.sym;
                key_just_pressed = COG_TRUE;
            }
            cog_state_global_fsm_push_event(COG_E_KEYDOWN);
        }
        if(event.type == SDL_KEYUP) {
            key_depressed = event.key.keysym.sym;
            key_just_depressed = COG_TRUE;
        }
        if(event.type == SDL_JOYAXISMOTION) {
            //TODO: Handle multiple joysticks.
            if(event.jaxis.which == 0) {
                if(event.jaxis.axis == 0) {
                    if((event.jaxis.value > -8000) && (event.jaxis.value < 8000)) {
                        joystick_x_depressed = true;
                    } else {
                        joystick_x_pressed = true;
                        if(event.jaxis.value < 0) {
                            joystick_x_value = -1.0;
                        } else {
                            joystick_x_value = 1.0;
                        }
                    }
                }
                if(event.jaxis.axis == 1) {
                    if((event.jaxis.value > -8000) && (event.jaxis.value < 8000)) {
                        joystick_y_depressed = true;
                    } else {
                        joystick_y_pressed = true;
                        if(event.jaxis.value < 0) {
                            joystick_y_value = 1.0;
                        } else {
                            joystick_y_value = -1.0;
                        }
                    }
                }
            }
        }
    }
}

bool cog_input_joystick_depressed(void) {
    return joystick_x_depressed || joystick_y_depressed;
}

bool cog_input_joystick_x_depressed(void) {
    return joystick_x_depressed;
}

bool cog_input_joystick_y_depressed(void) {
    return joystick_y_depressed;
}

bool cog_input_joystick_pressed(void) {
    return joystick_x_pressed || joystick_y_pressed;
}

bool cog_input_joystick_x_pressed(void) {
    return joystick_x_pressed;
}

bool cog_input_joystick_y_pressed(void) {
    return joystick_y_pressed;
}

double cog_input_joystick_x_value() {
    return joystick_x_value;
}

double cog_input_joystick_y_value() {
    return joystick_y_value;
}

bool cog_input_key_pressed(void) {
    return key_just_pressed;
}

bool cog_input_key_depressed(void) {
    return key_just_depressed;
}

uint32_t cog_input_key_code_pressed(void) {
    return key_pressed;
}

uint32_t cog_input_key_code_depressed(void) {
    return key_depressed;
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

void cog_input_quit(void) {
    //SDL_JoystickClose(sdl_joystick_0);
}

bool cog_input_space_pressed(void) {
    if(cog_input_key_pressed()) {
        SDL_Keycode key = cog_input_key_code_pressed();
        if(key == SDLK_SPACE) {
            return COG_TRUE;
        }
    }
    return COG_FALSE;
}

