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
		+ Make the behavior system more automated in terms of updating and
		  destroying behaviors and all that.
		+ Split the engine code from the game code.
*/

// Pre-processor functions
#define defineBehavior(name,structName,updateFunction,drawFunction)

#include "engine.c"
#include "behaviors.c"
#include "generated.c"
#include "entities.c"

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

Game* createGame(char* title, int width, int height, float windowScale)
{
	printf("Allocating %fMB of memory...\n", (float)sizeof(Game)/1024/1024);
	Game* game = (Game*)calloc(1, sizeof(Game));
	printf("Success!\n");
	game->internalResolution = createPointi(width, height);
	game->windowScale = windowScale;
	
	State state;
	state.level = 1;
	state.stage = 1;
	state.lives = 5;
	state.score = 0;
	game->state = state;
	
	game->window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		(int)((float)width * windowScale), (int)((float)height * windowScale),
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN); 
		//SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	game->renderer = 
		SDL_CreateRenderer(game->window, -1, 
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_RenderSetLogicalSize(game->renderer, width, height);
		
	SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
	
	game->clearColor = createColor(0, 0, 0, 255);
	
	defineFont(game, &game->assets.textures.font, createPointi(8, 8),
		"abcdefghijklmnopqrstuvwxyz0123456789.,:!-");
		
	return game;
}

void updateGame(Game* game)
{
	if(checkKeyReleased(game, SDLK_r))
		loadLevel(game);
	if(entityCount(game, ENEMY) == 0)
		loadLevel(game);
}

void startGame(Game* game)
{
	srand(time(null));
	
	game->running = true;
	game->camera.size = game->internalResolution;
	game->camera.position = 
		createPointf(
			(float)game->camera.size.x / 2, 
			(float)game->camera.size.y / 2);
	
	initBehaviorPools(&game->behaviorPools);
	loadLevel(game);

	SDL_Event event;
	int lastTime = SDL_GetTicks();
	while(game->running)
	{
		int time = SDL_GetTicks();
		game->delta = (float)(time - lastTime) / 1000.0f;
		lastTime = time;
		
		for(int i = 0; i < MAX_KEYS; ++i)
			game->keysPressed[i] = game->keysReleased[i] = false;
		
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					game->running = false;
					break;
				case SDL_KEYUP:
					game->keys[event.key.keysym.sym & 255] = false;
					game->keysPressed[event.key.keysym.sym & 255] = true;
					break;
				case SDL_KEYDOWN:
					game->keys[event.key.keysym.sym & 255] = true;
					game->keysReleased[event.key.keysym.sym & 255] = true;
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					break;
			}
		}
		
		if(checkKey(game, SDLK_ESCAPE))
			game->running = false;
				
		updateGame(game);
		updateBackground(game);
				
		updateBehaviorPools(game, &game->behaviorPools);
				
		for(int i = 0; i < MAX_ENTITIES_PER_GAME; ++i)
			if(game->entities[i].alive)
				updateEntity(game, &game->entities[i]);
				
		updateParticles(game);
		
		setDrawColor(game, game->clearColor);
		SDL_RenderClear(game->renderer);
		
		drawBackground(game);
		
		for(int i = 0; i < MAX_ENTITIES_PER_GAME; ++i)
			if(game->entities[i].alive)
				drawEntity(game, &game->entities[i]);
				
		drawBehaviorPools(game, &game->behaviorPools);
				
		drawParticles(game);
				
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
		
		sortRenderQueue(game);
		flushRenderQueue(game);

		SDL_RenderPresent(game->renderer);
		
		game->frameTimer += game->delta;
		game->frameCounter++;
		if(game->frameTimer > 1)
		{
			game->frameTimer -= 1;
			if(game->frameTimer > 1)
				game->frameTimer = 0;
				
			game->fps = game->frameCounter;
			game->frameCounter = 0;
		}
	}
}

int main(int argc, char** argv)
{	
	Game* game = createGame("Shmup", 320, 240, 2);
	
	loadAssets(game);
	startGame(game);
}
