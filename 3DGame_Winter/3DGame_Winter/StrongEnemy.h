#pragma once
#include "Enemy.h"
class EffectManager;
class StrongEnemy :public Enemy
{
public:
	StrongEnemy();
	~StrongEnemy();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<EffectManager> pEffectManager,VECTOR pos);
	void End();
	void Update() override;
	void Draw() override;
	virtual void OnAttack() override;
	void OnRangeAttack();
	virtual void OnDamage(int damage, bool isHatePlayer)override;
	float GetColRadius();
	virtual VECTOR GetDir() override;
	AttackSphere GetAttackInfo() { return m_enemyAttack; }
	int GetAttackPower() { return m_attackPower; }
	int GetAttackPos();
	int GetMaxHp();
private:
	void UpdateState();
	enum StrongEnemyState
	{
		DEFAULT,
		NORMALATTACK,
		RANGEATTACK_CHARGE,
		RANGEATTACK,
		COOLDOWN,
		DEAD
	};
	std::shared_ptr<EffectManager> m_pEffectManager;
	StrongEnemyState m_state;
	AttackSphere m_enemyAttack;
	float m_alpha;
	float m_targetAngle;
	float m_attackTimer;
	float m_actionCheckTimer; // 行動抽選のクールダウンタイマー
};

