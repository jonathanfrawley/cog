#cog - a KISS 2D game library written in C

##What is cog?
Cog is a 2D game library written to allow games to be prototyped quickly without sacrificing speed or portability.
Developed with the KISS philosophy and borrowing concepts from the brilliant actionscript game framework flixel.

##Current status
At the moment, cog is in alpha stage.
Some omissions include platform support (linux and win32 only at the moment), a decent input handling system and physics.

##Dependencies
 - SDL2
 - OpenGL
 - OpenAL
 - freealut
 - glew
 - freetype
 - libpng
 - zlib

See https://github.com/df3n5/cog_deps for some useful scripts to help build dependencies.

##Compiling
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
1) Install MSys + c compiler tool (w/ mingw-get-inst)
2) Build deps in place using cog_deps repo and associated bash script.
3) If you intend to use the python wrapper, you will need to install python and swig.

You will need to copy the resultant dlls to wherever you intend to run the program (distribute it from)

##Intro
###Screen Coordinates
The screen coordinates go from (-1.0f, -1.0f) in the bottom left of the screen to (1.0f, 1.0f) in the top right of the screen
(0.0f, 0.0f) is the centre of the screen.

See tests/

