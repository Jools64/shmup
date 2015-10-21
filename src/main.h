#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#pragma GCC diagnostic ignored "-Wwrite-strings"

#undef main

#define byte unsigned char
#define bool byte
#define true 1
#define false 0
#define null 0

#define PI 3.141592
#define MAX_ENTITIES_PER_GAME 512
#define BEHAVIOR_POOL_SIZE 1024 * 256
#define MAX_PARTICLES_PER_GAME 1024
#define MAX_KEYS 255
#define MAX_FRAMES_PER_ANIMATION 16
#define MAX_ANIMATIONS_PER_SPRITE 8
#define MAX_CHARACTERS_PER_FONT 255
#define MAX_RENDER_ITEMS_PER_GAME 2048

typedef struct Pointf
{
	float x, y;
} Pointf;

typedef struct Pointi
{
	int x, y;
} Pointi;

typedef struct Color
{
	byte r, g, b, a;
} Color;

typedef struct Rectanglei
{
	int x, y, width, height;
} Rectanglei;

typedef struct Rectanglef
{
	float x, y, width, height;
} Rectanglef;

typedef struct Texture
{
	SDL_Texture* data;
	int width, height;
} Texture;

typedef struct Animation
{
	int id, frameCount;
	int frames[MAX_FRAMES_PER_ANIMATION];
	float frameRate;
} Animation;

typedef struct Sprite
{
	Texture* texture;
	Pointi size, origin;
	Pointf position, scale;
	float frameTimer, frameRate, angle;
	Animation animations[MAX_ANIMATIONS_PER_SPRITE];
	int currentAnimation, animationCount, currentFrame, depth;
	bool alive, animationDone, loopAnimation, verticalFlip, horizontalFlip;
} Sprite;

typedef struct Entity Entity;
typedef struct Game Game;

typedef struct Behavior
{
	Entity* entity;
	bool active;
} Behavior;

typedef struct EmptyBehavior
{
	Behavior base;
} EmptyBehavior;

typedef struct BehaviorPool
{
	char data[BEHAVIOR_POOL_SIZE];
	int lastItemIndex, behaviorSize, itemCount;
	void (*update)(Game* game, Behavior*);
	void (*draw)(Game* game, Behavior*);
} BehaviorPool;

#include "behaviors.h"
#include "generated.h"

typedef struct Entity
{
	Pointi size, offset, origin;
	Pointf position, velocity;
	Color color;
	bool alive, drawRectangle, collidable;
	int type, depth;
	float timer;
	Texture* texture;
	Sprite sprite;
	
	Behaviors behaviors;
} Entity;

typedef struct ParticleAttributef
{
	float value, delta, acceleration;
} ParticleAttributef;

typedef struct Particle
{
	Pointf position, offset;
	Pointi origin;
	Texture* texture;
	ParticleAttributef speed, direction, angle, alpha, scale;
	float life;
	int depth;
	bool alive;
} Particle;

typedef struct Background
{
	Texture* texture;
	Pointf position, velocity;
	bool repeatX, repeatY;
	int depth;
} Background;

typedef struct Font
{
	Texture* texture;
	Pointi characterSize;
	int characterMap[MAX_CHARACTERS_PER_FONT];
	//int characterMapSize;
} Font;

typedef struct Textures
{
	Texture player;
	Texture crab;
	Texture playerBullet;
	Texture background;
	Texture font;
	Texture deathParticle;
} Textures;

typedef struct Assets
{
	Textures textures;
} Assets;

typedef struct State
{
	int score, lives, stage, level;
} State;

typedef struct RenderItem
{
	int depth, type;
	
	Texture* texture; 
	Pointf position, scale; 
	Pointi origin, sourcePosition, sourceSize;
	float angle;
	bool horizontalFlip, verticalFlip;
} RenderItem;

typedef struct Camera
{
	Pointf position;
	Pointi size;
} Camera;

typedef struct Game
{
	Assets assets;
	SDL_Window* window;
	SDL_Renderer* renderer;
	Pointi internalResolution;
	float windowScale;
	bool running;
	float delta;
	Color clearColor;
	Entity entities[MAX_ENTITIES_PER_GAME];
	Particle particles[MAX_PARTICLES_PER_GAME];
	int lastParticleIndex, particleCount;
	int keys[MAX_KEYS], keysPressed[MAX_KEYS], keysReleased[MAX_KEYS];
	Background background;
	Font font;
	State state;
	RenderItem renderItems[MAX_RENDER_ITEMS_PER_GAME];
	int renderItemCount;
	float frameTimer;
	int frameCounter, fps;
	Camera camera;
	
	BehaviorPools behaviorPools;
	//BehaviorPool mobBehaviors;
} Game;

typedef enum EntityType
{
	PLAYER,
	ENEMY,
	POWER_UP,
	BULLET
} EntityType;
