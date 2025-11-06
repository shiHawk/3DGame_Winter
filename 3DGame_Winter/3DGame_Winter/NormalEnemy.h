#pragma once
#include "Enemy.h"
#include "Player.h"
#include "DxLib.h"
class NormalEnemy :public Enemy
{
public:
	NormalEnemy();
	~NormalEnemy();
	void Init(std::shared_ptr<Player> pPlayer);
	void End();
	void Update() override;
	void Draw() override;
private:
};

