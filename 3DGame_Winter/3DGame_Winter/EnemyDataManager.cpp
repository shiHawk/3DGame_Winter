#include "EnemyDataManager.h"

EnemyDataManager::EnemyDataManager()
{
}

void EnemyDataManager::LoadEnemyData(const std::string fileName, 
									 std::vector<std::shared_ptr<NormalEnemy>>& pNormalEnemies, 
									 std::vector<std::shared_ptr<StrongEnemy>>& pStrongEnemies,
									 std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<EffectManager> pEffectManager)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		printfDx(L"CSVファイルが開けませんでした:%s\n", fileName.c_str());
	}
	std::string line;
	std::getline(file, line); // 最初の行を読み飛ばす
	while (std::getline(file, line))
	{
		std::istringstream stream(line); // 一行分をストリームに変換する
		std::string type, strX, strY, strZ, strRotY,hp;

		if (!std::getline(stream, type, ',')) continue; // 敵の種類
		if (!std::getline(stream, strX, ',')) continue;	// x座標
		if (!std::getline(stream, strY, ',')) continue;	// y座標
		if (!std::getline(stream, strZ, ',')) continue;	// z座標
		if (!std::getline(stream, strRotY, ',')) continue; // 向きを読み込む 
		if (!std::getline(stream, hp, ',')) continue; // HPを読み込む

		// 文字列をfloatに変換する
		float x = std::stof(strX);
		float y = std::stof(strY);
		float z = std::stof(strZ);
		float rotY = std::stof(strRotY);
		int enemyHp = std::stoi(hp);
		VECTOR enemyPos = VGet(x, y, z); // VECTOR型に変換する
		float angleRad = rotY;

		if (type == "normalEnemy")
		{
			auto normalEnemy = std::make_shared<NormalEnemy>();
			normalEnemy->Init(pPlayer,pCompanion,enemyPos,angleRad,enemyHp);
			pNormalEnemies.push_back(normalEnemy);
		}
		if (type == "strongEnemy")
		{
			auto strongEnemy = std::make_shared<StrongEnemy>();
			strongEnemy->Init(pPlayer, pCompanion,pEffectManager,enemyPos,enemyHp);
			pStrongEnemies.push_back(strongEnemy);
		}
	}
}