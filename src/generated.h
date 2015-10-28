typedef struct __attribute__((__packed__)) Behaviors
{
	int dataStart;
	BulletBehavior* bullet;
	MobBehavior* mob;
	PlayerBehavior* player;
	EnemyAiBehavior* enemyAi;
	ScoreMessageBehavior* scoreMessage;
} Behaviors;

typedef struct __attribute__((__packed__)) BehaviorPools
{
	int dataStart;
	BehaviorPool bullet;
	BehaviorPool mob;
	BehaviorPool player;
	BehaviorPool enemyAi;
	BehaviorPool scoreMessage;
} BehaviorPools;

