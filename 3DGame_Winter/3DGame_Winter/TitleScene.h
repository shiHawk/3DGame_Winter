#pragma once
#include "SceneBase.h"
#include <vector>
class TitleScene : public SceneBase
{
public:
	TitleScene();
	virtual ~TitleScene() {}
	virtual void Init() override;
	virtual void End() override;
	virtual SceneBase* Update() override;
	virtual void Draw() override;
private:
	bool m_isNextScene;
	VECTOR m_playerPos;
	// カメラの位置
	VECTOR m_cameraPos;
	// カメラの注視点(見ている場所)
	VECTOR m_cameraTarget;
	// カメラの視野角
	float m_viewAngle;
	int m_titleLogoHandle;
	int m_BGHandle;
	int m_manualHandle;
	int m_warriorModelHandle;
	int m_wizardModelHandle;
	int m_fontHandle;
	bool m_isManualHandle;
	bool m_isPlayingMovie;

	int m_tileTotal; // タイルの合計
	int m_stageStart; // 配置するタイルの始点
	int m_stageEnd; // 配置するタイルの終点
	float m_tileSize; // タイルのサイズ
	VECTOR m_tilePos;
	VECTOR m_tileStartPos;
	int m_tileModelBase;
	std::vector<int> m_tileModelHandles;
};

