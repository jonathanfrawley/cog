SHELL 		 = /bin/sh
#CC    		 = gcc
CC    		 = emcc
#LFLAGS		 = `deps/bin/sdl-config --libs` -lalut -lSDL_ttf -lSDL_image `deps/bin/libpng-config --libs`
LFLAGS		 = -lSDL -lalut -lSDL_ttf -lSDL_image
GLFLAGS		 = -lGL

#SDLLIBS=`sdl-config --libs`
#CFLAGS       = -std=c99 -fPIC -Ldeps/lib -Isrc -Ideps/include/SDL -Ideps/include -L/usr/lib $(LFLAGS) $(GLFLAGS) $(SDLLIBS) -Ideps/include/libpng15
CFLAGS       = -std=c99 -fPIC -Isrc -Ideps/include/SDL -Ideps/include $(LFLAGS) $(GLFLAGS) -s FULL_ES2=1

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
	$(CC) -o bin/test_anim.html tests/test_anim.c $(CFLAGS) $(TARGET_LIB) --preload-file media/kitten_anim.png

test_list:
	$(CC) -o bin/test_list tests/test_list.c $(CFLAGS) $(TARGET_LIB)

test_sound:
	#$(CC) -o bin/test_sound tests/test_sound.c $(CFLAGS) $(TARGET_LIB)
	$(CC) -o bin/test_sound.html tests/test_sound.c $(CFLAGS) $(TARGET_LIB) --preload-file media/testsnd.wav 

test_state:
	$(CC) -o bin/test_state tests/test_state.c $(CFLAGS) $(TARGET_LIB)

test_text:
	$(CC) -o bin/test_text tests/test_text.c $(CFLAGS) $(TARGET_LIB)

clean:
	rm -rf $(OBJECTS)
	rm -rf $(TARGET_LIB)
	rm -rf bin
