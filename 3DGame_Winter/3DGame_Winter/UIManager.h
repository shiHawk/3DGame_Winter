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
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion,std::shared_ptr<BossEnemy> pBoss,
		      std::vector<std::shared_ptr<NormalEnemy>> pNormalEnemies, std::vector<std::shared_ptr<StrongEnemy>> pStrongEnemies);
	void End();
	void Updata();
	void Draw();
	void DrawOperationPlayer();
	void SetOperationChara(bool isWarriorOperation) { m_isOperationWarrior = isWarriorOperation; }
private:
	void DrawHp();
	void DrawSg();
	void DrawBossHp();
	void DrawEnemyHP();
	void DrawSingleEnemyBar(VECTOR pos, int hp, int maxHp, float width, float height, float offsetY);
	void DrawManualUI();
	void DrawBuffUI();
	void DrawDamage(VECTOR pos);
	struct DamageText 
	{
		VECTOR worldPos; // 出現した3D座標
		int damage;      // 表示する数値
		float timer;     // 残り表示時間
		unsigned int color; // 色
	};
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
	int m_enemyHPHandle;
	int m_enemyHPFrameHandle;
	int m_warriorIconHandle;
	int m_wizardIconHandle;
	int m_fontHandle;
	int m_manualFontHandle;
	int m_aButtonHandle;
	int m_bButtonHandle;
	int m_xButtonHandle;
	int m_yButtonHandle;
	int m_rbButtonHandle;
	int m_lbButtonHandle;
	int m_stickHandle;
	int m_manualFrameHandle;
	int m_manualIconHandle;
	int m_buffIconHandle;
	int m_buffSwapIconHandle; // 交代時の一時的な攻撃力UPのバフ
	int m_operationPlayerUI;
	bool m_isDisplayManual;
	bool m_isDisplayBossHp;
	bool m_isOperationWarrior;
	std::shared_ptr<Player> m_pPlayer;
	std::vector<std::shared_ptr<NormalEnemy>> m_pNormalEnemies;
	std::vector<std::shared_ptr<StrongEnemy>> m_pStrongEnemies;
	std::shared_ptr<NormalEnemy> m_pNormalEnemy;
	std::shared_ptr<Companion> m_pCompanion;
	std::shared_ptr<FlyingEnemy> m_pFlyingEnemy;
	std::shared_ptr<StrongEnemy> m_pStrongEnemy;
	std::shared_ptr<BossEnemy> m_pBossEnemy;
};

