#include "TitleScene.h"
#include "Pad.h"
#include "game.h"
#include "GameScene.h"

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

	constexpr int kTitlePosX = 352;
	constexpr int kTitlePosY = 75;

	constexpr float kModelScale = 60.0f; // モデルのスケール
}


TitleScene::TitleScene():
	m_isNextScene(false),
	m_playerPos({0.0f,0.0f,0.0f}),
	m_cameraPos({ 0.0f,0.0f,0.0f }),
	m_cameraTarget({ 0.0f,0.0f,0.0f }),
	m_viewAngle(0.0f),
	m_titleLogoHandle(-1),
	m_BGHandle(-1),
	m_manualHandle(-1),
	m_warriorModelHandle(-1),
	m_wizardModelHandle(-1),
	m_fontHandle(-1),
	m_isManualHandle(false),
	m_isPlayingMovie(false),
	m_tileTotal(0),
	m_stageStart(0),
	m_stageEnd(0),
	m_tileSize(0),
	m_tileModelBase(-1),
	m_tilePos({ 0.0f,0.0f,0.0f }),
	m_tileStartPos({ 0.0f,0.0f,0.0f })
{
}

void TitleScene::Init()
{
	// 3D表示の設定
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

	m_titleLogoHandle = LoadGraph(L"Data/title/titleLogo.png");
	m_warriorModelHandle = MV1LoadModel(L"Data/model/Barbarian.mv1");
	m_wizardModelHandle = MV1LoadModel(L"Data/model/Mage.mv1");
	m_tileModelBase = MV1LoadModel(L"Data/model/floor_tile_large.mv1");
	MV1SetScale(m_warriorModelHandle,VGet(kModelScale, kModelScale, kModelScale));
	MV1SetScale(m_wizardModelHandle,VGet(kModelScale, kModelScale, kModelScale));
}

void TitleScene::End()
{
	DeleteGraph(m_titleLogoHandle);
	MV1DeleteModel(m_warriorModelHandle);
	MV1DeleteModel(m_wizardModelHandle);
	MV1DeleteModel(m_tileModelBase);
}

SceneBase* TitleScene::Update()
{
	UpdateFade();
	if (!m_isNextScene && !IsFadingOut() && Pad::isTrigger(PAD_INPUT_2))
	{
		StartFadeOut();
		m_isNextScene = true;
	}
	// フェードが終了したら遷移する
	if (m_isNextScene && IsFadeComplete())
	{
		return new GameScene();
	}
	return this;
}

void TitleScene::Draw()
{
	DrawBox(0,0,Game::kScreenWidth,Game::kScreenHeight,0xffffff,true);
	DrawGraph(kTitlePosX,kTitlePosY,m_titleLogoHandle,true);
	DrawFade();
}

SceneID TitleScene::GetSceneID() const
{
	return SceneID::TitleScene;
}
