typedef struct MobBehavior
{
	Behavior base;
	
	float shootTimer, shootDelay, moveSpeed;
	int health, maxHealth, scoreReward;
	bool invincible, deathAnimation, dead,
		moveLeft, moveRight, moveUp, moveDown, shoot;
} MobBehavior;

typedef struct PlayerBehavior
{
	Behavior base;
	
	float deathTimer;
} PlayerBehavior;

typedef struct BulletBehavior
{
	Behavior base;
	
	int health;
	float lifeTimer;
} BulletBehavior;

typedef enum MovementPattern
{
    MOVEMENT_TARGET,
    MOVEMENT_CIRCLE,
    MOVEMENT_CHARGE,
    MOVEMENT_WAIT
} MovementPattern;

typedef struct EnemyAiBehavior
{
	Behavior base;
	
	MovementPattern movementPattern;
	float movementTimer, speed;
	Pointf target;
} EnemyAiBehavior;
