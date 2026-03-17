#pragma once
#include "Enemy.h"
#include "Player.h"
#include "Companion.h"
#include "DxLib.h"
class NormalEnemy :public Enemy
{
public:
	NormalEnemy();
	~NormalEnemy();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion,VECTOR pos, float startAngle,int hp);
	void End();
	void Update() override;
	void Draw() override;
	virtual void OnAttack() override;
	virtual void OnDamage(int damage, bool isHatePlayer) override;
	float GetColRadius();
	virtual VECTOR GetDir() override;
	void ApplyKnockback(VECTOR direction, float power);
	void SetIsKnockbackFlag(bool flag) { m_isKnockbackFlag = flag; }
	bool IsAttackFlag() { return m_enemyAttack.active; }
	float GetAttackRadius() { return m_enemyAttack.radius; }
	int GetAttackPower() { return m_attackPower; }
	AttackSphere GetAttackInfo() { return m_enemyAttack; }
	int GetMaxHp();
private:
	AttackSphere m_enemyAttack;
	float m_alpha;
	float m_targetAngle;
	float m_knockbackTimer;
	float m_recoveryTimer; // 復帰待機タイマー
	VECTOR m_knockbackDir;
	bool m_isKnockbackFlag;
};

