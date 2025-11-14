#pragma once
#include "DxLib.h"
#include <memory>
#include <vector>
class Player;
class Stage;
class WorldCollision
{
public:
	WorldCollision();
	~WorldCollision();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Stage> pStage);
	void End();
	void Update();
	void Draw();
private:
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Stage> m_pStage;
	void CheckGroundCollision();
	float m_lastGroundY;
};

