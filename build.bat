@echo off
cls
erase preprocessor.exe
gcc src/preprocessor.c -o preprocessor.exe -std=c99 -g
echo.
echo ##################
echo # Pre-Processing #
echo ##################
@echo on
preprocessor.exe src/behaviors.c
@echo off
erase game.exe
echo.
echo ##################
echo # Building       #
echo ##################
@echo on
gcc src/main.c -o game.exe -std=c99 -lsdl2 -lSDL2_image -g
@echo off
echo.
echo ##################
echo # Running        #
echo ##################
echo.
game.exe