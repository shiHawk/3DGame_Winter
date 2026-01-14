#include "TitleScene.h"
#include "SoundManager.h"
#include "Pad.h"
#include "game.h"
#include "GameScene.h"

namespace
{
	constexpr int kMaxFadeBright = 255;
	// カメラの位置と注視点
	constexpr VECTOR kDefaultCameraPos = { 0.0f,200.0f,-840.0f };
	constexpr VECTOR kCameraTarget = { 0.0f,80.0f,400.0f };
	// カメラの視野角
	constexpr float kViewAngle = 0.447f;
	// nearとfarの位置
	constexpr float kCameraNearClip = 10.0f;
	constexpr float kCameraFarClip = 3000.0f;
	// 点滅周期
	constexpr int kBlinkCycleMs = 500;
	constexpr VECTOR kSecondLight = { -0.5f, -0.5f, 0.8f };
	// ライトのカラー
	constexpr float kRed = 0.5f;
	constexpr float kGreen = 0.5f;
	constexpr float kBlue = 0.5f;

	constexpr int kTitlePosX = 352;
	constexpr int kTitlePosY = 40;
	constexpr int kButtonPosX = 490;
	constexpr int kButtonPosY = 630;

	// フォントのサイズ、太さ
	constexpr int kFontSize = 50;
	constexpr int kFontThick = 5;

	constexpr float kModelScale = 60.0f; // モデルのスケール
	constexpr VECTOR kDefaultWarriorPos = { 100.0f,-1.0f,0.0f };
	constexpr VECTOR kDefaultWizardPos = { -100.0f,-1.0f,0.0f };
	constexpr VECTOR kStoolPos = { 0.0f,-9.0f,0.0f };
	constexpr VECTOR kCoinPos = { 0.0f,41.0f,0.0f };
	constexpr VECTOR kBoxPos = { 300.0f,41.0f,500.0f };
	constexpr float kDifColor = 0.7f;

	constexpr int kSitAnim = 73;
	constexpr int kStandUpAnim = 75;
	constexpr float kAnimIncrement = 0.5f;

	constexpr VECTOR kStartTilePos = { -800.0f,-10.0f,800.0f };
	constexpr float kTileSize = 400.0f; // タイルサイズ
	constexpr int kMaxTileNum = 26;
	constexpr int kWallNumPerSide = 5; // 1辺あたりの壁の数
	constexpr int kTotalWallNum = kWallNumPerSide * 4;
	constexpr float kWallOffset = kTileSize * 0.5f;
	constexpr VECTOR kTestTilePos = { 600.0f,130.0f,-600.0f };
	constexpr VECTOR kWallRotY90 = { 0.0f, DX_PI_F / 2.0f, 0.0f };
}


TitleScene::TitleScene():
	m_isNextScene(false),
	m_warriorPos({0.0f,0.0f,0.0f}),
	m_wizardPos({ 0.0f,0.0f,0.0f }),
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
	m_wallModelBase(-1),
	m_stoolHandle(-1),
	m_coinHandle(-1),
	m_boxHandle(-1),
	m_lightHandle(-1),
	m_startHandle(-1),
	m_tilePos({ 0.0f,0.0f,0.0f }),
	m_tileStartPos({ 0.0f,0.0f,0.0f }),
	m_wallPos({ 0.0f,0.0f,0.0f })
{
}

void TitleScene::Init()
{
	m_pWarriorAnim = std::make_shared<Animation>();
	m_pWizardAnim = std::make_shared<Animation>();
	m_pWarriorAnim->Init();
	m_pWizardAnim->Init();
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
	SetLightDifColor(GetColorF(kRed, kGreen, kBlue, 0.0f));
	SetLightDifColorHandle(m_lightHandle, GetColorF(kRed, kGreen, kBlue, 0.0f));
	m_lightHandle = CreateDirLightHandle(kSecondLight);

	SoundManager::GetInstance()->PlayBGM();
	m_startHandle = LoadGraph(L"Data/title/button.png");
	m_titleLogoHandle = LoadGraph(L"Data/title/titleLogo.png");
	m_warriorModelHandle = MV1LoadModel(L"Data/model/Barbarian.mv1");
	m_wizardModelHandle = MV1LoadModel(L"Data/model/Mage.mv1");
	m_tileModelBase = MV1LoadModel(L"Data/model/floor_tile_large.mv1");
	m_wallModelBase = MV1LoadModel(L"Data/model/wall.mv1");
	m_stoolHandle = MV1LoadModel(L"Data/model/stool.mv1");
	m_coinHandle = MV1LoadModel(L"Data/model/coin_stack_small.mv1");
	m_boxHandle = MV1LoadModel(L"Data/model/box_stacked.mv1");
	m_fontHandle = CreateFontToHandle(L"Arial Black", kFontSize, kFontThick, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	MV1SetScale(m_warriorModelHandle,VGet(kModelScale, kModelScale, kModelScale));
	MV1SetScale(m_wizardModelHandle,VGet(kModelScale, kModelScale, kModelScale));
	SetStage();
	m_pWarriorAnim->AttachAnim(m_warriorModelHandle,kSitAnim);
	m_pWizardAnim->AttachAnim(m_wizardModelHandle, kSitAnim);

	m_warriorPos = kDefaultWarriorPos;
	m_wizardPos = kDefaultWizardPos;
	MV1SetRotationXYZ(m_warriorModelHandle, VGet(0.0f, 90.0f * DX_PI_F / 180.0f, 0.0f));
	MV1SetRotationXYZ(m_wizardModelHandle, VGet(0.0f, -90.0f * DX_PI_F / 180.0f, 0.0f));
	MV1SetScale(m_boxHandle,VGet(0.4f,0.4f,0.4f));

	MV1SetPosition(m_stoolHandle,kStoolPos);
	MV1SetPosition(m_coinHandle,kCoinPos);
	MV1SetPosition(m_boxHandle,kBoxPos);
}

void TitleScene::End()
{
	DeleteGraph(m_titleLogoHandle);
	DeleteGraph(m_startHandle);
	MV1DeleteModel(m_warriorModelHandle);
	MV1DeleteModel(m_wizardModelHandle);
	MV1DeleteModel(m_stoolHandle);
	MV1DeleteModel(m_coinHandle);
	MV1DeleteModel(m_boxHandle);
	DeleteLightHandle(m_lightHandle);
	DeleteFontToHandle(m_fontHandle);

	for (int tileModelHandle : m_tileModelHandles)
	{
		if (tileModelHandle != -1)
		{
			MV1DeleteModel(tileModelHandle);
		}
	}
	m_tileModelHandles.clear();
	MV1DeleteModel(m_tileModelBase);

	for (int wallModelHandle : m_wallModelHandles)
	{
		if (wallModelHandle != -1)
		{
			MV1DeleteModel(wallModelHandle);
		}
	}
	m_wallModelHandles.clear();
	MV1DeleteModel(m_wallModelBase);
	SoundManager::GetInstance()->StopBGM();
}

SceneBase* TitleScene::Update()
{
	UpdateFade();
	SoundManager::GetInstance()->Update();
	if (!m_isNextScene)
	{
		m_pWarriorAnim->ChangeAnim(m_warriorModelHandle, kSitAnim, true, kAnimIncrement);
		m_pWizardAnim->ChangeAnim(m_wizardModelHandle, kSitAnim, true, kAnimIncrement);
	}
	else
	{
		m_pWarriorAnim->ChangeAnim(m_warriorModelHandle, kStandUpAnim, false, kAnimIncrement);
		m_pWizardAnim->ChangeAnim(m_wizardModelHandle, kStandUpAnim, false, kAnimIncrement);
	}
	
	if (!m_isNextScene && !IsFadingOut() && Pad::isTrigger(PAD_INPUT_2))
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
		return new GameScene();
	}
	MV1SetPosition(m_warriorModelHandle,m_warriorPos);
	MV1SetPosition(m_wizardModelHandle,m_wizardPos);
	m_pWarriorAnim->UpdateAnim(m_warriorModelHandle);
	m_pWizardAnim->UpdateAnim(m_wizardModelHandle);
	return this;
}

void TitleScene::Draw()
{
	//DrawBox(0,0,Game::kScreenWidth,Game::kScreenHeight,0xffffff,true);
	for (int i = 0; i < m_tileTotal;i++)
	{
		MV1DrawModel(m_tileModelHandles[i]);
	}
	for (int wallModelHandle : m_wallModelHandles)
	{
		MV1DrawModel(wallModelHandle);
	}
	DrawGraph(kTitlePosX, kTitlePosY, m_titleLogoHandle, true);
	//DrawGraph(kButtonPosX, kButtonPosY, m_startHandle, true);
	DrawRectGraph(kButtonPosX, kButtonPosY,35,104,309,35, m_startHandle,true);
	MV1DrawModel(m_warriorModelHandle);
	MV1DrawModel(m_wizardModelHandle);
	MV1DrawModel(m_stoolHandle);
	MV1DrawModel(m_coinHandle);
	MV1SetDifColorScale(m_boxHandle, GetColorF(kDifColor, kDifColor, kDifColor,1.0f));
	MV1DrawModel(m_boxHandle);
	//DrawFormatStringToHandle(kButtonPosX,kButtonPosY,0xffffff,m_fontHandle,L"Press B button");
	DrawFade();
}

SceneID TitleScene::GetSceneID() const
{
	return SceneID::TitleScene;
}

void TitleScene::SetStage()
{
	m_tilePos = kStartTilePos;
	m_tileSize = kTileSize;
	m_tileTotal = kMaxTileNum;
	m_tileModelHandles.resize(m_tileTotal, -1);
	// コピー元から複製
	for (int i = 0; i < m_tileTotal;i++)
	{
		m_tileModelHandles[i] = MV1DuplicateModel(m_tileModelBase);
	}
	for (int i = 0; i < m_tileTotal;i++)
	{
		MV1SetPosition(m_tileModelHandles[i], m_tilePos);
		m_tilePos.x += kTileSize;
		if ((i + 1) % 5 == 0)
		{
			// x座標を開始位置に戻す
			m_tilePos.x = kStartTilePos.x;
			// z座標をkTileSize分引いて、手前へ移動
			m_tilePos.z -= kTileSize;
		}
	}

	m_wallModelHandles.resize(kTotalWallNum, -1);
	for (int i = 0; i < kTotalWallNum; i++)
	{
		m_wallModelHandles[i] = MV1DuplicateModel(m_wallModelBase);
	}
	m_wallPos.y = kStartTilePos.y;
	int wallIndex = 0; // m_wallModelHandles のインデックス
	m_wallPos.z = kStartTilePos.z + kWallOffset; // 800 + 200 = 1000
	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.x = kStartTilePos.x + (kTileSize * i); // -800, -400, 0, 400, 800

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		wallIndex++;
	}

	// Z最小 (-800) からさらに手前へ
	m_wallPos.z = (kStartTilePos.z - kTileSize * (kWallNumPerSide - 1)) - kWallOffset; // (800 - 400*4) - 200 = -1000
	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.x = kStartTilePos.x + (kTileSize * i); // -800, -400, 0, 400, 800

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		wallIndex++;
	}

	m_wallPos.x = kStartTilePos.x - kWallOffset; // -800 - 200 = -1000

	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.z = kStartTilePos.z - (kTileSize * i); // 800, 400, 0, -400, -800

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		MV1SetRotationXYZ(m_wallModelHandles[wallIndex], kWallRotY90); // 回転を適用
		wallIndex++;
	}

	// X最大 (800) からさらに右へ
	m_wallPos.x = (kStartTilePos.x + kTileSize * (kWallNumPerSide - 1)) + kWallOffset; // (-800 + 400*4) + 200 = 1000
	// 回転は左の壁と同じ (kWallRotY90)

	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.z = kStartTilePos.z - (kTileSize * i); // 800, 400, 0, -400, -800

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		MV1SetRotationXYZ(m_wallModelHandles[wallIndex], kWallRotY90); // 回転を適用
		wallIndex++;
	}
}
