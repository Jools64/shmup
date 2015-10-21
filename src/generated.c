void initBehaviorPools(BehaviorPools* behaviorPools)
{
	initBehaviorPool(&behaviorPools->bullet, 
		sizeof(BulletBehavior), updateBulletBehavior, (null));

	initBehaviorPool(&behaviorPools->mob, 
		sizeof(MobBehavior), updateMobBehavior, (null));

	initBehaviorPool(&behaviorPools->player, 
		sizeof(PlayerBehavior), updatePlayerBehavior, (null));

	initBehaviorPool(&behaviorPools->enemyAi, 
		sizeof(EnemyAiBehavior), updateEnemyAiBehavior, (null));
}