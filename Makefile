all:
	gcc -std=c99 src/preprocessor.c -lm -o preprocessor; ./preprocessor src/behaviors.c; gcc -std=c99 src/main.c -lSDL2 -lSDL2_image -lm -o game -g
run:
	./game
