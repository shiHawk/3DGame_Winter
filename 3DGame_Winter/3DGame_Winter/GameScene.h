#pragma once
#include "SceneBase.h"
#include "Camera.h"
#include "Player.h"
#include "Enemy.h"
#include "NormalEnemy.h"
#include "Companion.h"
#include "GameplayCollision.h"
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
	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Enemy> m_pEnemy;
	std::shared_ptr<NormalEnemy> m_pNormalEnemy;
	std::shared_ptr<Companion> m_pCompanion;
	std::shared_ptr<GameplayCollision> m_pGameplayCollision;
};

