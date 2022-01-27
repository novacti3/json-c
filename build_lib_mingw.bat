mkdir build
cd build

Rem TODO: Add user input for build options
cmake -S ../ -B . -G "MinGW Makefiles" -D JSON-C_BUILD_EXAMPLES=ON
cmake --build . 
pause