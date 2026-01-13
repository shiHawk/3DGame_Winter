#pragma once
#include "SceneBase.h"
#include "ScoreManager.h"
#include <memory>
class ResultScene :public SceneBase
{
public:
	ResultScene(std::shared_ptr<ScoreManager> pScoreManager);
	~ResultScene() {};
	virtual void Init() override;
	virtual void End() override;
	virtual SceneBase* Update() override;
	virtual void Draw() override;
	virtual SceneID GetSceneID() const override;
private:
	std::shared_ptr<ScoreManager> m_pScoreManager;
	bool m_isNextScene;
	VECTOR m_playerPos;
	// カメラの位置
	VECTOR m_cameraPos;
	// カメラの注視点(見ている場所)
	VECTOR m_cameraTarget;
	// カメラの視野角
	float m_viewAngle;
	int m_fontHandle;
	int m_bgHandle;
};

