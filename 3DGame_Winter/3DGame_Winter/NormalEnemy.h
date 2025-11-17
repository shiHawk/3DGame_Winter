#pragma once
#include "Enemy.h"
#include "Player.h"
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
private:
	AttackSphere m_enemyAttack;
	float m_alpha;
	float m_targetAngle;
};

