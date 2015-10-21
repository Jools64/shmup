Entity* createPlayer(Game* game, Pointf position)
{
	Entity* entity = createEntity(game, PLAYER, position);
	
	attachSpriteToEntity(entity, 
		&game->assets.textures.player, createPointi(16, 16));
	entity->size = createPointi(8, 8);
	entity->offset = createPointi(4, 4);
	entity->drawRectangle = false;
	
	MobBehavior* mb = 
		(MobBehavior*) createBehaviorFromPool(&game->behaviorPools.mob, entity);
	mb->shootDelay = 0.1;
	mb->shootTimer = 0;
	mb->moveSpeed = 120;
	entity->behaviors.mob = mb;
	
	PlayerBehavior* pb = 
	(PlayerBehavior*) createBehaviorFromPool(
		&game->behaviorPools.player, entity);
	entity->behaviors.player = pb;
	
	return entity;
}

Entity* createEnemy(Game* game, Pointf position)
{
	Entity* entity = createEntity(game, ENEMY, position);
	
	attachSpriteToEntity(entity, 
		&game->assets.textures.crab, createPointi(32, 32));
		
	int frames[4] = {0, 1, 2, 3};
	int animation = defineSpriteAnimation(&entity->sprite, frames, 4, 12);
	playSpriteAnimation(&entity->sprite, animation);
		
	entity->color = createColor(200, 0, 80, 255);
	entity->velocity = createPointf(0, 2);
	entity->size = createPointi(24, 24);
	entity->offset = createPointi(4, 4);
	entity->origin = createPointi(16, 16);
	entity->drawRectangle = false;
	
	MobBehavior* mb = 
		(MobBehavior*) createBehaviorFromPool(
			&game->behaviorPools.mob, entity);
	mb->shootDelay = 0.1;
	mb->moveSpeed = 120;
	mb->health = mb->maxHealth = 3;
	mb->scoreReward = 152;
	mb->deathAnimation = true;
	entity->behaviors.mob = mb;
	
	EnemyAiBehavior* eab =
		(EnemyAiBehavior*) createBehaviorFromPool(
				&game->behaviorPools.enemyAi, entity);
	eab->movementPattern = 0;
	entity->behaviors.enemyAi = eab;
	
	return entity;
}

Entity* createPlayerBullet(Game* game, Pointf position)
{
	Entity* entity = createEntity(game, BULLET, position);
	
	entity->color = createColor(255, 180, 0, 255);
	entity->velocity = createPointf(randomBetweenf(-30, 30), -600);
	entity->size = createPointi(8, 16);
	entity->origin = createPointi(4, 8);
	entity->timer = 1.0f;
	entity->texture = &game->assets.textures.playerBullet;
	entity->drawRectangle = false;
	
	BulletBehavior* bb = 
		(BulletBehavior*) createBehaviorFromPool(
			&game->behaviorPools.bullet, entity);
	bb->lifeTimer = 1.0f;
	bb->health = 1;
	entity->behaviors.bullet = bb;
	
	return entity;
}