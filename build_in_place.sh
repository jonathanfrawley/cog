
#export LD_LIBRARY_PATH=win32/lib && make -f Makefile.static clean && make -f Makefile.static
dest_dir=win32

export CMAKE_LIBRARY_PATH="${CMAKE_LIBRARY_PATH}:$dest_dir/lib"
export CMAKE_INCLUDE_PATH="${CMAKE_INCLUDE_PATH}:$dest_dir/include/AL:$dest_dir/include/SDL2:$dest_dir/include/freetype2:$dest_dir/include/"
export C_INCLUDE_PATH="${C_INCLUDE_PATH}:$dest_dir/include"
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$dest_dir/lib"
mkdir -p build
pushd build
cmake -G"MSYS Makefiles" -DCMAKE_INSTALL_PREFIX:PATH= ../src
make
popd
