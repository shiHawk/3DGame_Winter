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
private:
	AttackSphere m_enemyAttack;
};

