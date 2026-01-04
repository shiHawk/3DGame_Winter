#pragma once
#include "DxLib.h"
#include <memory>
#include <vector>
#include "Player.h"
#include "Companion.h"
#include "NormalEnemy.h"
#include "StrongEnemy.h"
#include "FlyingEnemy.h"
#include "BossEnemy.h"
class UIManager
{
public:
	UIManager();
	~UIManager();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion);
	void End();
	void Updata();
	void Draw();
private:
	void DrawHp();
	void DrawSg();
	float m_playerHpGaugeRate;
	float m_companionHpGaugeRate;
	int m_hpGaugeFrameHandle;
	int m_hpGaugeHandle;
	std::shared_ptr<Player> m_pPlayer;
	std::vector<std::shared_ptr<NormalEnemy>> m_pNormalEnemies;
	std::vector<std::shared_ptr<StrongEnemy>> m_pStrongEnemies;
	std::shared_ptr<NormalEnemy> m_pNormalEnemy;
	std::shared_ptr<Companion> m_pCompanion;
	std::shared_ptr<FlyingEnemy> m_pFlyingEnemy;
	std::shared_ptr<StrongEnemy> m_pStrongEnemy;
	std::shared_ptr<BossEnemy> m_pBossEnemy;

};

