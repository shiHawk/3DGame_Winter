#pragma once
#include "Enemy.h"
class BossEnemy :public Enemy
{
public:
	BossEnemy();
	~BossEnemy();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, VECTOR pos);
	void End();
	void Update() override;
	void Draw() override;
	virtual void OnAttack() override;
	void OnRangeAttack();
	virtual void OnDamage()override;
	float GetColRadius();
	virtual VECTOR GetDir() override;
	int GetStrongEnemyAttackPower();
private:
	enum BossEnemyState
	{
		DEFAULT,
		NORMALATTACK,
		RANGEATTACK_CHARGE,
		RANGEATTACK,
		COOLDOWN
	};
	BossEnemyState m_state;
	AttackSphere m_enemyAttack;
	float m_alpha;
	float m_targetAngle;
	float m_attackTimer;
	float m_actionCheckTimer; // 行動抽選のクールダウンタイマー
};

