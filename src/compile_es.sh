FILES="cog_anim.c \
cog_core.c \
cog_graphics.c \
cog_graphics_gles.c \
cog_group.c \
cog_input.c \
cog_list.c \
cog_log.c \
cog_main.c \
cog_map.c \
cog_math.c \
cog_snd.c \
cog_snd_sdl2.c \
cog_sprite.c \
cog_state.c \
cog_test.c \
cog_text.c \
cog_text_freetype.c \
cog_tween.c \
cog_window.c \
cog_window_gles.c \
cog_window_sdl.c"

LIBS="-I ../libs/zlib-1.2.5/src -I . -I ../libs/freetype/include -I../libs/jansson-2.6/src -I ../libs/libpng-1.2.49/src/ -I ../cog/src/ ../libs/libjansson.so ../libs/libfreetype.so ../libs/zlib-1.2.5/src/*.c ../libs/libpng-1.2.49/src/*.c"

PRELOADS="--preload-file assets"

emcc -O0 -std=c99 $FILES \ $LIBS $PRELOADS gles_util/esUtil.c gles_util/esShader.c ../tests/test_es.c -I gles_util -I. -o a.html -s GL_UNSAFE_OPTS=0
