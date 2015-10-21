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
