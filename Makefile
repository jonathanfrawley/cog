SHELL 		 = /bin/sh
CC    		 = gcc
#LFLAGS		 = deps/lib/libGLEW.so deps/lib/cogSDL.so deps/lib/libSDL_ttf.so deps/lib/libopenal.so deps/lib/libGLEWmx.so deps/lib/libSDL_image.so deps/lib/libalut.so deps/lib/libpng.so -lm
#LFLAGS		 = `deps/bin/sdl-config --libs` -lalut -lSDL_ttf -lSDL_image `deps/bin/libpng-config --libs` deps/lib/libpng16.a
LFLAGS		 = `deps/bin/sdl-config --libs` -lalut -lSDL_ttf -lSDL_image `deps/bin/libpng-config --libs`
#LFLAGS		 = deps/lib/libGLEW.so -lSDL_cog deps/lib/libSDL_ttf.so deps/lib/libopenal.so deps/lib/libGLEWmx.so deps/lib/libSDL_image.so deps/lib/libalut.so deps/lib/libpng16_cog.so -lm 
GLFLAGS		 = -lGL

SDLLIBS=`sdl-config --libs`
#CFLAGS       = -std=c99 -fPIC -Ldeps/lib -Isrc -Ideps/include $(LFLAGS) $(GLFLAGS) -isystem deps/include -nodefaultlibs
#CFLAGS       = -std=c99 -fPIC -Ldeps/lib -Isrc -Ideps/include_deps $(LFLAGS) $(GLFLAGS) -Wl,--dll-search-prefix=cog
#Can't use this...
#CFLAGS       = -std=c99 -fPIC -Ldeps/lib -Isrc -Ideps/include_deps -I/usr/include -L/usr/lib $(LFLAGS) $(GLFLAGS) --sysroot=deps
CFLAGS       = -std=c99 -fPIC -Ldeps/lib -Isrc -Ideps/include/SDL -Ideps/include -L/usr/lib $(LFLAGS) $(GLFLAGS) $(SDLLIBS) -Ideps/include/libpng15
#CFLAGS_COG       = -std=c99 -fPIC -Ldeps/lib -Isrc -Ideps/include_deps -I/usr/include -L/usr/lib $(LFLAGS) $(GLFLAGS) -Wl,-z

TARGET_LIB  = libcog.so
SOURCES = $(shell echo src/*.c)
HEADERS = $(shell echo src/*.h)
OBJECTS = $(SOURCES:.c=.o)

all: mkdirs $(TARGET_LIB) tests

testsdl:
	$(CC) $(CFLAGS) -o test sdl_test.c

mkdirs:
	$(shell mkdir bin)

$(TARGET_LIB):
	$(CC) -shared $(CFLAGS) -o $(TARGET_LIB) $(SOURCES)

tests: test test_anim test_list test_sound test_state test_text

test:
	$(CC) -o bin/test tests/test.c $(CFLAGS) $(TARGET_LIB)

test_anim:
	$(CC) -o bin/test_anim tests/test_anim.c $(CFLAGS) $(TARGET_LIB)

test_list:
	$(CC) -o bin/test_list tests/test_list.c $(CFLAGS) $(TARGET_LIB)

test_sound:
	$(CC) -o bin/test_sound tests/test_sound.c $(CFLAGS) $(TARGET_LIB)

test_state:
	$(CC) -o bin/test_state tests/test_state.c $(CFLAGS) $(TARGET_LIB)

test_text:
	$(CC) -o bin/test_text tests/test_text.c $(CFLAGS) $(TARGET_LIB)

clean:
	rm -rf $(OBJECTS)
	rm -rf $(TARGET_LIB)
	rm -rf bin
