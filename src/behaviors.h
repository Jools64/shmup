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
	
	int something;
} PlayerBehavior;

typedef struct BulletBehavior
{
	Behavior base;
	
	int health;
	float lifeTimer;
} BulletBehavior;

typedef struct EnemyAiBehavior
{
	Behavior base;
	
	int movementPattern;
} EnemyAiBehavior;
