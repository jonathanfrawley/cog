#cog - a KISS 2D game library written in C

##Getting started
Use cmake to build the project in a seperate directory.
For example on linux:

    mkdir build
    cd build
    cmake ../src
    make

This will build a bunch of tests.
To run one, first copy the media folder into the current directory:

    cp -rf ../media ./

and then run the test:

    ./test_anim


##What is cog?
Cog is a 2D game library written to allow games to be prototyped quickly without sacrificing speed or portability.
Developed with the KISS philosophy and borrowing concepts from the brilliant actionscript game framework flixel.

##Current status
At the moment, cog is in alpha stage.
Some omissions include platform support (linux and win32 only at the moment), a decent input handling system and physics.

##Future ideas
 - Make API more opaque (use the PIMPL idiom : https://en.wikipedia.org/wiki/Opaque_pointer)
 - C++ support

##Dependencies
 - SDL2
 - OpenGL
 - OpenAL
 - freealut
 - glew
 - freetype
 - libpng
 - zlib

In addition, to build cog you will need:
 - A C compiler (e.g - gcc)
 - cmake

See https://github.com/df3n5/cog_deps for some useful scripts to help build dependencies.

###Generating python bindings
The resultant library will be called libcog.so. 
A python shared library will also be created if you have first generated the bindings by going into the swig directory and executing:

    cp ../src/*.h ./
    swig -python cog.i

This will generate a cog.py file which you will need alongside the libcogsharedpython.so in the build directory.
To run a test of the python binding, you will need to go to the ``cog/lang/python`` directory and copy over the cog.py and libcogsharedpython.so into their correct places. 

On linux, to run an example from the root of the repo:

    cd lang/python
    cp ../../build/libcogsharedpython.so ./_cog.so
    cp ../../swig/cog.py ./
    cp -rf ../../media ./
    python test_anim.py


###Windows
 1. Install MSys + c compiler tool (w/ mingw-get-inst)
 2. Build deps in place using cog_deps repo and associated bash script.
 3. If you intend to use the python wrapper, you will need to install python and swig.

You will need to copy the resultant dlls to wherever you intend to run the program (distribute it from)

##Intro
###Screen Coordinates
The screen coordinates go from (-1.0f, -1.0f) in the bottom left of the screen to (1.0f, 1.0f) in the top right of the screen
(0.0f, 0.0f) is the centre of the screen.

See tests/

##License
GPLv3. See LICENSE.txt for a copy.

##Roadmap
###Emscripten support 
 - [DONE] Get image rendering in emscripten with SDL1.2
 - [DONE] Get image rotation and scaling working with SDL1.2
 - [DONE] Get audio working with OpenAL or SDL1.2_mixer
 - Get SDL1.2 working in cog again (Bringing up a window, make it another backend)
 - Play a sound
 - Get rendering working
 - Get animations working

###Other things
- [DONE] Fix rotations
 - [DONE] Add ability to plug in update funcs
 - Add some simple tweens
  >> Features wanted:
   -> Want to be able to change pos, rot, vel, ang_vel, alpha of a sprite or anim over a period of time.
   -> Have 4 types:
    == PERSIST Once and stay around at end
    == LOOPING After complete start at start
    == PINGPONG After complete start at end and go backwards, then go back and continue from start, and so on.
    == ONESHOT Once and die at end
   -> Model after gokit : http://forum.unity3d.com/threads/133823-Prime31-GoKit-Tween-Library-Live
 - Add importing of objects in tiled
 - Finish off tiled importer
 - Maybe rethink state changes to be more flixel-like?

###TODO port
 - Get working with cross compiler for OSX X

###TODO general
 - Think about using top-left coordinate system - this mimicks how files are read, how people read, etc. Maybe better then center-based as is currently?
 - Set media folder on init, so no need to type each time.
 - Good readable definition of all keycodes (enum?)
 - Normalisation of vectors built-in
 - Make shutdown more safe -> Deallocate memory and call shutdown functions in correct order
 - Status of SDL2 port: Anims, sprites and sound working. Mysterious crash on text example and text offsets are incorrect.

