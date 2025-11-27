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
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion);
	void End();
	void Update() override;
	void Draw() override;
	virtual void OnAttack() override;
	void OnDamage();
	float GetColRadius();
	virtual VECTOR GetDir() override;
	void ApplyKnockback(VECTOR direction, float power);
	void SetIsKnockbackFlag(bool flag) { m_isKnockbackFlag = flag; }
private:
	AttackSphere m_enemyAttack;
	float m_alpha;
	float m_targetAngle;
	float m_knockbackTimer;
	VECTOR m_knockbackDir;
	bool m_isKnockbackFlag;
};

