rm -rf ./build/gcc
rm -rf ./build/gcc-release
rm -rf ./build/vs

mkdir ./build/gcc
mkdir ./build/gcc-release
mkdir ./build/vs

cmake -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-Og -g -Wall -Wpedantic -Wextra -Wformat" -S . -B ./build/gcc
cmake -G "MinGW Makefiles" -DCMAKE_SH="CMAKE_SH-NOTFOUND" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-w -O3" -S . -B ./build/gcc-release
cmake -G "Visual Studio 16 2019" -S . -B ./build/vs
