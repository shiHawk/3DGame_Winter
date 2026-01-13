#pragma once
#include "SceneBase.h"
#include "Animation.h"
#include <vector>
#include <memory>
class TitleScene : public SceneBase
{
public:
	TitleScene();
	virtual ~TitleScene() {}
	virtual void Init() override;
	virtual void End() override;
	virtual SceneBase* Update() override;
	virtual void Draw() override;
	virtual SceneID GetSceneID() const override;
private:
	void SetStage();
	std::shared_ptr<Animation> m_pWarriorAnim;
	std::shared_ptr<Animation> m_pWizardAnim;
	bool m_isNextScene;
	VECTOR m_warriorPos;
	VECTOR m_wizardPos;
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
	VECTOR m_wallPos;
	int m_tileModelBase;
	int m_wallModelBase;
	int m_stoolHandle;
	int m_coinHandle;
	int m_lightHandle;
	std::vector<int> m_tileModelHandles;
	std::vector<int> m_wallModelHandles;
};

