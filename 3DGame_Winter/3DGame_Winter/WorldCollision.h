#pragma once
#include "DxLib.h"
#include "CharacterBase.h"
#include <memory>
#include <vector>
class Player;
class Companion;
class Stage;
class NormalEnemy;
class StrongEnemy;
class WorldCollision
{
public:
	WorldCollision();
	~WorldCollision();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Stage> pStage, std::shared_ptr<Companion> pCompanion,
		std::vector<std::shared_ptr<NormalEnemy>> pNormalEnemies, std::vector<std::shared_ptr<StrongEnemy>> pStrongEnemies);
	void End();
	void Update();
	void Draw();
private:
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Stage> m_pStage;
	std::shared_ptr<Companion> m_pCompanion;
	std::vector<std::shared_ptr<NormalEnemy>> m_pNormalEnemies;
	std::vector<std::shared_ptr<StrongEnemy>> m_pStrongEnemies;
	void CheckGroundCollision(CharacterBase* pTargetCharacter);
	void CheckWallCollision(CharacterBase* pTargetCharacter);
	void MoveCharacter(CharacterBase* pTargetCharacter);
	float m_lastGroundY;
};

