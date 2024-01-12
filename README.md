# Install Mingw64 on Windows
https://github.com/niXman/mingw-builds-binaries/releases -> the 1st X86 : x86_64-13.2.0-release-posix-seh-msvcrt-rt_v11-rev0.7z

# Install SDL2 on Windows
https://github.com/libsdl-org/SDL/releases/tag/release-2.28.5 ->  SDL2-devel-2.28.5-mingw.zip 
Put the download dir at the top of the application (the same dir where src is)

# Install SDL2 on mac with brew

# Chose playing with paddle or see infinite game
Compile with WITH_PADDLE option from contants.h to interact with the game

# Sync automatically with the screen parameter
SDL_RENDERER_PRESENTVSYNC option from contants.h to make a fluid game

# Compilation
With scons (python style makefile manager)
With MAKEFILE_WINDOWS (rename to Makefile to use it)
With Makefile for Mac or Linux
