#include "ResultScene.h"
#include "TitleScene.h"
#include "SoundManager.h"
#include "game.h"
#include "Pad.h"
namespace
{
	constexpr int kMaxFadeBright = 255;
	// カメラの位置と注視点
	constexpr VECTOR kDefaultCameraPos = { 0.0f,200.0f,-840.0f };
	constexpr VECTOR kCameraTarget = { 0.0f,50.0f,0.0f };
	// カメラの視野角
	constexpr float kViewAngle = 0.447f;
	// nearとfarの位置
	constexpr float kCameraNearClip = 10.0f;
	constexpr float kCameraFarClip = 3000.0f;
	// 点滅周期
	constexpr int kBlinkCycleMs = 500;
	// フォントのサイズ、太さ
	constexpr int kFontSize = 60;
	constexpr int kFontThick = 5;

	constexpr int kScorePosX = 430;
	constexpr int kScorePosY = 100;

	constexpr int kTimeScorePosX = 300;
	constexpr int kTimeScorePosY = 220;
	constexpr int kTreasurePosY = 320;
	constexpr int kTotalScorePosY = 420;

	constexpr int kButtonPosX = 400;
	constexpr int kButtonPosY = 600;

	constexpr int kRankPosX = 950;
	constexpr int kRankPosY = 220;

	// 背景の位置
	constexpr int kBGPosX = -350;
	constexpr int kBGPosY = -150;

	// ランクの基準
	constexpr int kRankThresholdS = 20000;
	constexpr int kRankThresholdA = 15000;

	constexpr int kRollUpSpeed = 300;
}

ResultScene::ResultScene(std::shared_ptr<ScoreManager> pScoreManager, bool isGameover):
	m_pScoreManager(pScoreManager),
	m_isNextScene(false),
	m_playerPos({ 0.0f,0.0f,0.0f }),
	m_cameraPos({ 0.0f,0.0f,0.0f }),
	m_cameraTarget({ 0.0f,0.0f,0.0f }),
	m_viewAngle(0.0f),
	m_fontHandle(-1),
	m_bgHandle(-1),
	m_retrurnTitleHandle(-1),
	m_rankSHandle(-1),
	m_rankAHandle(-1),
	m_rankBHandle(-1),
	m_gameoverFlag(isGameover),
	m_timeScore(0),
	m_treasureScore(0),
	m_totalScore(0),
	m_drawTimeScore(0),
	m_drawTreasureScore(0),
	m_drawTotalScore(0),
	m_isDramrollStarted(false),
	m_isFinishSEPlayed(false),
	m_isBGMStarted(false)
{
}

void ResultScene::Init()
{
	SetUseZBuffer3D(true);	  // Zバッファを指定する
	SetWriteZBuffer3D(true);  // Zバッファへの書き込みを行う

	SetUseBackCulling(true);  // ポリゴンの裏面を表示しない

	// カメラの位置の初期化を行う

	// カメラ(始点)の位置
	m_cameraPos = kDefaultCameraPos;

	// カメラがどこを見ているか(注視点)
	m_cameraTarget = kCameraTarget;

	// カメラの位置と注視点を指定する
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, m_cameraTarget);

	// カメラの視野角を設定する
	m_viewAngle = kViewAngle;
	SetupCamera_Perspective(m_viewAngle);

	// カメラのnear,farを設定する
	SetCameraNearFar(kCameraNearClip, kCameraFarClip);
	//SoundManager::GetInstance()->PlayBGM();

	m_fontHandle = CreateFontToHandle(L"Arial Black", kFontSize, kFontThick, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	m_bgHandle = LoadGraph(L"Data/UI/result_seat.png");
	m_retrurnTitleHandle = LoadGraph(L"Data/title/button2.png");
	m_rankSHandle = LoadGraph(L"Data/UI/rank_s.png");
	m_rankAHandle = LoadGraph(L"Data/UI/rank_a.png");
	m_rankBHandle = LoadGraph(L"Data/UI/rank_b.png");
	m_timeScore = m_pScoreManager->GetTimeBonus();
	m_treasureScore = m_pScoreManager->GetTreasureCount() * 1000;
	m_totalScore = m_timeScore + m_treasureScore;
}

void ResultScene::End()
{
	DeleteFontToHandle(m_fontHandle);
	DeleteGraph(m_bgHandle);
	DeleteGraph(m_retrurnTitleHandle);
	DeleteGraph(m_rankSHandle);
	DeleteGraph(m_rankAHandle);
	DeleteGraph(m_rankBHandle);
	m_pScoreManager->End();
}

SceneBase* ResultScene::Update()
{
	UpdateFade();
	SoundManager::GetInstance()->Update();
	if (!m_isDramrollStarted)
	{
		SoundManager::GetInstance()->PlayDramroll(false); // ドラムロール(ループ)開始
		m_isDramrollStarted = true;
	}
	// ロールアップ演出の更新
	if (m_drawTimeScore < m_timeScore) 
	{
		m_drawTimeScore += kRollUpSpeed;
		if (m_drawTimeScore > m_timeScore) m_drawTimeScore = m_timeScore;
	}
	else if (m_drawTreasureScore < m_treasureScore) 
	{
		m_drawTreasureScore += kRollUpSpeed;
		if (m_drawTreasureScore > m_treasureScore) m_drawTreasureScore = m_treasureScore;
	}
	else if (m_drawTotalScore < m_totalScore) 
	{
		m_drawTotalScore += kRollUpSpeed;
		if (m_drawTotalScore > m_totalScore) m_drawTotalScore = m_totalScore;
	}
	else if (!m_isFinishSEPlayed)
	{
		SoundManager::GetInstance()->PlayDramroll(true); // 終了SE再生
		m_isFinishSEPlayed = true;
	}
	else if (m_isFinishSEPlayed && !m_isBGMStarted)
	{
		if (SoundManager::GetInstance()->IsPlayingFinishSE() == false)
		{
			SoundManager::GetInstance()->PlayBGM(); // BGM開始
			m_isBGMStarted = true;
		}
	}

	if (!m_isNextScene && !IsFadingOut() && Pad::isTrigger(PAD_INPUT_1) || Pad::isTrigger(PAD_INPUT_2)
		|| Pad::isTrigger(PAD_INPUT_3) || Pad::isTrigger(PAD_INPUT_4))
	{
		StartFadeOut();
		m_isNextScene = true;
	}
	if (IsFadingOut())
	{
		SoundManager::GetInstance()->FadeBGMVol();
	}
	// フェードが終了したら遷移する
	if (m_isNextScene && IsFadeComplete())
	{
		return new TitleScene();
	}
	return this;
}

void ResultScene::Draw()
{
	DrawGraph(kBGPosX,kBGPosY,m_bgHandle,true);
	float angle = (GetNowCount() % 2000) / 2000.0f * DX_PI_F * 2.0f;
	int alpha = (int)((sin(angle) * 0.5f + 0.5f) * 255); // 0 ～ 255 に変換
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawRectGraph(kButtonPosX,kButtonPosY,79,101,500,35,m_retrurnTitleHandle,true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // ブレンドモードを解除
	if (!m_gameoverFlag)
	{
		DrawFormatStringToHandle(kScorePosX, kScorePosY, 0xff8c00, m_fontHandle, L"GameClear!");
	}
	DrawFormatStringToHandle(kTimeScorePosX,kTimeScorePosY,0x87cefa,m_fontHandle,L"TimeScore:%d", m_drawTimeScore);
	DrawFormatStringToHandle(kTimeScorePosX, kTreasurePosY,0x87cefa,m_fontHandle,L"TreasureScore:%d", m_drawTreasureScore);
	DrawFormatStringToHandle(kTimeScorePosX,kTotalScorePosY,0x87cefa,m_fontHandle,L"TotalScore:%d", m_drawTotalScore);

	if (m_drawTotalScore >= m_totalScore)
	{
		if (m_totalScore >= kRankThresholdS)
		{
			DrawGraph(kRankPosX, kRankPosY, m_rankSHandle, true);
		}
		else if (m_totalScore >= kRankThresholdA)
		{
			DrawGraph(kRankPosX, kRankPosY, m_rankAHandle, true);
		}
		else
		{
			DrawGraph(kRankPosX, kRankPosY, m_rankBHandle, true);
		}
	}
	
	DrawFade();
}

SceneID ResultScene::GetSceneID() const
{
	return SceneID::ResultScene;
}
