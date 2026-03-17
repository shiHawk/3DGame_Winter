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
	constexpr float kShadowMapRange = 2000.0f;
	constexpr float kSkyDomeScale = 120.0f;
	constexpr float kSensingRange = 300.0f;
	constexpr float kBattleAreaSize = 150.0f;
	constexpr float kBossBattleAreaSize = 350.0f;
	constexpr int kChangePowerUpBonus = 30;
	constexpr float kPowerUpTimeLimit = 100.0f;
}
GameScene::GameScene():
	m_isNextScene(false),
	m_isGameover(false),
	m_powerUpTime(0.0f),
	m_shadowReferencePosition({0.0f,0.0f,0.0f})
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
	m_pChest->Init(m_pPlayer, m_pCompanion,m_pEffectManager,m_pScoreManager,m_pUIManager.get());
	SoundManager::GetInstance()->PlayBGM();
	m_shadowMapHandle = MakeShadowMap(2048, 2048);
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
	m_pChest->End();
	m_pStage->End();
	m_pUIManager->End();
	m_pSkyDome->End();
	SoundManager::GetInstance()->StopBGM();
	DeleteShadowMap(m_shadowMapHandle);
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
		for (const auto& res : enemy->PopDamageResults())
		{
			m_pUIManager->RegisterDamageUI(res.pos, res.damage, res.color); // ダメージを表示するためにUIManagerに情報を渡す
		}
	}
	for (auto& enemy : m_pStrongEnemies)
	{
		enemy->Update();
		for (const auto& res : enemy->PopDamageResults())
		{
			m_pUIManager->RegisterDamageUI(res.pos, res.damage, res.color); // ダメージを表示するためにUIManagerに情報を渡す
		}
	}
	m_pStage->Update();
	if (Pad::isTrigger(PAD_INPUT_6 | PAD_INPUT_RT) && (!m_pPlayer->IsDead() && !m_pCompanion->IsDead())) // RBボタンかRTボタンでプレイヤーとコンパニオンの切り替え
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
	// モードに応じてカメラに渡す位置の情報を決定
	if (currentControlMode == CharacterBase::ControlMode::PLAYER)
	{
		// プレイヤーが操作モードの場合、プレイヤーの位置をカメラに渡す
		m_pCamera->SetControlledCharacterPosition(m_pPlayer->GetPos());
	}
	else if (currentControlMode == CharacterBase::ControlMode::COMPANION)
	{
		// コンパニオンが操作モードの場合、コンパニオンの位置をカメラに渡す
		m_pCamera->SetControlledCharacterPosition(m_pCompanion->GetPos());
	}
	m_pPlayer->SetFollowTargetPos(m_pCompanion->GetPos());
	m_pCompanion->SetPlayerPos(m_pPlayer->GetPos());

	// 操作キャラとAIキャラを判別
	CharacterBase* pControlledChar = nullptr; // 操作しているキャラ
	CharacterBase* pAIChar = nullptr;         // コンパニオン

	if (currentControlMode == CharacterBase::ControlMode::PLAYER)
	{
		pControlledChar = m_pPlayer.get();
		m_shadowReferencePosition = m_pPlayer->GetPos();
		pAIChar = m_pCompanion.get();
	}
	else
	{
		pControlledChar = m_pCompanion.get();
		m_shadowReferencePosition = m_pCompanion->GetPos();
		pAIChar = m_pPlayer.get();
	}

	// まず操作キャラにとって一番近い敵を取得（除外なし）
	VECTOR targetForMain = GetNearestEnemyPos(pControlledChar->GetPos(),kInvalidPos);

	// 次にAIキャラにとって近い敵を取得（操作キャラのターゲットを除外指定）
	VECTOR targetForAI = GetNearestEnemyPos(pAIChar->GetPos(), targetForMain);

	//それぞれにセット
	pControlledChar->SetEnemyPos(targetForMain);
	pAIChar->SetEnemyPos(targetForAI);

	// カメラのロックオンは操作キャラのターゲットに合わせる
	m_pCamera->SetLockOnPosition(targetForMain);
	m_pPlayer->SetEnemyAttackSensingFlag(IsEnemyAttacking(m_pPlayer->GetPos()));
	// ターゲットが存在するかチェック（x座標が有効範囲内か）
	bool isTargetValid = (targetForMain.x < 100000.0f);

	if (!isTargetValid)
	{
		// 敵がいない（ボスが死んだ）ならロックオンを強制解除
		m_pCamera->SetIsLockOn(false);
		m_pCamera->SetIsBossBattle(false);
	}

	//// 最も近い敵の座標を取得
	//VECTOR currentBasePos = (currentControlMode == CharacterBase::ControlMode::PLAYER)
	//					     ? m_pPlayer->GetPos()
	//						 : m_pCompanion->GetPos();
	//VECTOR targetEnemyPos = GetNearestEnemyPos(currentBasePos,kInvalidPos);
	//m_pCamera->SetLockOnPosition(targetEnemyPos);
	//if (targetEnemyPos.x < 100000.0f)
	//{
	//	m_pPlayer->SetEnemyPos(targetEnemyPos);
	//	m_pCompanion->SetEnemyPos(targetEnemyPos);
	//}
	//else
	//{
	//	// 敵がいない（ボスが死んだ）ならロックオンを強制解除
	//	m_pCamera->SetIsLockOn(false);
	//	m_pCamera->SetIsBossBattle(false); // ボス戦フラグも折る
	//}
	//m_pPlayer->SetEnemyAttackSensingFlag(IsEnemyAttacking(m_pPlayer->GetPos()));
	////printfDx(L"targetPosX:%f,targetPosY:%f,targetPosZ:%f\n",targetEnemyPos.x,targetEnemyPos.y,targetEnemyPos.z);
	//// 取得した「一番近い敵の座標」を各クラスに渡す
	//m_pPlayer->SetEnemyPos(targetEnemyPos);
	//m_pCompanion->SetEnemyPos(targetEnemyPos);
	//m_pCamera->SetLockOnPosition(targetEnemyPos); // ロックオン対象も一番近い敵にする

	m_pGameplayCollision->Update();
	m_pWorldCollision->Update();
	m_pFlyingEnemy->Update();
	m_pBossEnemy->Update();
	auto results = m_pBossEnemy->PopDamageResults(); // ボスに発生させたダメージを表示するためにUIManagerに情報を渡す
	for (const auto& res : results)
	{
		m_pUIManager->RegisterDamageUI(res.pos, res.damage, res.color);
	}
	m_pSkyDome->Update();
	m_pChest->Update();
	m_pChest->SetBattleActiveFlag(m_pBattleAreaManager->IsBattleAreaActive());
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
	// activeCharacter を渡すことで、操作中のキャラの位置を基準にエリア判定が行われる
	m_pBattleAreaManager->Update(activeCharacter, m_pNormalEnemies, m_pStrongEnemies);
	m_pUIManager->Updata();
	m_pScoreManager->Updata();
	m_pCamera->Update();
	DeathProcessing();

	if (m_pBattleAreaManager->IsBattleAreaActive())
	{
		if (VSize(VSub(m_pBossEnemy->GetPos(), m_pPlayer->GetPos())) <= 2000.0f || VSize(VSub(m_pBossEnemy->GetPos(), m_pCompanion->GetPos())) <= 2000.0f)
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

	if (m_powerUpTime > 0.0f)
	{
		m_powerUpTime--;
	}
	else
	{
		m_powerUpTime = 0.0f;
		m_pPlayer->ResetPower();
		m_pCompanion->ResetPower();
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
	// シャドウマップ作成
	SetShadowMapLightDirection(m_shadowMapHandle, VGet(0.5f, -1.0f, 0.5f));
	SetShadowMapDrawArea(m_shadowMapHandle,VSub(m_shadowReferencePosition,VGet(kShadowMapRange,60.0f, kShadowMapRange)),
						 VAdd(m_shadowReferencePosition, VGet(kShadowMapRange, 10.0f, kShadowMapRange)));
	// シャドウマップへの書き込み
	ShadowMap_DrawSetup(m_shadowMapHandle);
	m_pStage->Draw();
	m_pPlayer->Draw();
	m_pCompanion->Draw();
	for (auto& enemy : m_pNormalEnemies)
	{
		enemy->Draw();
	}
	for (auto& enemy : m_pStrongEnemies)
	{
		enemy->Draw();
	}
	m_pBossEnemy->Draw();
	m_pChest->Draw();
	ShadowMap_DrawEnd();
	// 実際の画面への描画パス
	ClearDrawScreen(); // 通常の描画開始
	m_pSkyDome->Draw();
	// シャドウマップの適応
	SetUseShadowMap(0, m_shadowMapHandle);
	m_pStage->Draw();
	m_pPlayer->Draw();
	m_pCompanion->Draw();
	for (auto& enemy : m_pNormalEnemies)
	{
		enemy->Draw();
	}
	for (auto& enemy : m_pStrongEnemies)
	{
		enemy->Draw();
	}
	m_pBossEnemy->Draw();
	m_pChest->Draw();
	m_pCamera->Draw();
	SetUseShadowMap(0, -1); // シャドウマップの使用を終了
	// チャージの円はシャドウマップの影響を受けてほしくないのでシャドウマップ使用後に描画
	for (auto& enemy : m_pStrongEnemies)
	{
		enemy->DrawChargeEffect();
	}
	m_pEffectManager->Draw();
	m_pUIManager->Draw();
	DrawFade();
}

SceneID GameScene::GetSceneID() const
{
	return SceneID::GameScene;
}

VECTOR GameScene::GetNearestEnemyPos(VECTOR basePos, VECTOR avoidPos)
{
	float minDistanceSq = kEnemySearchRange * kEnemySearchRange;     // 通常の最小距離
	VECTOR nearestPos = kInvalidPos;                                 // 通常の最寄り敵

	float minDistanceSqSub = kEnemySearchRange * kEnemySearchRange;  // 被りを避けた場合の最小距離
	VECTOR nearestPosSub = kInvalidPos;                              // 被りを避けた場合の最寄り敵

	auto CheckEnemy = [&](const auto& enemy) // 判定用ラムダ式
		{
			if (enemy->IsDead()) return;

			VECTOR ePos = enemy->GetPos();
			float distSq = VSquareSize(VSub(ePos, basePos));

			// 1 単純に一番近い敵を探す
			if (distSq < minDistanceSq)
			{
				minDistanceSq = distSq;
				nearestPos = ePos;
			}

			// 2 被りを避けた敵を探す（avoidPos と座標が違う場合のみ更新）
			if (VSquareSize(VSub(ePos, avoidPos)) > 1.0f)
			{
				if (distSq < minDistanceSqSub)
				{
					minDistanceSqSub = distSq;
					nearestPosSub = ePos;
				}
			}
		};
	// NormalEnemiesから探す
	for (auto& enemy : m_pNormalEnemies) 
	{
		CheckEnemy(enemy);
	}
	// StrongEnemiesからも探す
	for (auto& enemy : m_pStrongEnemies)
	{
		CheckEnemy(enemy);
	}

	//float minDistanceSq = kEnemySearchRange * kEnemySearchRange;
	//VECTOR nearestPos = kInvalidPos; // 初期値として無効な座標を設定
	//bool found = false;
	//// NormalEnemiesから探す
	//for (auto& enemy : m_pNormalEnemies) 
	//{
	//	if (enemy->IsDead())
	//	{
	//		continue;
	//	}
	//	float distSq = VSquareSize(VSub(enemy->GetPos(), basePos));
	//	if (distSq < minDistanceSq) 
	//	{
	//		minDistanceSq = distSq;
	//		nearestPos = enemy->GetPos();
	//		found = true;
	//	}
	//}
	//// StrongEnemiesからも探す
	//for (auto& enemy : m_pStrongEnemies) 
	//{
	//	if (enemy->IsDead())
	//	{
	//		continue;
	//	}
	//	float distSq = VSquareSize(VSub(enemy->GetPos(), basePos));
	//	if (distSq < minDistanceSq) {
	//		minDistanceSq = distSq;
	//		nearestPos = enemy->GetPos();
	//		found = true;
	//	}
	//}

	if (m_pBossEnemy != nullptr && !m_pBossEnemy->IsDead())
	{
		float distSq = VSquareSize(VSub(m_pBossEnemy->GetPos(), basePos));
		if (m_pCamera->IsBossBattle() || distSq < minDistanceSq)
		{
			// ボスを優先、あるいは一番近ければ更新
			minDistanceSq = distSq;
			nearestPos = m_pBossEnemy->GetPos();
			//found = true;
		}
	}
	// もし「被りを避けた敵(Sub)」が見つかっていれば、そちらを優先して返す
	if (nearestPosSub.x < 1000000.0f)
	{
		//printfDx(L"nearestPosSub.x:%f\nnearestPosSub.y:%f\nnearestPosSub.z:%f\n\n", nearestPosSub.x, nearestPosSub.y, nearestPosSub.z);
		// 被りを避けた敵が最も近い敵から著しく遠い場合は無理にターゲットを分散させずに最も近い敵を狙う
		// 最も近い敵と被りを避けた敵の距離が1.5倍以上離れているなら最も近い敵を狙う 距離の2乗で比較するため、1.5倍の距離なら 2.25倍の数値で比較
		if (minDistanceSqSub > minDistanceSq * 2.25f)
		{
			// 被りを避けた敵が遠すぎるので、一番近い敵（被りあり）を返す
			return nearestPos;
		}
		return nearestPosSub;
	}
	// 別の敵が見つからなかった場合は最も近い敵を返す
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
	// 感知する範囲（これ以上離れている敵の攻撃は無視する)
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

		if (enemy->IsAttackCharge() || enemy->GetAttackInfo().active)
		{
			if (VSquareSize(VSub(enemy->GetPos(), targetPos)) < rangeSq)
			{
				return true;
			}
		}
	}

	if (m_pBossEnemy && !m_pBossEnemy->IsDead())
	{
		if (m_pBossEnemy->IsAttackCharge() || m_pBossEnemy->GetAttackInfo().active)
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
	if (m_powerUpTime > 0.0f)
	{
		m_powerUpTime = 0.0f;
		m_pPlayer->ResetPower();
		m_pCompanion->ResetPower();
	}
	CharacterBase::ControlMode currentMode = m_pPlayer->GetControlMode(); // 現在のモード
	CharacterBase::ControlMode playerNewMode; // 次のプレイヤーのモード
	// 切り替えボタンを押すと現在のモードとは別のモードに変更
	if (currentMode == CharacterBase::ControlMode::PLAYER)
	{
		playerNewMode = CharacterBase::ControlMode::COMPANION;
		m_pPlayer->ResetPower();
		m_pUIManager->SetOperationChara(false);
	}
	else
	{
		playerNewMode = CharacterBase::ControlMode::PLAYER;
		m_pPlayer->ChangePowerUp(kChangePowerUpBonus);
		m_pUIManager->SetOperationChara(true);
	}
	m_pPlayer->SetControlMode(playerNewMode);

	CharacterBase::ControlMode companionNewMode; // 次のコンパニオンのモード
	// 切り替えボタンを押すと現在のモードとは別のモードに変更
	if (playerNewMode == CharacterBase::ControlMode::PLAYER)
	{
		companionNewMode = CharacterBase::ControlMode::COMPANION;
		m_pCompanion->ResetPower();
	}
	else
	{
		companionNewMode = CharacterBase::ControlMode::PLAYER;
		m_pCompanion->ChangePowerUp(kChangePowerUpBonus);
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
	m_powerUpTime = kPowerUpTimeLimit;
}