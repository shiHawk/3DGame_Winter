#include "BattleAreaManager.h"
namespace
{
	constexpr int kDivNum = 16;
	constexpr float kBattleAreaRadiusSize = 600.0f;
	constexpr float kBossBattleAreaRadiusSize = 1500.0f;
	constexpr float kBattleTriggerDistance = 300.0f;
	constexpr int kSegmentCount = 64;    // 円周を分割する数
	constexpr float kLineWidth = 10.0f; // ラインの太さ
	// 光るエフェクト用定数
	constexpr float kGlowSpeed = 0.005f;    // 明滅スピード
	constexpr float kGlowMinOffset = 0.5f;      // sin波を0～1に補正するためのオフセット
	constexpr float kGlowAmplitude = 0.5f;      // sin波の振幅
	constexpr float kColorMaxValue = 255.0f;    // 8bitカラー最大値

	// 水色の固定ベースカラー
	constexpr int kBaseRed = 50;                 // 基本赤成分
	constexpr int kBaseGreen = 200;              // 基本緑成分
	constexpr int kBaseBlue = 200;               // 基本青成分（明滅でここから加算）

	// 円形分割用
	constexpr float kTwoPi = DX_PI_F * 2.0f; // 2π
}

BattleAreaManager::BattleAreaManager():
	m_battleState(State::None),
	m_battleAreaCenterPos({0.0f,0.0f,0.0f}),
	m_battleAreaRadius(0.0f),
	m_battleTriggerDistance(0.0f),
	m_distToNormal(0.0f),
	m_distToStrong(0.0f),
	m_battleAreaCenterPosToPlayerDistance(0.0f),
	m_afterCorrectionPos({ 0.0f,0.0f,0.0f }),
	m_batlleAreaActive(false)
{
}

BattleAreaManager::~BattleAreaManager()
{
}

void BattleAreaManager::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	m_battleAreaCenterPos = { 0.0f,0.0f,0.0f };
	m_battleAreaRadius = kBattleAreaRadiusSize;
	m_battleTriggerDistance = kBattleTriggerDistance;
	m_battleState = State::None;
	// アクティブリストの初期化
	m_activeNormalenemies.clear();
	m_activeStrongEnemies.clear();
}

void BattleAreaManager::SetEnemy(std::vector<std::shared_ptr<NormalEnemy>>& normalenemies, std::vector<std::shared_ptr<StrongEnemy>>& strongEnemies, std::shared_ptr<BossEnemy> pBossEnemy)
{
	m_normalenemies = normalenemies;
	m_strongEnemies = strongEnemies;
	m_pBossEnemy = pBossEnemy;
}

void BattleAreaManager::Update(std::shared_ptr<CharacterBase> activeCharacter, std::vector<std::shared_ptr<NormalEnemy>>& normalEnemies, std::vector<std::shared_ptr<StrongEnemy>>& strongEnemies)
{
	// 1. 戦闘終了状態の処理
	if (m_battleState == State::Finish)
	{
		// 敵から十分に離れたら、再び戦闘可能な状態(None)に戻す
		// CheckEncounterが false (=近くに敵がいない) ならリセット
		if (!CheckEncounter(activeCharacter, normalEnemies, strongEnemies))
		{
			m_batlleAreaActive = false;
			m_battleState = State::None;
		}
	}
	// 2. 通常探索状態の処理
	else if (m_battleState == State::None)
	{
		if (CheckEncounter(activeCharacter, normalEnemies, strongEnemies))
		{
			m_batlleAreaActive = true;

			// ボスが近くにいるか確認して、引数を切り替える
			float distToBoss = VSize(VSub(m_pBossEnemy->GetPos(), activeCharacter->GetPos()));

			if (!m_pBossEnemy->IsDead() && distToBoss <= 1445.0f)
			{
				// ボス戦開始：中心はボス、半径は大きく
				EnterBattle(m_pBossEnemy->GetPos(), normalEnemies, strongEnemies, kBossBattleAreaRadiusSize);
			}
			else
			{
				// 通常戦開始：中心はプレイヤー、半径はデフォルト
				EnterBattle(activeCharacter->GetPos(), normalEnemies, strongEnemies,kBattleAreaRadiusSize);
			}
		}
	}
	// 3. 戦闘中の処理
	else if (m_battleState == State::InBattle)
	{
		// プレイヤーとコンパニオン、両方の移動を制限する
		ConstraintCharacterMovement(m_pPlayer);
		ConstraintCharacterMovement(m_pCompanion);

		// バトルが終わっているかをチェック
		CheckBattleEnd();
	}
}

void BattleAreaManager::DebugDraw()
{
	if (m_battleState == State::InBattle)
	{
		DrawSphere3D(m_battleAreaCenterPos, m_battleAreaRadius, kDivNum, 0xff00ff, 0xffffff, false);
	}
}

bool BattleAreaManager::IsInBattle()
{
	return m_battleState == State::InBattle;
}

bool BattleAreaManager::IsFinished()
{
	return m_battleState == State::Finish;
}

void BattleAreaManager::DrawBattleAreaBodary()
{
	if (m_battleState == State::InBattle)
	{
		// 明滅する輝き計算
		float glow = (sinf(GetNowCount() * kGlowSpeed) * kGlowAmplitude + kGlowMinOffset) * kColorMaxValue;
		int glowColor = GetColor(kBaseRed, kBaseGreen, kBaseBlue + static_cast<int>(glow));

		for (int i = 0; i < kSegmentCount; ++i)
		{
			// 2点分の角度を計算
			float angle1 = kTwoPi * i / kSegmentCount;
			float angle2 = kTwoPi * (i + 1) / kSegmentCount;

			// 円周上の2点を計算
			VECTOR p1 = VGet(
				m_battleAreaCenterPos.x + cosf(angle1) * m_battleAreaRadius,
				m_battleAreaCenterPos.y,
				m_battleAreaCenterPos.z + sinf(angle1) * m_battleAreaRadius
			);

			VECTOR p2 = VGet(
				m_battleAreaCenterPos.x + cosf(angle2) * m_battleAreaRadius,
				m_battleAreaCenterPos.y,
				m_battleAreaCenterPos.z + sinf(angle2) * m_battleAreaRadius
			);

			// 境界線を描画
			DrawLine3D(p1, p2, glowColor);
		}
	}
}

bool BattleAreaManager::CheckEncounter(std::shared_ptr<CharacterBase> activeCharacter, const std::vector<std::shared_ptr<NormalEnemy>>& normalEnemies, const std::vector<std::shared_ptr<StrongEnemy>>& strongEnemies)
{
	VECTOR charaPos = activeCharacter->GetPos();

	// NormalEnemyとの距離チェック
	for (const auto& enemy : normalEnemies)
	{
		if (enemy->IsDead()) continue;

		float distSq = VSize(VSub(enemy->GetPos(), charaPos));
		if (distSq < m_battleTriggerDistance)
		{
			return true; // 接近した
		}
	}

	// StrongEnemyとの距離チェック
	for (const auto& enemy : strongEnemies)
	{
		if (enemy->IsDead()) continue;

		float distSq = VSize(VSub(enemy->GetPos(), charaPos));
		if (distSq < m_battleTriggerDistance)
		{
			return true; // 接近した
		}
	}

	if (m_pBossEnemy && !m_pBossEnemy->IsDead())
	{
		// ボス戦の感知範囲（少し広めにとるのがおすすめ）
		// GameScene側で1445.0fで判定しているので、それに合わせるか、クラス内の定数を使います
		float distSq = VSize(VSub(m_pBossEnemy->GetPos(), charaPos));
		if (distSq < 1445.0f) // 数値は調整してください
		{
			return true; // ボスに接近した
		}
	}

	return false; // 誰も近くにいない
}

void BattleAreaManager::EnterBattle(const VECTOR& centerPos, 
									const std::vector<std::shared_ptr<NormalEnemy>>& normalEnemies, 
									const std::vector<std::shared_ptr<StrongEnemy>>& strongEnemies,
									float radius)
{
	m_battleState = State::InBattle;

	// 戦闘エリアの中心を設定（Y軸は0固定あるいはキャラの高さに合わせる）
	// ここではシンプルにY=0平面でエリアを作る想定にします
	m_battleAreaCenterPos = centerPos;
	m_battleAreaRadius = radius;
	// 以前のアクティブリストをクリア
	m_activeNormalenemies.clear();
	m_activeStrongEnemies.clear();

	// バトルエリア内（あるいはその付近）にいる敵だけをアクティブリストに入れる
	// ※エリア半径より少し広めに取って、ギリギリの敵も巻き込むようにしています
	float searchRadius = m_battleAreaRadius + 100.0f;

	for (const auto& enemy : normalEnemies)
	{
		if (!enemy->IsDead() && VSize(VSub(enemy->GetPos(), m_battleAreaCenterPos)) < searchRadius)
		{
			m_activeNormalenemies.push_back(enemy);
		}
	}

	for (const auto& enemy : strongEnemies)
	{
		if (!enemy->IsDead() && VSize(VSub(enemy->GetPos(), m_battleAreaCenterPos)) < searchRadius)
		{
			m_activeStrongEnemies.push_back(enemy);
		}
	}
}

void BattleAreaManager::ConstraintCharacterMovement(std::shared_ptr<CharacterBase> character)
{
	VECTOR charaPos = character->GetPos();

	// Y座標の差を無視したベクトルを作成
	VECTOR toCharaXZ = VSub(charaPos, m_battleAreaCenterPos);
	toCharaXZ.y = 0.0f;

	float distXZ = VSize(toCharaXZ);

	// バトルエリア（円柱）から出ようとしたら
	if (distXZ > m_battleAreaRadius)
	{
		// 中心からそのキャラへの向き（水平方向のみ）
		VECTOR dirXZ = VNorm(toCharaXZ);

		// バトルエリアの縁の座標を計算
		VECTOR correctPos = VAdd(m_battleAreaCenterPos, VScale(dirXZ, m_battleAreaRadius));

		// 高さを現在のキャラクターの状態に合わせる
		correctPos.y = charaPos.y;

		character->SetPos(correctPos);
	}
}

void BattleAreaManager::CheckBattleEnd()
{
	// アクティブなNormalEnemyが残っているかチェック
	for (const auto& enemy : m_activeNormalenemies)
	{
		if (!enemy->IsDead()) return; // まだ生きている
	}

	// アクティブなStrongEnemyが残っているかチェック
	for (const auto& enemy : m_activeStrongEnemies)
	{
		if (!enemy->IsDead()) return; // まだ生きている
	}

	if (m_pBossEnemy && !m_pBossEnemy->IsDead())
	{
		// ボスがバトルエリア内にいるなら戦闘続行
		float dist = VSize(VSub(m_pBossEnemy->GetPos(), m_battleAreaCenterPos));
		if (dist < m_battleAreaRadius + 100.0f)
		{
			return; // ボスがまだ生きている
		}
	}
	// 全滅していたら終了状態へ
	m_battleState = State::Finish;
}
