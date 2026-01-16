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
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion,std::shared_ptr<BossEnemy> pBoss);
	void End();
	void Updata();
	void Draw();
private:
	void DrawHp();
	void DrawSg();
	void DrawBossHp();
	float m_playerHpGaugeRate;
	float m_companionHpGaugeRate;
	float m_playerSpecialGaugeRate;
	float m_companionSpecialGaugeRate;
	float m_bossHpGaugeRate;
	int m_hpGaugeFrameHandle;
	int m_hpGaugeHandle;
	int m_sgGaugeHandle;
	int m_bossHpGaugeHandle;
	int m_bossHPGaugeFlameHandle;
	int m_warriorIconHandle;
	int m_wizardIconHandle;
	std::shared_ptr<Player> m_pPlayer;
	std::vector<std::shared_ptr<NormalEnemy>> m_pNormalEnemies;
	std::vector<std::shared_ptr<StrongEnemy>> m_pStrongEnemies;
	std::shared_ptr<NormalEnemy> m_pNormalEnemy;
	std::shared_ptr<Companion> m_pCompanion;
	std::shared_ptr<FlyingEnemy> m_pFlyingEnemy;
	std::shared_ptr<StrongEnemy> m_pStrongEnemy;
	std::shared_ptr<BossEnemy> m_pBossEnemy;
};

