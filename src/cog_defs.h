#ifndef COG_DEFS_H
#define COG_DEFS_H

#define COG_RENDERER_OPENGL 0

#define COG_AUDIO_SDL 0
#define COG_AUDIO_AL 1

#define COG_TEXT_SDL 0
#define COG_TEXT_FREETYPE 1
#define COG_TEXT_SDL2 2

#define COG_WINDOW_SDL 0
#define COG_WINDOW_SDL2 1
#define COG_WINDOW_GLES 2

// Enabled for FPS logging
//#define COG_PERF_STATS 0

//Define defaults
#define COG_RENDERER COG_RENDERER_OPENGL
#define COG_AUDIO COG_AUDIO_AL
#define COG_TEXT COG_TEXT_FREETYPE
#define COG_WINDOW COG_WINDOW_SDL2

//#define USE_LEGACY_SDL //TODO : Switch to above system

#endif // COG_DEFS_H
