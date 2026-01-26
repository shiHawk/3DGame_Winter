#include "GameScene.h"
#include "ResultScene.h"
#include "SoundManager.h"
#include "DxLib.h"
namespace
{
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
	constexpr float kEnemySearchRange = 800.0f; // 敵を探す最大範囲
	constexpr VECTOR kInvalidPos = { 1000000.0f, 1000000.0f, 1000000.0f }; // 無効な座標（超遠方）
	constexpr float kSkyDomeScale = 120.0f;
	constexpr float kSensingRange = 600.0f;
	constexpr float kBattleAreaSize = 150.0f;
	constexpr float kBossBattleAreaSize = 350.0f;
}
GameScene::GameScene():
	m_isNextScene(false),
	m_isGameover(false)
{
}

void GameScene::Init()
{
	m_pCamera = std::make_shared<Camera>();
	m_pPlayer = std::make_shared<Player>();
	//m_pEnemy = std::make_shared<Enemy>();
	//m_pNormalEnemy = std::make_shared<NormalEnemy>();
	m_pCompanion = std::make_shared<Companion>();
	m_pStage = std::make_shared<Stage>();
	m_pGameplayCollision = std::make_shared<GameplayCollision>();
	m_pWorldCollision = std::make_shared<WorldCollision>();
	m_pFlyingEnemy = std::make_shared<FlyingEnemy>();
	m_pBossEnemy = std::make_shared<BossEnemy>();
	//m_pStrongEnemy = std::make_shared<StrongEnemy>();
	m_pEffectManager = std::make_shared<EffectManager>();
	m_pEnemyDataManager = std::make_unique<EnemyDataManager>();
	m_pBattleAreaManager = std::make_unique<BattleAreaManager>();
	m_pUIManager = std::make_unique<UIManager>();
	m_pScoreManager = std::make_shared<ScoreManager>();
	m_pSkyDome = std::make_shared<SkyDome>();
	m_pChest = std::make_shared<Chest>();
	m_pCamera->Init();
	m_pPlayer->Init(m_pCamera,m_pEffectManager);
	m_pStage->Init();
	m_pEnemyDataManager->LoadEnemyData("Data/enemyData/enemyData.csv",m_pNormalEnemies,m_pStrongEnemies,
									   m_pPlayer,m_pCompanion,m_pEffectManager);
	//m_pNormalEnemy->Init(m_pPlayer,m_pCompanion);
	m_pCompanion->Init(m_pCamera);
	m_pGameplayCollision->Init(m_pPlayer, m_pCompanion, m_pNormalEnemies, m_pStrongEnemies,m_pBossEnemy);
	m_pWorldCollision->Init(m_pPlayer, m_pStage, m_pCompanion,m_pNormalEnemies,m_pStrongEnemies);
	m_pFlyingEnemy->Init(m_pPlayer, m_pCompanion);
	m_pBossEnemy->Init(m_pPlayer, m_pCompanion,m_pEffectManager);
	//m_pStrongEnemy->Init(m_pPlayer, m_pCompanion);
	m_pEffectManager->Init(m_pPlayer,m_pCompanion,m_pGameplayCollision);
	m_pBattleAreaManager->Init(m_pPlayer, m_pCompanion);
	m_pBattleAreaManager->SetEnemy(m_pNormalEnemies, m_pStrongEnemies,m_pBossEnemy);
	m_pUIManager->Init(m_pPlayer, m_pCompanion,m_pBossEnemy, m_pNormalEnemies, m_pStrongEnemies);
	m_pScoreManager->Init();
	m_pSkyDome->Init();
	m_pSkyDome->SetScale(kSkyDomeScale);
	m_pChest->Init(m_pPlayer, m_pCompanion,m_pEffectManager);
	SoundManager::GetInstance()->PlayBGM();
}

void GameScene::End()
{
	m_pCamera->End();
	m_pPlayer->End();
	//m_pEnemy->End();
	//m_pNormalEnemy->End();
	for (auto& enemy : m_pNormalEnemies) 
	{ 
		enemy->End(); 
	}
	for (auto& enemy : m_pStrongEnemies)
	{
		enemy->End();
	}
	m_pCompanion->End();
	m_pEffectManager->End();
	m_pFlyingEnemy->End();
	m_pBossEnemy->End();
	//m_pStrongEnemy->End();
	m_pStage->End();
	m_pUIManager->End();
	m_pSkyDome->End();
	m_pChest->End();
	SoundManager::GetInstance()->StopBGM();
}

SceneBase* GameScene::Update()
{
	UpdateFade();
	m_pSkyDome->SetPos(m_pCamera->GetCameraPos());
	SoundManager::GetInstance()->Update();
	if (!m_pPlayer->IsDead())
	{
		m_pPlayer->Update();
	}
	if (!m_pCompanion->IsDead())
	{
		m_pCompanion->Update();
	}
	for (auto& enemy : m_pNormalEnemies)
	{
		enemy->Update();
	}
	for (auto& enemy : m_pStrongEnemies)
	{
		enemy->Update();
	}
	m_pStage->Update();
	if (Pad::isTrigger(PAD_INPUT_6)) // RBボタンでプレイヤーとコンパニオンの切り替え
	{
		ChangeControl();
	}

	// 現在のコントロールモードを取得
	CharacterBase::ControlMode currentControlMode = m_pPlayer->GetControlMode();
	if (currentControlMode == CharacterBase::ControlMode::PLAYER && m_pPlayer->IsDead()) // 近接キャラがプレイヤーモードで死亡した場合
	{
		if (!m_pCompanion->IsDead())
		{
			ChangeControl();
		}
	}
	else if (currentControlMode == CharacterBase::ControlMode::COMPANION && m_pCompanion->IsDead()) // 遠距離キャラがプレイヤーモードで死亡した場合
	{
		if (!m_pPlayer->IsDead())
		{
			ChangeControl();
		}
	}
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
	m_pPlayer->SetFollowTargetPos(m_pCompanion->GetPos());
	//m_pPlayer->SetEnemyPos(m_pNormalEnemy->GetPos());
	//m_pCamera->SetLockOnPosition(m_pNormalEnemy->GetPos());
	//m_pCompanion->SetEnemyPos(m_pNormalEnemy->GetPos());
	m_pCompanion->SetPlayerPos(m_pPlayer->GetPos());

	// 最も近い敵の座標を取得
	VECTOR currentBasePos = (currentControlMode == CharacterBase::ControlMode::PLAYER)
						     ? m_pPlayer->GetPos()
							 : m_pCompanion->GetPos();

	VECTOR targetEnemyPos = GetNearestEnemyPos(currentBasePos);
	m_pCamera->SetLockOnPosition(targetEnemyPos);
	if (targetEnemyPos.x < 100000.0f)
	{
		m_pPlayer->SetEnemyPos(targetEnemyPos);
		m_pCompanion->SetEnemyPos(targetEnemyPos);
	}
	else
	{
		// 敵がいない（ボスが死んだ）ならロックオンを強制解除
		m_pCamera->SetIsLockOn(false);
		m_pCamera->SetIsBossBattle(false); // ボス戦フラグも折る
	}
	m_pPlayer->SetEnemyAttackSensingFlag(IsEnemyAttacking(m_pPlayer->GetPos()));
	//printfDx(L"targetPosX:%f,targetPosY:%f,targetPosZ:%f\n",targetEnemyPos.x,targetEnemyPos.y,targetEnemyPos.z);

	// 取得した「一番近い敵の座標」を各クラスに渡す
	m_pPlayer->SetEnemyPos(targetEnemyPos);
	m_pCompanion->SetEnemyPos(targetEnemyPos);
	m_pCamera->SetLockOnPosition(targetEnemyPos); // ロックオン対象も一番近い敵にする

	m_pGameplayCollision->Update();
	m_pWorldCollision->Update();
	m_pFlyingEnemy->Update();
	m_pBossEnemy->Update();
	m_pSkyDome->Update();
	m_pChest->Update();
	//m_pStrongEnemy->Update();
	
	m_pEffectManager->Update();
	
	// 操作中のキャラクターを決定
	std::shared_ptr<CharacterBase> activeCharacter = nullptr;
	if (currentControlMode == CharacterBase::ControlMode::PLAYER)
	{
		activeCharacter = m_pPlayer; // プレイヤー操作中
	}
	else
	{
		activeCharacter = m_pCompanion; // コンパニオン操作中
	}

	// BattleAreaManagerの更新
	// activeCharacter を渡すことで、操作中のキャラの位置を基準にエリア判定が行われます
	m_pBattleAreaManager->Update(activeCharacter, m_pNormalEnemies, m_pStrongEnemies);
	m_pUIManager->Updata();
	m_pScoreManager->Updata();
	m_pCamera->Update();
	DeathProcessing();

	if (m_pBattleAreaManager->IsBattleAreaActive())
	{
		if (VSize(VSub(m_pBossEnemy->GetPos(), m_pPlayer->GetPos())) <= 2000.0f)
		{
			m_pEffectManager->BattleAreaEffect(m_pBattleAreaManager->GetCenterPos(), kBossBattleAreaSize);
			SoundManager::GetInstance()->SwitchToBossBGM();
			m_pCamera->SetIsBossBattle(true);
		}
		else
		{
			m_pEffectManager->BattleAreaEffect(m_pBattleAreaManager->GetCenterPos(), kBattleAreaSize);
			m_pCamera->SetIsBossBattle(false);
		}
	}
	else
	{
		// エリアが無効ならエフェクトを停止
		m_pEffectManager->StopBattleAreaEffect();
	}

	if (!m_isNextScene && !IsFadingOut() && (m_pBossEnemy->IsDead() || (m_pPlayer->IsDead() && m_pCompanion->IsDead())))
	{
		StartFadeOut();
		m_isNextScene = true;
		if (m_pPlayer->IsDead() && m_pCompanion->IsDead())
		{
			m_isGameover = true;
			m_pScoreManager->SetGameoverFlag(true);
		}
	}
	if (IsFadingOut())
	{
		SoundManager::GetInstance()->FadeBGMVol();
	}
	// フェードが終了したら遷移する
	if (m_isNextScene && IsFadeComplete())
	{
		return new ResultScene(m_pScoreManager,m_isGameover);
	}
	
	return this;
}

void GameScene::Draw()
{
	m_pSkyDome->Draw();
	m_pStage->Draw();
	m_pPlayer->Draw();
	m_pCompanion->Draw();
	//m_pNormalEnemy->Draw();
	for (auto& enemy : m_pNormalEnemies)
	{
		enemy->Draw();
	}
	m_pEffectManager->Draw();
	//m_pFlyingEnemy->Draw();
	//m_pStrongEnemy->Draw();
	for (auto& enemy : m_pStrongEnemies)
	{
		enemy->Draw();
	}
	m_pBossEnemy->Draw();
	m_pChest->Draw();
	m_pUIManager->Draw();
	m_pCamera->Draw();
	DrawFade();
	//DrawGrid();
}

SceneID GameScene::GetSceneID() const
{
	return SceneID::GameScene;
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

VECTOR GameScene::GetNearestEnemyPos(VECTOR basePos)
{
	float minDistanceSq = kEnemySearchRange * kEnemySearchRange;
	VECTOR nearestPos = kInvalidPos; // 初期値として「無効な座標」を設定
	bool found = false;

	// NormalEnemiesから探す
	for (auto& enemy : m_pNormalEnemies) {
		if (enemy->IsDead())
		{
			continue;
		}
		float distSq = VSquareSize(VSub(enemy->GetPos(), basePos));
		if (distSq < minDistanceSq) {
			minDistanceSq = distSq;
			nearestPos = enemy->GetPos();
			found = true;
		}
	}

	// StrongEnemiesからも探す
	for (auto& enemy : m_pStrongEnemies) {
		if (enemy->IsDead())
		{
			continue;
		}
		float distSq = VSquareSize(VSub(enemy->GetPos(), basePos));
		if (distSq < minDistanceSq) {
			minDistanceSq = distSq;
			nearestPos = enemy->GetPos();
			found = true;
		}
	}

	if (m_pBossEnemy != nullptr && !m_pBossEnemy->IsDead())
	{
		float distSq = VSquareSize(VSub(m_pBossEnemy->GetPos(), basePos));
		if (m_pCamera->IsBossBattle() || distSq < minDistanceSq)
		{
			// ボスを優先、あるいは一番近ければ更新
			nearestPos = m_pBossEnemy->GetPos();
			found = true;
		}
	}

	return nearestPos;
}

void GameScene::DeathProcessing()
{
	if (m_pPlayer->IsDead())
	{
		m_pPlayer->End();
	}
	if (m_pCompanion->IsDead())
	{
		m_pCompanion->End();
	}
}

bool GameScene::IsEnemyAttacking(VECTOR targetPos)
{
	// 感知する範囲（これ以上離れている敵の攻撃は無視する）
	float rangeSq = kSensingRange * kSensingRange;
	for (const auto& enemy : m_pNormalEnemies)
	{
		if (enemy->IsDead()) continue;

		// 攻撃中 かつ 距離が近い場合
		if (enemy->GetAttackInfo().active)
		{
			if (VSquareSize(VSub(enemy->GetPos(), targetPos)) < rangeSq)
			{
				return true;
			}
		}
	}

	for (const auto& enemy : m_pStrongEnemies)
	{
		if (enemy->IsDead()) continue;

		if (enemy->GetAttackInfo().active)
		{
			if (VSquareSize(VSub(enemy->GetPos(), targetPos)) < rangeSq)
			{
				return true;
			}
		}
	}

	if (m_pBossEnemy && !m_pBossEnemy->IsDead())
	{
		if (m_pBossEnemy->GetAttackInfo().active)
		{
			if (VSquareSize(VSub(m_pBossEnemy->GetPos(), targetPos)) < rangeSq)
			{
				return true;
			}
		}
	}
	return false;
}

void GameScene::ChangeControl()
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

	// 切り替え時のエフェクト設定
	VECTOR effectPos;
	if (playerNewMode == CharacterBase::ControlMode::PLAYER)
	{
		effectPos = m_pPlayer->GetPos();
	}
	else
	{
		effectPos = m_pCompanion->GetPos();
	}
	m_pEffectManager->PlayChangeEffect(effectPos);
}
