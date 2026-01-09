#pragma once
#include "SceneBase.h"
class ResultScene :public SceneBase
{
public:
	ResultScene();
	~ResultScene() {};
	virtual void Init() override;
	virtual void End() override;
	virtual SceneBase* Update() override;
	virtual void Draw() override;
	virtual SceneID GetSceneID() const override;
private:
	bool m_isNextScene;
	VECTOR m_playerPos;
	// カメラの位置
	VECTOR m_cameraPos;
	// カメラの注視点(見ている場所)
	VECTOR m_cameraTarget;
	// カメラの視野角
	float m_viewAngle;
	int m_fontHandle;
};

