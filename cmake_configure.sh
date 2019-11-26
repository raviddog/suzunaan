rm -rf ./build/gcc
rm -rf ./build/vs

mkdir ./build/gcc
mkdir ./build/vs

cmake -G "Visual Studio 16 2019" -S . -B ./build/vs
cmake -G "MinGW Makefiles" -S . -B ./build/gcc -DCMAKE_SH="CMAKE_SH-NOTFOUND"
