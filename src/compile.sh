../emscripten/emcc -I. -s USE_SDL_LEGACY=1 cog_anim.c cog_core.c cog_graphics.c cog_input.c cog_list.c cog_log.c cog_main.c cog_map.c cog_math.c cog_snd.c cog_sprite.c cog_state.c cog_test.c cog_text.c cog_window.c cog_window_sdl.c ../tests/test_emscripten.c -o a.html  --preload-file screenshot.png --verbose -s LEGACY_GL_EMULATION=1


