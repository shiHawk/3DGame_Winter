#pragma once
#include "DxLib.h"
#include "NormalEnemy.h"
#include "StrongEnemy.h"
#include "Player.h"
#include "Companion.h"
#include "EffectManager.h"
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
class EnemyDataManager
{
public:
	EnemyDataManager();
	~EnemyDataManager() {};
	void LoadEnemyData(const std::string fileName,
					   std::vector<std::shared_ptr<NormalEnemy>>& pNormalEnemies,
					   std::vector<std::shared_ptr<StrongEnemy>>& pStrongEnemies,
					   std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<EffectManager> pEffectManager);
private:
	//std::shared_ptr<NormalEnemy> m_pNormalEnemy;
	std::vector<std::shared_ptr<NormalEnemy>> m_pNormalEnemies;
	std::vector<std::shared_ptr<StrongEnemy>> m_pStrongEnemies;
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Companion> m_pCompanion;
	std::shared_ptr<EffectManager> m_pEffectManager;
};

