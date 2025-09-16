#pragma once
#include "SceneBase.h"
#include "GameScene.h"
class SceneManager : public SceneBase
{
public:
	SceneManager();
	virtual ~SceneManager() {};

	void Init();
	void End();

	void Updata();
	void Draw();
private:
	SceneBase* m_pScene;
};

