gcc src/preprocessor.c -o preprocessor.exe -std=c99 -g
preprocessor.exe src/behaviors.c
gcc src/main.c -o game.exe -std=c99 -lsdl2 -lSDL2_image -g