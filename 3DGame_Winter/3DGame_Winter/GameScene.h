#pragma once
#include "SceneBase.h"
#include "Camera.h"
#include "Player.h"
#include "Enemy.h"
#include "NormalEnemy.h"
#include "Companion.h"
#include "Stage.h"
#include "GameplayCollision.h"
#include "WorldCollision.h"
#include "FlyingEnemy.h"
#include "StrongEnemy.h"
#include "BossEnemy.h"
#include "EffectManager.h"
#include "EnemyDataManager.h"
#include "BattleAreaManager.h"
#include "UIManager.h"
#include "DxLib.h"
#include "Pad.h"
#include "CharacterBase.h"
#include <memory>
class GameScene :public SceneBase
{
public:
	GameScene();
	virtual ~GameScene() {}
	virtual void Init() override;
	virtual void End() override;
	virtual SceneBase* Update() override;
	virtual void Draw() override;
private:
	void DrawGrid();
	VECTOR GetNearestEnemyPos(VECTOR basePos);
	void DeathProcessing();
	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Enemy> m_pEnemy;
	std::vector<std::shared_ptr<NormalEnemy>> m_pNormalEnemies;
	std::vector<std::shared_ptr<StrongEnemy>> m_pStrongEnemies;
	std::shared_ptr<NormalEnemy> m_pNormalEnemy;
	std::shared_ptr<Companion> m_pCompanion;
	std::shared_ptr<Stage> m_pStage;
	std::shared_ptr<GameplayCollision> m_pGameplayCollision;
	std::shared_ptr<WorldCollision> m_pWorldCollision;
	std::shared_ptr<FlyingEnemy> m_pFlyingEnemy;
	std::shared_ptr<StrongEnemy> m_pStrongEnemy;
	std::shared_ptr<BossEnemy> m_pBossEnemy;
	std::shared_ptr<EffectManager> m_pEffectManager;
	std::unique_ptr<EnemyDataManager> m_pEnemyDataManager;
	std::unique_ptr<BattleAreaManager> m_pBattleAreaManager;
	std::unique_ptr<UIManager> m_pUIManager;
	bool m_isNextScene;
};

