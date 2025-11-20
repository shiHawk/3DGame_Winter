#include "GameScene.h"
#include "DxLib.h"
namespace
{
	constexpr VECTOR kTrianglePos1 = { 200.0f,200.0f,0.0f };
	constexpr VECTOR kTrianglePos2 = { 400.0f,0.0f,0.0f };
	constexpr VECTOR kTrianglePos3 = { 0.0f,0.0f,0.0f };
	constexpr int kTriangleColor = 0xfff0ff;
	constexpr VECTOR kDefaultPos = { 0.0f,0.0f,0.0f };
	constexpr VECTOR kSpherePos2 = { 600.0f,0.0f,500.0f };
	constexpr float kSphereRadius = 20.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0xff0fff;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kGridSize = 1000.0f;   // グリッド全体の半径
	constexpr float kGridInterval = 100.0f; // 線と線の間隔
	constexpr int kGridColor = 0xffffff;    // グリッドの色
}
GameScene::GameScene()
{
}

void GameScene::Init()
{
	m_pCamera = std::make_shared<Camera>();
	m_pPlayer = std::make_shared<Player>();
	//m_pEnemy = std::make_shared<Enemy>();
	m_pNormalEnemy = std::make_shared<NormalEnemy>();
	m_pCompanion = std::make_shared<Companion>();
	m_pStage = std::make_shared<Stage>();
	m_pGameplayCollision = std::make_shared<GameplayCollision>();
	m_pWorldCollision = std::make_shared<WorldCollision>();
	m_pFlyingEnemy = std::make_shared<FlyingEnemy>();
	m_pEffectManager = std::make_shared<EffectManager>();
	m_pCamera->Init();
	m_pPlayer->Init(m_pCamera);
	m_pStage->Init();
	m_pNormalEnemy->Init(m_pPlayer,m_pCompanion);
	m_pCompanion->Init(m_pCamera);
	m_pGameplayCollision->Init(m_pPlayer, m_pCompanion, m_pNormalEnemy);
	m_pWorldCollision->Init(m_pPlayer, m_pStage, m_pCompanion);
	m_pFlyingEnemy->Init(m_pPlayer, m_pCompanion);
	m_pEffectManager->Init(m_pPlayer);
}

void GameScene::End()
{
	m_pCamera->End();
	m_pPlayer->End();
	//m_pEnemy->End();
	m_pNormalEnemy->End();
	m_pCompanion->End();
	m_pEffectManager->End();
	m_pFlyingEnemy->End();
	m_pStage->End();
}

SceneBase* GameScene::Update()
{
	m_pPlayer->Update();
	m_pNormalEnemy->Update();
	m_pCompanion->Update();
	m_pStage->Update();
	if (Pad::isTrigger(PAD_INPUT_6)) // RBボタンでプレイヤーとコンパニオンの切り替え
	{
		CharacterBase::ControlMode currentMode = m_pPlayer->GetControlMode(); // 現在のモード
		CharacterBase::ControlMode playerNewMode; // 次のプレイヤーのモード
		if (currentMode == CharacterBase::ControlMode::PLAYER)
		{
			playerNewMode = CharacterBase::ControlMode::COMPANION;
		}
		else
		{
			playerNewMode = CharacterBase::ControlMode::PLAYER;
		}
		m_pPlayer->SetControlMode(playerNewMode);
		CharacterBase::ControlMode companionNewMode; // コンパニオンのモード
		if (playerNewMode == CharacterBase::ControlMode::PLAYER)
		{
			companionNewMode = CharacterBase::ControlMode::COMPANION;
		}
		else
		{
			companionNewMode = CharacterBase::ControlMode::PLAYER;
		}
		m_pCompanion->SetControlMode(companionNewMode);
	}
	// 現在のコントロールモードを取得
	CharacterBase::ControlMode currentControlMode = m_pPlayer->GetControlMode();
	// モードに応じてカメラに渡す位置を決定
	if (currentControlMode == CharacterBase::ControlMode::PLAYER)
	{
		// プレイヤーが操作モードの場合、プレイヤーの位置をカメラに渡す
		m_pCamera->SetControlledCharacterPosition(m_pPlayer->GetPos());
		// プレイヤーの向きをカメラに渡す
		m_pCamera->SetPlayerDir(m_pPlayer->GetDir());
	}
	else if (currentControlMode == CharacterBase::ControlMode::COMPANION)
	{
		// コンパニオンが操作モードの場合、コンパニオンの位置をカメラに渡す
		m_pCamera->SetControlledCharacterPosition(m_pCompanion->GetPos());
		m_pCamera->SetPlayerDir(m_pCompanion->GetPlayerDir());
	}
	m_pPlayer->SetEnemyPos(m_pNormalEnemy->GetPos());
	m_pPlayer->SetFollowTargetPos(m_pCompanion->GetPos());
	m_pCamera->SetLockOnPosition(m_pNormalEnemy->GetPos());
	m_pCompanion->SetEnemyPos(m_pNormalEnemy->GetPos());
	m_pCompanion->SetPlayerPos(m_pPlayer->GetPos());
	m_pGameplayCollision->Update();
	m_pWorldCollision->Update();
	m_pFlyingEnemy->Update();
	m_pEffectManager->Update();
	m_pCamera->Update();
	return this;
}

void GameScene::Draw()
{
	m_pStage->Draw();
	m_pPlayer->Draw();
	m_pCompanion->Draw();
	m_pNormalEnemy->Draw();
	m_pEffectManager->Draw();
	//m_pFlyingEnemy->Draw();
	//DrawGrid();
}

void GameScene::DrawGrid()
{
	// Y=0の平面にグリッドを描く
	for (float i = -kGridSize; i <= kGridSize; i += kGridInterval)
	{
		// Z方向に平行な線（X固定）
		DrawLine3D(
			VGet(i, 0.0f, -kGridSize),  // 始点
			VGet(i, 0.0f, kGridSize),   // 終点
			kGridColor
		);

		// X方向に平行な線（Z固定）
		DrawLine3D(
			VGet(-kGridSize, 0.0f, i),  // 始点
			VGet(kGridSize, 0.0f, i),   // 終点
			kGridColor
		);
	}
}
