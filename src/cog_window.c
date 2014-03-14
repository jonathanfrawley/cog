#include "cog_window.h"

#include "cog_log.h"

#ifdef LEGACY_SDL
#include "cog_window_sdl.h"
#else
#include "cog_window_sdl2.h"
#endif

typedef struct {
    void (*init)(cog_config, cog_window*);
    void (*update)(cog_window*);
    void (*quit)(cog_window*);
    void (*toggle_fullscreen)(cog_window*);
} cog_wm;

static cog_wm wm;

void cog_window_init(cog_config config, cog_window* window) {
#ifdef LEGACY_SDL
    wm.init = cog_window_sdl_init;
    wm.update = cog_window_sdl_update;
    wm.quit = cog_window_sdl_quit;
    wm.toggle_fullscreen = cog_window_sdl_toggle_fullscreen;
#else
    wm.init = cog_window_sdl2_init;
    wm.update = cog_window_sdl2_update;
    wm.quit = cog_window_sdl2_quit;
    wm.toggle_fullscreen = cog_window_sdl2_toggle_fullscreen;
#endif
    wm.init(config, window);
}

void cog_window_update(cog_window* window) {
    wm.update(window);
}

void cog_window_quit(cog_window* window) {
    wm.quit(window);
}

void cog_window_toggle_fullscreen(cog_window* window) {
    wm.toggle_fullscreen(window);
}
