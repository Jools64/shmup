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

int randomi(int max)
{
	return (rand() % max) + 1;
}

float randomf(float max)
{
	return ((float)rand() / RAND_MAX) * max;
}

int randomBetweeni(int min, int max)
{
	int delta = max - min;
	return min + randomi(delta);
}

float randomBetweenf(float min, float max)
{
	int delta = max - min;
	return min + randomf(delta);
}

void quickSorti(int* values, int valueCount)
{
	if(valueCount < 2)
		return;
		
	int pivot = values[valueCount/2];
	
	int i, j;
	for(i = 0, j = valueCount - 1; ; ++i, --j)
	{
		while(values[i] < pivot)
			++i;
			
		while(pivot < values[j])
			--j;
			
		if(i >= j)
			break;
			
		int temp = values[i];
		values[i] = values[j];
		values[j] = temp;
	}
		
	quickSorti(values, i);
	quickSorti(values + i, valueCount - i);
}

float degToRad(float degrees)
{
	return degrees * (PI / 180);
}

Color createColor(byte r, byte g, byte b, byte a)
{
	Color color = {r, g, b, a};
	return color;
}

Pointi createPointi(int x, int y)
{
	Pointi point = {x, y};
	return point;
}

Pointf createPointf(float x, float y)
{
	Pointf point = {x, y};
	return point;
}

SDL_Point getSdlPointi(Pointi point)
{
	SDL_Point sdlPoint = {point.x, point.y};
	return sdlPoint;
}

Pointf addPointfs(Pointf a, Pointf b)
{
	return createPointf(a.x + b.x, a.y + b.y);
}

Pointf subtractPointfs(Pointf a, Pointf b)
{
	return createPointf(a.x - b.x, a.y - b.y);
}

Pointf scalePointf(Pointf point, float scaler)
{
	return createPointf(point.x * scaler, point.y * scaler);
}

Pointi scalePointi(Pointf point, float scaler)
{
	return createPointi((int)((float)point.x * scaler), 
						(int)((float)point.y * scaler));
}

Rectanglei createRectanglei(int x, int y, int width, int height)
{
	Rectanglei rectangle = {x, y, width, height};
	return rectangle;
}

Texture loadTexture(Game* game, char* filePath)
{
	Texture texture = {0};

    SDL_Surface* surface = IMG_Load(filePath);
    if(!surface)
        printf("Unable to load image \"%s\". SDL_image Error: %s\n", 
			filePath, IMG_GetError());
    else
    {
        texture.data = SDL_CreateTextureFromSurface(game->renderer, surface);
        texture.width = surface->w;
        texture.height = surface->h;
        
        
        if(!texture.data)
            printf("Unable to create texture from %s! SDL Error: %s\n", 
				filePath, SDL_GetError());

        SDL_FreeSurface(surface);
    }

    return texture;
}

void pushRenderItem(Game* game, RenderItem renderItem)
{
	if(game->renderItemCount < MAX_RENDER_ITEMS_PER_GAME)
	{
		RenderItem* item = &game->renderItems[game->renderItemCount];
		
		*item = renderItem;
		
		++game->renderItemCount;
	}
}

void internalDrawTextureSectionExt(Game* game, Texture* texture, Pointf position,
	Pointi sourcePosition, Pointi sourceSize, Pointf scale, float angle,
	Pointi origin, bool horizontalFlip, bool verticalFlip)
{
	Pointi offset;
	offset.x = (sourceSize.x * (scale.x - 1)) / 2;
	offset.y = (sourceSize.y * (scale.y - 1)) / 2;
	
	SDL_Rect source = 
	{
		sourcePosition.x, sourcePosition.y, 
		sourceSize.x, sourceSize.y
	};
		
	SDL_Rect destination = 
	{
		position.x - offset.x, position.y - offset.y, 
		sourceSize.x * scale.x, sourceSize.y * scale.y
	};
	
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if(horizontalFlip)
		flip = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | flip);
	if(verticalFlip)
		flip = (SDL_RendererFlip)(SDL_FLIP_VERTICAL | flip);
	
	SDL_Point originSdl = getSdlPointi(origin);
	
	SDL_RenderCopyEx(game->renderer, texture->data, &source, &destination,
		angle, &originSdl, flip);
}

void drawTextureSectionExt(Game* game, Texture* texture, Pointf position,
	Pointi sourcePosition, Pointi sourceSize, Pointf scale, float angle,
	Pointi origin, bool horizontalFlip, bool verticalFlip, int depth)
{
	RenderItem item;
	
	item.depth = depth;
	
	item.texture = texture; 
	item.position = position;
	item.sourcePosition = sourcePosition;
	item.sourceSize = sourceSize;
	item.scale = scale; 
	item.origin = origin;
	item.angle = angle;
	item.horizontalFlip = horizontalFlip;
	item.verticalFlip = verticalFlip;
	
	pushRenderItem(game, item);
}

void drawTextureSection(Game* game, Texture* texture, Pointf position,
	Pointi sourcePosition, Pointi sourceSize, int depth)
{
	drawTextureSectionExt(game, texture, position,
		sourcePosition, sourceSize, createPointf(1, 1), 0,
		createPointi(texture->width/2, texture->height/2), false, false,
		depth);
}

void drawTexture(Game* game, Texture* texture, Pointf position, int depth)
{
	drawTextureSection(game, texture, position,
		createPointi(0, 0), createPointi(texture->width, texture->height),
		depth);
}

void drawTextureExt(Game* game, Texture* texture, Pointf position, 
	Pointf scale, float angle, Pointi origin, 
	bool horizontalFlip, bool verticalFlip, int depth)
{
	if(scale.x < 0)
		scale.x = 0;
	if(scale.y < 0)
		scale.y = 0;
	
	drawTextureSectionExt(game, texture, position,
		createPointi(0, 0), createPointi(texture->width, texture->height),
			scale, angle, origin, horizontalFlip, verticalFlip, depth);
}

Pointi calculateTileSourcePosition(Texture* texture, Pointi size, int id)
{
	Pointi sourcePosition;
	sourcePosition.x = (id * size.x) % texture->width;
	sourcePosition.y = ((id * size.x) / texture->width) * size.x; 
	return sourcePosition;
}

void drawTile(Game* game, Texture* texture, 
	Pointf position, Pointi size, int id, int depth)
{
	Pointi sourcePosition = calculateTileSourcePosition(texture, size, id);
	drawTextureSection(game, texture, position, sourcePosition, size, depth);
}

void drawTileExt(Game* game, Texture* texture, 
	Pointf position, Pointi size, int id, Pointf scale, float angle,
	Pointi origin, bool horizontalFlip, bool verticalFlip, int depth)
{
	Pointi sourcePosition = calculateTileSourcePosition(texture, size, id);
	drawTextureSectionExt(game, texture, position, sourcePosition, size, scale,
		angle, origin, horizontalFlip, verticalFlip, depth);
}

void quickSortRenderItems(RenderItem* values, int valueCount)
{
	if(valueCount < 2)
		return;
		
	RenderItem pivot = values[valueCount/2];
	
	int i, j;
	for(i = 0, j = valueCount - 1; ; ++i, --j)
	{
		while(-values[i].depth < -pivot.depth)
			++i;
			
		while(-pivot.depth < -values[j].depth)
			--j;
			
		if(i >= j)
			break;
			
		RenderItem temp = values[i];
		values[i] = values[j];
		values[j] = temp;
	}
		
	quickSortRenderItems(values, i);
	quickSortRenderItems(values + i, valueCount - i);
}


void sortRenderQueue(Game* game)
{
	quickSortRenderItems(game->renderItems, game->renderItemCount);
}

void flushRenderQueue(Game* game)
{
	for(int i = 0; i < game->renderItemCount; ++i)
	{
		
		
		RenderItem* item = &game->renderItems[i];
		internalDrawTextureSectionExt(game, item->texture, 
			createPointf(
				item->position.x - game->camera.position.x + (game->camera.size.x * 0.5),
				item->position.y - game->camera.position.y + (game->camera.size.y * 0.5)
			),
			item->sourcePosition, item->sourceSize,
			item->scale, item->angle, item->origin, 
			item->horizontalFlip, item->verticalFlip); 
	}
	
	game->renderItemCount = 0;
}

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

bool doRectangleisIntersect(Rectanglei a, Rectanglei b)
{
	return !(a.x > b.x + b.width
		  || a.y > b.y + b.height
		  || a.x + a.width < b.x
		  || a.y + a.height < b.y);
}

void setDrawColor(Game* game, Color color)
{
	SDL_SetRenderDrawColor( 
		game->renderer, 
		color.r, color.g, color.b, color.a );
}

void drawRectangle(Game* game, Rectanglei rectangle, Color color)
{
	SDL_Rect r;
	
    r.x = rectangle.x;
    r.y = rectangle.y;
    r.w = rectangle.width;
    r.h = rectangle.height;

    setDrawColor(game, color);
    SDL_RenderFillRect(game->renderer, &r);
}

bool checkKey(Game* game, int key)
{
	return game->keys[key & 255];
}

bool checkKeyPressed(Game* game, int key)
{
	return game->keysPressed[key & 255];
}

bool checkKeyReleased(Game* game, int key)
{
	return game->keysReleased[key & 255];
}

int defineSpriteAnimation(
	Sprite* sprite, int* frames, int frameCount, float frameRate)
{
	Animation* animation = &sprite->animations[sprite->animationCount];
	animation->frameCount = frameCount;
	memcpy(animation->frames, frames, sizeof(int) * frameCount);
	sprite->animationCount++;
	
	return sprite->animationCount - 1;
}

void playSpriteAnimation(Sprite* sprite, int animationId)
{
	sprite->currentAnimation = animationId;
}

Sprite* attachSpriteToEntity(Entity* entity, Texture* texture, Pointi size)
{
	Sprite* sprite = &entity->sprite;
	
	sprite->texture = texture;
	sprite->size = size;
	sprite->frameRate = 4;
	sprite->currentAnimation = -1;
	sprite->alive = true;
	sprite->loopAnimation = true;
	sprite->scale = createPointf(1, 1);
	sprite->origin = 
		createPointi(sprite->size.x/2, sprite->size.y/2);
	
	return sprite;
}

void updateSprite(Game* game, Sprite* sprite)
{
	if(sprite->currentAnimation >= 0)
	{
		Animation* animation = &sprite->animations[sprite->currentAnimation];
		
		sprite->frameTimer += sprite->frameRate * game->delta;
		
		if(sprite->frameTimer >= animation->frameCount)
		{
			sprite->animationDone = true;
			
			if(sprite->loopAnimation)
			{
				sprite->frameTimer = 
					sprite->frameTimer 
					- ((int)(sprite->frameTimer/animation->frameCount)
					* animation->frameCount);
			}
			else
				sprite->frameTimer = animation->frameCount - 1;
		}
			
		sprite->currentFrame = animation->frames[(int)sprite->frameTimer];
	}
}

Pointf getEntityDrawPosition(Entity* entity)
{
	return createPointf(
		entity->position.x - entity->origin.x, 
		entity->position.y - entity->origin.y
	);
}

void drawSprite(Game* game, Sprite* sprite)
{
	drawTileExt(game, sprite->texture, sprite->position, 
		sprite->size, (int)sprite->currentFrame,
		sprite->scale, sprite->angle, sprite->origin, 
		sprite->horizontalFlip, sprite->verticalFlip, sprite->depth);
}

Particle* createParticle(Game* game, Texture* texture, Pointf position)
{
	Particle* particle = null;
	
	if(game->particleCount < MAX_PARTICLES_PER_GAME)
	{
		for(int i = game->lastParticleIndex; i < MAX_PARTICLES_PER_GAME; ++i)
		{
			if(!game->particles[i].alive)
			{
				particle = &game->particles[i];
				game->lastParticleIndex = i;
				break;
			}
		}
		
		if(!particle)
		{
			for(int i = 0; i < game->lastParticleIndex; ++i)
			{
				if(!game->particles[i].alive)
				{
					particle = &game->particles[i];
					game->lastParticleIndex = i;
					
					break;
				}
			}
		}
		
		if(particle)
		{
			game->particleCount++;
			
			particle->alive = true;
			particle->texture = texture;
			particle->life = 1;
			particle->scale.value = 1;
			particle->position = position;
			particle->origin = 
				createPointi(texture->width/2, texture->height/2);
			particle->offset = 
				createPointf(texture->width/2, texture->height/2);
		}
	}
	
	return particle;
}

void destroyParticle(Game* game, Particle* particle)
{
	game->particleCount--;
	particle->alive = false;
}

void updateParticleAttributef(Game* game, ParticleAttributef* attribute)
{
	attribute->delta += attribute->acceleration * game->delta;
	attribute->value += attribute->delta * game->delta;
}

void updateParticles(Game* game)
{
	for(int i = 0; i < MAX_PARTICLES_PER_GAME; ++i)
		if(game->particles[i].alive)
		{
			Particle* particle = &game->particles[i];
			
			Pointf velocity = createPointf(0, 0);
			
			velocity.x = 
				cos(degToRad(particle->direction.value)) 
				* particle->speed.value;
				
			velocity.y = 
				-sin(degToRad(particle->direction.value)) 
				* particle->speed.value;
			
			particle->position = 
				addPointfs(particle->position, 
					scalePointf(velocity, game->delta));
					
			updateParticleAttributef(game, &particle->scale);
					
			particle->life -= game->delta;
			if(particle->life < 0)
				destroyParticle(game, particle);
		}
}

void drawParticles(Game* game)
{
	for(int i = 0; i < MAX_PARTICLES_PER_GAME; ++i)
		if(game->particles[i].alive)
		{
			Particle* particle = &game->particles[i];
			
			drawTextureExt(game, particle->texture, 
				subtractPointfs(particle->position, particle->offset),
				createPointf(particle->scale.value, particle->scale.value),
				particle->angle.value, particle->origin, false, false,
				particle->depth);
		}
}

Entity* getCollision(Game* game, Entity* entity, int type)
{
	for(int i = 0; i < MAX_ENTITIES_PER_GAME; ++i)
		if(game->entities[i].alive)
		{
			Entity* other = &game->entities[i];
			if(other != entity && other->type == type && other->collidable)
			{
				Rectanglei r1 = createRectanglei(
					entity->position.x - entity->origin.x + entity->offset.x,
					entity->position.y - entity->origin.y + entity->offset.y,
					entity->size.x, entity->size.y
				);
				
				Rectanglei r2 = createRectanglei(
					other->position.x - other->origin.x + other->offset.x,
					other->position.y - other->origin.y + other->offset.y,
					other->size.x, other->size.y
				);
				
				if(doRectangleisIntersect(r1, r2))
					return other;
			}
		}
		
	return null;
}

void initBehaviorPool(BehaviorPool* pool, int behaviorSize, 
	void (*update)(Game* game, Behavior*), void (*draw)(Game* game, Behavior*))
{
	pool->behaviorSize = behaviorSize;
	pool->update = update;
	pool->draw = draw;
}

void updateBehaviorPool(Game* game, BehaviorPool* pool)
{
	void (*updateFunction)(Game* game, Behavior*) = pool->update;
	
	if(updateFunction != null)
	{
		int count = 0;
		for(int i = 0; i < BEHAVIOR_POOL_SIZE; i += pool->behaviorSize)
		{
			if(count++ > pool->itemCount)
				break;
			Behavior* behavior = (Behavior*)(&pool->data[i]);
			if(behavior->active)
				updateFunction(game, behavior);
		}
	}
}

void drawBehaviorPool(Game* game, BehaviorPool* pool)
{
	void (*drawFunction)(Game* game, Behavior*) = pool->draw;
	
	if(drawFunction != null)
	{
		int count = 0;
		for(int i = 0; i < BEHAVIOR_POOL_SIZE; i += pool->behaviorSize)
		{
			if(count++ > pool->itemCount)
				break;
			Behavior* behavior = (Behavior*)(&pool->data[i]);
			if(behavior->active)
				drawFunction(game, behavior);
		}
	}
}

Behavior* createBehaviorFromPool(BehaviorPool* pool, Entity* entity)
{
	int initialOffset = pool->behaviorSize * pool->lastItemIndex;
	
	for(int i = initialOffset; i < BEHAVIOR_POOL_SIZE; i += pool->behaviorSize)
	{
		Behavior* behavior = (Behavior*)(&pool->data[i]);
		if(!behavior->active)
		{
			behavior->active = true;
			behavior->entity = entity;
			pool->itemCount++;
			
			return behavior;
		}
	}
	
	for(int i = 0; i < initialOffset; i += pool->behaviorSize)
	{
		Behavior* behavior = (Behavior*)(&pool->data[i]);
		if(!behavior->active)
		{
			behavior->active = true;
			behavior->entity = entity;
			pool->itemCount++;
			return behavior;
		}
	}
	
	printf("Warning: No room for behaviors left in pool!\n");
	return null;
}

void clearBehaviorPool(BehaviorPool* pool)
{
	memset((void*)pool->data, 0, BEHAVIOR_POOL_SIZE);
}

Entity* createEntity(Game* game, int type, Pointf position)
{
	for(int i = 0; i < MAX_ENTITIES_PER_GAME; ++i)
		if(!game->entities[i].alive)
		{
			Entity* entity = &game->entities[i];
			
			Entity blank = { 0 };
			*entity = blank;
			
			entity->alive = true;
			entity->type = type;
			entity->position = position;
			entity->size = createPointi(16, 16);
			entity->origin = createPointi(8, 8);
			entity->color = createColor(255, 255, 0, 255);
			entity->drawRectangle = true;
			entity->collidable = true;
			
			return entity;
		}
		
	printf("Error: Ran out of room for entities");
		
	return null;
}

void updateBehaviorPools(Game* game, BehaviorPools* behaviorPools)
{
	BehaviorPool* pool = (BehaviorPool*) (&behaviorPools->dataStart + 1);
	
	int count = (sizeof(BehaviorPools) - sizeof(int)) / sizeof(BehaviorPool);
	for(int i = 0; i < count; i++)
		updateBehaviorPool(game, &(pool[i]));
}

void drawBehaviorPools(Game* game, BehaviorPools* behaviorPools)
{
	BehaviorPool* pool = (BehaviorPool*) (&behaviorPools->dataStart + 1);
	
	int count = (sizeof(BehaviorPools) - sizeof(int)) / sizeof(BehaviorPool);
	for(int i = 0; i < count; i++)
		drawBehaviorPool(game, &(pool[i]));
}

void clearBehaviorPools(BehaviorPools* behaviorPools)
{;
	BehaviorPool* pool = (BehaviorPool*) (&behaviorPools->dataStart + 1);
	
	int count = (sizeof(BehaviorPools) - sizeof(int)) / sizeof(BehaviorPool);
	for(int i = 0; i < count; i++)
		clearBehaviorPool(&(pool[i]));
}

void destroyBehaviors(Behaviors* behaviors)
{
	EmptyBehavior** behavior = (EmptyBehavior**) (&behaviors->dataStart + 1);
	
	int count = (sizeof(BehaviorPools) - sizeof(int)) / sizeof(BehaviorPool);
	for(int i = 0; i < count; i++)
		if(behavior[i])
			behavior[i]->base.active = false;
}

#include "behaviors.c"
#include "generated.c"

void clearAllEntities(Game* game)
{
	memset((void*)game->entities, 0, sizeof(Entity) * MAX_ENTITIES_PER_GAME);
	clearBehaviorPools(&game->behaviorPools);
}

void destroyEntity(Entity* entity)
{
	entity->alive = false;
	destroyBehaviors(&(entity->behaviors));
}

#include "entities.c"

void updateEntity(Game* game, Entity* entity)
{
	if(entity->sprite.alive)
	{
		updateSprite(game, &entity->sprite);
		entity->sprite.position = getEntityDrawPosition(entity);
	}
	
	entity->position = 
		addPointfs(
			entity->position, 
			scalePointf(entity->velocity, game->delta));
}

void drawEntity(Game* game, Entity* entity)
{
	if(entity->texture)
	{
		Pointf location = createPointf(
			entity->position.x - entity->origin.x, 
			entity->position.y - entity->origin.y);
			
		drawTexture(game, entity->texture, location, entity->depth);
	}
	
	if(entity->drawRectangle)
	{
		Rectanglei rectangle = createRectanglei(
			(int)entity->position.x - entity->origin.x + entity->offset.x,
			(int)entity->position.y - entity->origin.y + entity->offset.y,
			entity->size.x,
			entity->size.y
		);
		
		drawRectangle(game, rectangle, entity->color);
	}
	
	if(entity->sprite.alive)
	{
		drawSprite(game, &entity->sprite);
	}
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

void updateBackground(Game* game)
{
	Background* background = &game->background;
	background->position = 
		addPointfs(background->position, 
			scalePointf(background->velocity, game->delta));
}

void drawBackground(Game* game)
{
	Background* background = &game->background;
	
	if(background->repeatY)
	{
		int from, to;
		from = ((int)background->position.y % game->internalResolution.y)
			- background->texture->height;
		for(int i = from; 
		    i < game->internalResolution.y; 
		    i += background->texture->height)
		{
			drawTexture(game, background->texture,
				createPointf(background->position.x, i), background->depth);
		}
	}
	else 
		drawTexture(game, background->texture, background->position, background->depth);
}

int defineFont(Game* game, Texture* texture, Pointi characterSize, char* characterOrder)
{
	
	game->font.texture = texture;
	game->font.characterSize = characterSize;
	
	for(int i = 0; i < 255; ++i)
		game->font.characterMap[i] = -1;
	
	for(int i = 0; i < strlen(characterOrder); ++i)
		game->font.characterMap[characterOrder[i]] = i;
	
	return 0; // TODO: Implement multiple fonts and return the font's ID
}

void drawText(Game* game, int fontId, char* string, Pointf position, int depth)
{
	Pointi offset = {0};
	Font* font = &game->font; // TODO: Implement multiple fonts
	
	for(int i = 0; i < strlen(string); ++i)
	{
		if(font->characterMap[string[i]] >= 0)
		{
			drawTile(
				game, font->texture, 
				createPointf(position.x + offset.x, position.y + offset.y), 
				font->characterSize, 
				font->characterMap[string[i]], depth);
		}
		
		offset.x += font->characterSize.x;
		
		if(string[i] == '\n')
		{
			offset.x = 0;
			offset.y += font->characterSize.y;
		}
	}
}

void intToFixedSizeString(int value, int digits, char* dest)
{
	dest[digits] = 0;
	for(int i = 0; i < digits; ++i)
		dest[digits - i - 1] = '0' + ((value / (int)pow((double)10, (double)i))%10);
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
