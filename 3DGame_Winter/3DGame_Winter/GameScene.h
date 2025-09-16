#pragma once
#include "SceneBase.h"
#include "Camera.h"
#include "Player.h"
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
	int m_temp;
	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<Player> m_pPlayer;
};

