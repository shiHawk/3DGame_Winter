#pragma once
#include "Enemy.h"
class BossEnemy :public Enemy
{
public:
	BossEnemy();
	~BossEnemy();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion);
	void End();
	void Update() override;
	void Draw() override;
	virtual void OnDamage()override;
	float GetColRadius();
	virtual VECTOR GetDir() override;
	int GetBossEnemyAttackPower();
private:
	virtual void OnAttack() override;
	void OnStrongAttack();
	void OnRangeAttack();
	void UpdateDefault();
	void UpdateMove();
	enum BossEnemyState
	{
		DEFAULT,
		MOVE,           // 追跡
		NORMAL_ATTACK,  // 通常攻撃（素早い）
		STRONG_ATTACK_CHARGE, // 強攻撃の溜め（予兆表示）
		STRONG_ATTACK,        // 強攻撃（直線または扇形）
		RANGE_ATTACK_CHARGE,  // 範囲攻撃の溜め（予兆表示）
		RANGE_ATTACK,         // 範囲攻撃（周囲円形）
		COOLDOWN,       // 攻撃後の隙
		DAMAGE,         // 怯み
		DEAD            // 死亡
	};
	BossEnemyState m_state;
	AttackSphere m_enemyAttack;
	float m_alpha;
	float m_targetAngle;
	float m_attackTimer;
	float m_actionCheckTimer; // 行動抽選のクールダウンタイマー
	// 強攻撃の直線範囲（AABB）
	VECTOR m_strongAttackPos1;
	VECTOR m_strongAttackPos2;
};
