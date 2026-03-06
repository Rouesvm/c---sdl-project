# factroy game
a factory game made using SDL3 and C plus plus

### 1. Clone the libraries below into a folder called vendor in the root directory. Create the vendor folder if it doesn't exist.
* SDL3
* SDL_ttf

#

### 2. After cloning those libraries into the vendor folder
* Go to SDL_ttf/external and run download.sh

#

### 3. Configuring the build directory and compiling.

#### Run these commands inside of the root directory!

#### Linux
1. `cmake -S . -B build`
2. `cmake --build build`

#### Windows
1. `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=toolchain-windows.cmake`
2. `cmake --build build`

#

### Congratulations! You now have an executable file. Ensure you have the executable in a directory with the assets for the game.
#### The structure would be as shown below.
* game/
    * ├─ assets/
    * └─ executable
