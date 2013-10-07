SHELL 		 = /bin/sh
CC    		 = gcc
GLFLAGS		 = -lGL -lGLU -lGLEW
#TODO: Remove ttf and image
LFLAGS		 = `sdl2-config --libs` -lalut -lSDL2_ttf -l SDL2_image `pkg-config --libs libpng` `pkg-config --libs freetype2` $(GLFLAGS) 
CFLAGS       = -std=c99 -Wall -Werror -fPIC -Isrc `sdl2-config --cflags` `pkg-config --cflags libpng` `pkg-config --cflags freetype2` $(LFLAGS) -g

TARGET_LIB  = libcog.so
SOURCES = $(shell echo src/*.c)
HEADERS = $(shell echo src/*.h)
OBJECTS = $(SOURCES:.c=.o)

all: mkdirs cpdirs $(TARGET_LIB) tests cplib

testsdl:
	$(CC) $(CFLAGS) -o test sdl_test.c

mkdirs:
	$(shell mkdir bin)

cpdirs:
	$(shell cp -rf media bin)

cplib:
	$(shell cp $(TARGET_LIB) bin)

$(TARGET_LIB):
	$(CC) -shared $(CFLAGS) -o $(TARGET_LIB) $(SOURCES)

tests: test test_anim test_list test_sound test_sprite test_state test_text tmp_text

test:
	$(CC) -o bin/test tests/test.c $(CFLAGS) $(TARGET_LIB)

test_anim:
	$(CC) -o bin/test_anim tests/test_anim.c $(CFLAGS) $(TARGET_LIB)

test_list:
	$(CC) -o bin/test_list tests/test_list.c $(CFLAGS) $(TARGET_LIB)

test_sound:
	$(CC) -o bin/test_sound tests/test_sound.c $(CFLAGS) $(TARGET_LIB)

test_sprite:
	$(CC) -o bin/test_sprite tests/test_sprite.c $(CFLAGS) $(TARGET_LIB)

test_state:
	$(CC) -o bin/test_state tests/test_state.c $(CFLAGS) $(TARGET_LIB)

test_text:
	$(CC) -o bin/test_text tests/test_text.c $(CFLAGS) $(TARGET_LIB)

tmp_text:
	$(CC) -o bin/tmp_text tests/tmp_text.c $(CFLAGS) $(TARGET_LIB)

clean:
	rm -rf $(OBJECTS)
	rm -rf $(TARGET_LIB)
	rm -rf bin
