#ifndef COG_DEFS_H
#define COG_DEFS_H

#define COG_RENDERER_SDL 0
#define COG_RENDERER_OPENGL 1
#define COG_RENDERER_SDL2 2

#define COG_AUDIO_SDL 0
#define COG_AUDIO_AL 1

#define COG_TEXT_SDL 0
#define COG_TEXT_FREETYPE 1

//Define defaults
#define COG_RENDERER COG_RENDERER_SDL
#define COG_AUDIO COG_AUDIO_SDL
#define COG_TEXT COG_TEXT_SDL

//#define USE_LEGACY_SDL //TODO : Switch to above system

#ifdef EMSCRIPTEN
#define USE_LEGACY_SDL
#define EMSCRIPTEN_ENABLED
#define DEBUG
#endif

#endif // COG_DEFS_H
