#pragma once
#include "DxLib.h"
#include "CharacterBase.h"
#include <memory>
#include <vector>
class Player;
class Companion;
class Stage;
class WorldCollision
{
public:
	WorldCollision();
	~WorldCollision();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Stage> pStage, std::shared_ptr<Companion> pCompanion);
	void End();
	void Update();
	void Draw();
private:
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Stage> m_pStage;
	std::shared_ptr<Companion> m_pCompanion;
	void CheckGroundCollision(CharacterBase* pTargetCharacter);
	float m_lastGroundY;
};

