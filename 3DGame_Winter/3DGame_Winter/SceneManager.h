#pragma once
#include "SceneBase.h"
#include "GameScene.h"
class SceneManager : public SceneBase
{
public:
	SceneManager();
	virtual ~SceneManager() {};

	virtual void Init() override;
	virtual void End() override;

	virtual SceneBase* Update() override;
	virtual void Draw() override;
private:
	SceneBase* m_pScene;
};

