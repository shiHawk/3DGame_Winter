#pragma once
#include "Enemy.h"
class EffectManager;
class BossEnemy :public Enemy
{
public:
	BossEnemy();
	~BossEnemy();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<EffectManager> pEffectManager);
	void End();
	void Update() override;
	void Draw() override;
	virtual void OnDamage(int damage, bool isHatePlayer)override;
	float GetColRadius();
	virtual VECTOR GetDir() override { return m_enemyAttack.dir; };
	AttackSphere GetAttackInfo() { return m_enemyAttack; }
	int GetAttackPower() { return m_attackPower; }
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
		STRONG_ATTACK,        // 強攻撃
		RANGE_ATTACK_CHARGE,  // 範囲攻撃の溜め（予兆表示）
		RANGE_ATTACK,         // 範囲攻撃（周囲円形）
		COOLDOWN,       // 攻撃後の隙
		DAMAGE,         // 被弾
		DEAD            // 死亡
	};
	BossEnemyState m_state;
	AttackSphere m_enemyAttack;
	float m_alpha;
	float m_targetAngle;
	float m_attackTimer;
	float m_actionCheckTimer; // 行動抽選のクールダウンタイマー
	VECTOR m_storngAttackTargetPos;
	std::shared_ptr<CharacterBase> m_lockedTarget;
	std::shared_ptr<EffectManager> m_pEffectManager;
};
