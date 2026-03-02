#!/bin/bash
set -e 

mkdir -p vendor
cd vendor

git clone https://github.com/open-source-parsers/jsoncpp.git
git clone https://github.com/libsdl-org/SDL.git
git clone https://github.com/libsdl-org/SDL_ttf.git

cd SDL_ttf/external
chmod +x download.sh
./download.sh
cd ../..

# jsoncpp hard patch;
sed -i 's/set(CMAKE_CXX_STANDARD 11)/set(CMAKE_CXX_STANDARD 17)/' jsoncpp/CMakeLists.txt

cd jsoncpp
mkdir build && cd build
cmake .. -DBUILD_SHARED_LIBS=OFF -DJSONCPP_WITH_TESTS=OFF -DCMAKE_CXX_STANDARD=20 -DCMAKE_TOOLCHAIN_FILE=/path/to/mingw_toolchain.cmake
make -j$(nproc)

cd ../../../..

cmake -S . -B build-linux
cmake -S . -B build-windows -DCMAKE_TOOLCHAIN_FILE=toolchain-windows.cmake