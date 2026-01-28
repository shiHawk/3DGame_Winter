#pragma once
#include "SceneBase.h"
#include "ScoreManager.h"
#include <memory>
class ResultScene :public SceneBase
{
public:
	ResultScene(std::shared_ptr<ScoreManager> pScoreManager,bool isGameover);
	~ResultScene() {};
	virtual void Init() override;
	virtual void End() override;
	virtual SceneBase* Update() override;
	virtual void Draw() override;
	virtual SceneID GetSceneID() const override;
	void SetGameoverFlag(bool gameover) { m_gameoverFlag = gameover; }
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
	int m_retrurnTitleHandle;
	// ランクのハンドル
	int m_rankSHandle;
	int m_rankAHandle;
	int m_rankBHandle;

	bool m_gameoverFlag;
	int m_timeScore;
	int m_treasureScore;
	int m_totalScore;

	int m_drawTimeScore;     // 表示用のタイムスコア
	int m_drawTreasureScore; // 表示用の宝箱スコア
	int m_drawTotalScore;    // 表示用の合計スコア

	bool m_isDramrollStarted;  // ドラムロールを開始したか
	bool m_isFinishSEPlayed;   // 終了SEを鳴らしたか
	bool m_isBGMStarted; 
};

