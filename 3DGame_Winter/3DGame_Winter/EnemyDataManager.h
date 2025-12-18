#pragma once
#include "DxLib.h"
#include "NormalEnemy.h"
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
	void LoadEnemyData(const std::string fileName);
private:
	std::shared_ptr<NormalEnemy> m_pNormalEnemy;
};

