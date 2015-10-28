#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<string.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include "main.h"

/*
	Task List:
		+ Split the engine code from the game code.
		+ Split renderer stuff from game struct and functions.
		+ Implement assets system.
*/

#include "engine.c"

void loadAssets(Game* game)
{
	game->assets.textures.player = loadTexture(game, "assets/player.png");
	game->assets.textures.crab = loadTexture(game, "assets/crab.png");
	game->assets.textures.playerBullet = loadTexture(game, "assets/playerBullet.png");
	game->assets.textures.background = loadTexture(game, "assets/background.png");
	game->assets.textures.font = loadTexture(game, "assets/font.png");
	game->assets.textures.deathParticle = 
		loadTexture(game, "assets/deathParticle.png");
}

void loadLevel(Game* game)
{
	clearAllEntities(game);
	
	createPlayer(game, createPointf(160, 200));
	
	for(int i = 0; i < 8; ++i)
		for(int t = 0; t < 3; ++t)
			createEnemy(game, createPointf((i * 36) + 32, (t * 36) + 32));
			
	game->background.texture = &game->assets.textures.background;
	game->background.position = createPointf(0, 0);
	game->background.velocity = createPointf(0, 60);
	game->background.repeatY = true;
	game->background.depth = 1000;
}

void drawUi(Game* game)
{
	char uiString[255];
		
	char scoreString[9];
	intToFixedSizeString(game->state.score, 8, scoreString);
	
	char livesString[3];
	intToFixedSizeString(game->state.lives, 2, livesString);
	
	sprintf(uiString, 
		"score:%s    lives:%s    level:%d-%d", 
		scoreString, 
		livesString, 
		game->state.stage,
		game->state.level);
	drawText(game, 0, uiString, createPointf(4, 4), -1000);

	sprintf(uiString, 
		"fps:%d", 
		game->fps);
	drawText(game, 0, uiString, createPointf(4, 240-8-4), -1000);
}

void startGame(Game* game)
{
	initGame(game);
	
	defineFont(game, &game->assets.textures.font, createPointi(8, 8),
		"abcdefghijklmnopqrstuvwxyz0123456789.,:!-");
		
	loadLevel(game);
	
	while(game->running)
	{			
		updateGame(game);
		
		if(checkKey(game, SDLK_ESCAPE))
			game->running = false;
		if(checkKeyReleased(game, SDLK_r))
			loadLevel(game);
		if(entityCount(game, ENEMY) == 0)
			loadLevel(game);
				
		drawUi(game);
		
		drawGame(game);
	}
}

int main(int argc, char** argv)
{	
	Game* game = createGame("Shmup", 320, 240, 2);
	
	loadAssets(game);
	startGame(game);
}
