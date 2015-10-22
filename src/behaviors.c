// -----------------------------------------------------------------------------
// Misc Functions
// -----------------------------------------------------------------------------

void destroyEntity(Entity* entity);
void loadLevel(Game* game);

void hurtMob(Entity* entity, int damage)
{
	if(entity->behaviors.mob)
	{
		if(!entity->behaviors.mob->invincible)
			entity->behaviors.mob->health -= damage;
	}
}

// -----------------------------------------------------------------------------
// Bullet
// -----------------------------------------------------------------------------

defineBehavior(bullet, BulletBehavior, updateBulletBehavior, null);

void updateBulletBehavior(Game* game, Behavior* behavior)
{
	BulletBehavior* b = (BulletBehavior*)behavior;
	Entity* entity = b->base.entity;
	
	b->lifeTimer -= game->delta;
	if(b->lifeTimer < 0)
		destroyEntity(entity);
		
	Entity* other = getCollision(game, entity, ENEMY);
	if(other)
	{
		if(--b->health < 1)
			destroyEntity(entity);
		hurtMob(other, 1);
	}
}

Entity* createPlayerBullet(Game* game, Pointf position);

// -----------------------------------------------------------------------------
// Mob
// -----------------------------------------------------------------------------

defineBehavior(mob, MobBehavior, updateMobBehavior, null);

void updateMobBehavior(Game* game, Behavior* behavior)
{
	MobBehavior* b = (MobBehavior*)behavior;
	Entity* entity = b->base.entity;
	
	entity->sprite.depth = -entity->position.y;

	if(b->shootTimer > 0)
		b->shootTimer -= game->delta;
		
	if(b->shoot && b->shootTimer <= 0)
	{
		b->shootTimer = b->shootDelay;
		Entity* bullet = createPlayerBullet(game, b->base.entity->position);
	}
	
	//game->camera.position = entity->position;
	
	if(!b->dead)
	{
		entity->velocity.y = 
			(float)(b->moveDown - b->moveUp)
			* b->moveSpeed;
			
		entity->velocity.x = 
			(float)(b->moveRight - b->moveLeft)
			* b->moveSpeed;
		
		Entity* other = getCollision(game, entity, ENEMY);
		
		//if(other)
			//hurtEntity(entity, 1);
	}
	else
		entity->velocity = createPointf(0, 0);
		
	if(b->health <= 0)
	{
		bool deathAnimation = b->deathAnimation;
		
		if(!b->invincible)
		{
			game->state.score += b->scoreReward;
			entity->sprite.scale.x += 1.0f;
			entity->sprite.scale.y += 1.0f;
			entity->collidable = false;
			b->dead = true;
			
			if(entity->type == BULLET)
			{
				Particle* particle = createParticle(game, 
					&game->assets.textures.deathParticle, entity->position);
				particle->scale.value = 0.5;
				particle->scale.delta = -2;
				particle->life = 1.0f/4;
			}
			else if(entity->type == ENEMY || entity->type == PLAYER)
			{
				for(int i = 0; i < 8; ++i)
				{
					Particle* particle = createParticle(game, 
						&game->assets.textures.deathParticle, entity->position);
					particle->scale.delta = -2;
					particle->life = 0.5;
					particle->direction.value = i * 45;
					particle->speed.value = 120;
				}
			}
		}
		
		if(deathAnimation)
		{
			b->invincible = true;
			entity->sprite.scale.x -= 8 * game->delta;
			entity->sprite.scale.y -= 8 * game->delta;
			if(entity->sprite.scale.y < 0)
				destroyEntity(entity);
		}
		else
			destroyEntity(entity);
	}
	
	if(b->health > b->maxHealth)
		b->health = b->maxHealth;
}

// -----------------------------------------------------------------------------
// Player
// -----------------------------------------------------------------------------

defineBehavior(player, PlayerBehavior, updatePlayerBehavior, null);

void updatePlayerBehavior(Game* game, Behavior* behavior)
{
	PlayerBehavior* b = (PlayerBehavior*)behavior;
	Entity* entity = b->base.entity;
	
	entity->behaviors.mob->moveLeft = checkKey(game, SDLK_LEFT);
	entity->behaviors.mob->moveRight = checkKey(game, SDLK_RIGHT);
	entity->behaviors.mob->moveUp = checkKey(game, SDLK_UP);
	entity->behaviors.mob->moveDown = checkKey(game, SDLK_DOWN);
	entity->behaviors.mob->shoot = checkKey(game, SDLK_z);
	
	if(getCollision(game, entity, ENEMY))
		hurtMob(entity, 1);

	if(!entity->behaviors.mob->dead)
	{
		entity->sprite.currentFrame = 0;
		if(entity->velocity.x > 0)
			entity->sprite.currentFrame = 2;
		if(entity->velocity.x < 0)
			entity->sprite.currentFrame = 1;
			
		Particle* particle = createParticle(game, 
			&game->assets.textures.deathParticle, 
			addPointfs(entity->position, createPointf(1, 8)));
		particle->scale.value = 0.3;
		particle->scale.delta = -2;
		particle->life = 0.2;
		particle->direction.value = 270;
		particle->speed.value = 180;
	}
}

// -----------------------------------------------------------------------------
// Enemy AI
// -----------------------------------------------------------------------------

defineBehavior(enemyAi, EnemyAiBehavior,updateEnemyAiBehavior, null);

void updateEnemyAiBehavior(Game* game, Behavior* behavior)
{
	EnemyAiBehavior* b = (EnemyAiBehavior*)behavior;
	Entity* entity = b->base.entity;
    
	// This is all a great big hacky mess to get things moving on the screen
	// because moving things are more exciting than things that are not moving.

	b->movementTimer -= game->delta;

	if(entity->position.y > game->internalResolution.y+16)
		entity->position.y = -16;

	if(b->movementTimer < 0)
	{
		b->movementTimer = 1 + randomf(3);
		b->movementPattern = randomBetweeni(0, 3);
		b->target = createPointf(
			randomBetweenf(32, game->internalResolution.x - 32), 
			randomBetweenf(32, game->internalResolution.y - 64));

		entity->velocity = createPointf(0.0f, 0.0f);

		if(b->movementPattern == MOVEMENT_CHARGE)
		{
			if(randomi(2) == 1)
				b->movementTimer = 0.0f;
			else
				b->movementTimer = 
					(game->internalResolution.y + 32) / (b->speed * 2);
		}
		if(b->movementPattern == MOVEMENT_CIRCLE)
			b->movementTimer = (float)ceil(b->movementTimer);
	}

    switch((int)b->movementPattern)
    {
    	case MOVEMENT_WAIT: {
    		entity->velocity = 
    			createPointf(
    				0.0f, 
    				-sin(degToRad(b->movementTimer * 360.0f)) * b->speed * 0.2);
    		} break;

    	case MOVEMENT_TARGET: {
    		entity->velocity = 
    			scalePointf(
					normalizePointf(
						subtractPointfs(b->target, entity->position)
					), b->speed);

    		if(distanceBetweenPointfs(b->target, entity->position) 
    			< b->speed * game->delta)
				b->movementTimer = 0;
    		} break;

    	case MOVEMENT_CIRCLE: {
			float circleSpeed = 0.5f;

    		entity->velocity = createPointf(
				cos(degToRad(b->movementTimer * 360.0f * circleSpeed)) 
					* b->speed * 2,
				-sin(degToRad(b->movementTimer * 360.0f * circleSpeed)) 
					* b->speed * 2);
    		} break;

    	case MOVEMENT_CHARGE: {
    		entity->velocity = createPointf(0, b->speed * 2);
    		} break;
    }
}
