#pragma once
#include "Enemy.h"
class StrongEnemy :public Enemy
{
public:
	StrongEnemy();
	~StrongEnemy();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion);
	void End();
	void Update() override;
	void Draw() override;
	virtual void OnAttack() override;
	void OnRangeAttack();
	void OnDamage();
	float GetColRadius();
	virtual VECTOR GetDir() override;
	int GetStrongEnemyAttackPower();
private:
	enum StrongEnemyState
	{
		DEFAULT,
		NORMALATTACK,
		RANGEATTACK_CHARGE,
		RANGEATTACK,
		COOLDOWN
	};
	StrongEnemyState m_state;
	AttackSphere m_enemyAttack;
	float m_alpha;
	float m_targetAngle;
	float m_attackTimer;
	float m_actionCheckTimer; // 行動抽選のクールダウンタイマー
};

