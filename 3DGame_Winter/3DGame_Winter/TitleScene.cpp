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
}


TitleScene::TitleScene():
	m_isNextScene(false),
	m_playerPos({0.0f,0.0f,0.0f}),
	m_cameraPos({ 0.0f,0.0f,0.0f }),
	m_cameraTarget({ 0.0f,0.0f,0.0f }),
	m_viewAngle(0.0f)
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
}

void TitleScene::End()
{
}

SceneBase* TitleScene::Update()
{
	UpdateFade();
	if (!m_isNextScene && !IsFadingOut() && Pad::isTrigger(PAD_INPUT_2))
	{
		// 2回目のAボタンでフェード開始
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
	DrawFade();
}
