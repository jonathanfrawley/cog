#ifndef  COG_INPUT_H
#define  COG_INPUT_H

#include <SDL2/SDL_keyboard.h>

#include "cog_types.h"
#include "cog_window.h"

/*-----------------------------------------------------------------------------
 *  Blank the state of the input.
 *-----------------------------------------------------------------------------*/
void cog_input_blank();

/*-----------------------------------------------------------------------------
 *  Check whether the mouse has been pressed or not.
 *-----------------------------------------------------------------------------*/
void cog_input_check_mouse(void);

/*-----------------------------------------------------------------------------
 *  Check which keys are pressed and set flags accordingly.
 *-----------------------------------------------------------------------------*/
void cog_input_check_keys(void);

void cog_input_init(cog_window* window);

bool cog_input_joystick_depressed(void);

bool cog_input_joystick_x_depressed(void);

bool cog_input_joystick_y_depressed(void);

bool cog_input_joystick_pressed(void);

bool cog_input_joystick_x_pressed(void);

bool cog_input_joystick_y_pressed(void);

double cog_input_joystick_x_value();

double cog_input_joystick_y_value();

/*-----------------------------------------------------------------------------
 *  Key pressed in the last loop.
 *-----------------------------------------------------------------------------*/
bool cog_input_key_pressed(void);

/*-----------------------------------------------------------------------------
 *  Key depressed in the last loop.
 *-----------------------------------------------------------------------------*/
bool cog_input_key_depressed(void);

/*-----------------------------------------------------------------------------
 *  Key code pressed.
 *-----------------------------------------------------------------------------*/
uint32_t cog_input_key_code_pressed(void);

uint32_t cog_input_key_code_depressed(void);

/*-----------------------------------------------------------------------------
 *  Mouse left was pressed in the last main loop.
 *-----------------------------------------------------------------------------*/
bool cog_input_mouse_left_just_pressed();

/*-----------------------------------------------------------------------------
 *  Mouse left is pressed at the moment.
 *-----------------------------------------------------------------------------*/
bool cog_input_mouse_left_pressed();

/*-----------------------------------------------------------------------------
 *  Mouse right was pressed in the last main loop.
 *-----------------------------------------------------------------------------*/
bool cog_input_mouse_right_just_pressed();

/*-----------------------------------------------------------------------------
 *  Mouse right is pressed at the moment.
 *-----------------------------------------------------------------------------*/
bool cog_input_mouse_right_pressed();

/*-----------------------------------------------------------------------------
 *  Mouse x coord
 *-----------------------------------------------------------------------------*/
double cog_input_mouse_x();

/*-----------------------------------------------------------------------------
 *  Mouse y coord
 *-----------------------------------------------------------------------------*/
double cog_input_mouse_y();

void cog_input_quit(void);

bool cog_input_space_pressed();

#endif // COG_INPUT_H
