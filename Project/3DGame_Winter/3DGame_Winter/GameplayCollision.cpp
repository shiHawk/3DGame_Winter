#include "GameplayCollision.h"
namespace
{
	constexpr float kKnockbackPower = 5.0f;
	constexpr float kHitBoxScale = 0.001f;
	constexpr float kPushRate = 0.1f;
	constexpr int kSpecialGaugeIncrement = 5;
}
GameplayCollision::GameplayCollision():
	m_overLapData({ 0.0f,0.0f,0.0f },0.0f, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f },0.0f)
{
}

GameplayCollision::~GameplayCollision()
{
}

void GameplayCollision::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::vector<std::shared_ptr<NormalEnemy>>& pNormalEnemies,
	const std::vector<std::shared_ptr<StrongEnemy>>& pStrongEnemies, std::shared_ptr<BossEnemy> pBossEnemy)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	/*m_pNormalEnemy = pNormalEnemy;
	m_pStrongEnemy = pStrongEnemy;*/
	m_pNormalEnemies = pNormalEnemies;
	m_pStrongEnemies = pStrongEnemies;
	m_pBossEnemy = pBossEnemy;
}

void GameplayCollision::End()
{
}

void GameplayCollision::Update()
{
	// 毎フレーム、前フレームのヒット情報をクリアする
	ClearHitPositions();
	CheckPlayerAttack();
	CheckCompanionAttack();
	CheckNormalEnemyAttack();
	CheckStrongEnemyRangeAttack();
	CheckBossEnemyAttack();

	for (auto& enemy : m_pNormalEnemies) 
	{
		if (!enemy->IsDead()) 
		{
			PushBackCharacter(m_pPlayer.get(), enemy.get());
		}
	}
	// Strong Enemy
	for (auto& enemy : m_pStrongEnemies) 
	{
		if (!enemy->IsDead()) 
		{
			PushBackCharacter(m_pPlayer.get(), enemy.get());
		}
	}
	// Boss Enemy
	if (m_pBossEnemy && !m_pBossEnemy->IsDead()) 
	{
		PushBackCharacter(m_pPlayer.get(), m_pBossEnemy.get());
	}

	// 2. コンパニオン vs 全エネミー
	// Normal Enemy
	for (auto& enemy : m_pNormalEnemies) 
	{
		if (!enemy->IsDead()) 
		{
			PushBackCharacter(m_pCompanion.get(), enemy.get());
		}
	}
	// Strong Enemy
	for (auto& enemy : m_pStrongEnemies)
	{
		if (!enemy->IsDead()) 
		{
			PushBackCharacter(m_pCompanion.get(), enemy.get());
		}
	}
	// Boss Enemy
	if (m_pBossEnemy && !m_pBossEnemy->IsDead())
	{
		PushBackCharacter(m_pCompanion.get(), m_pBossEnemy.get());
	}
	PushBackCharacter(m_pPlayer.get(),m_pCompanion.get());
}

void GameplayCollision::Draw()
{
}

void GameplayCollision::CheckPlayerAttack()
{
	if (!m_pPlayer->IsAttackActive())
	{
		return; // プレイヤーが攻撃中でなければ処理を終わる
	}
	// プレイヤーの現在の攻撃情報を取得
	float playerAttackRadius = m_pPlayer->GetAttackRadius();
	// HitDetectionInfo を一時的な計算用に作成
	HitDetectionInfo hitInfo;
	// 敵の当たり判定情報を取得
	for (auto& enemy : m_pNormalEnemies)
	{
		if (enemy->IsDead() || enemy->GetInvincibilityByAttacker(true) > 0.0f) continue;
		float enemyColRadius = enemy->GetColRadius();
		hitInfo.m_deltaVector = VSub(enemy->GetPos(), m_pPlayer->GetAttackPos());
		hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
		if (hitInfo.m_distance < playerAttackRadius + enemyColRadius )
		{
			VECTOR normVec = VNorm(hitInfo.m_deltaVector);
			VECTOR hitPos = VAdd(m_pPlayer->GetAttackPos(), VScale(normVec, playerAttackRadius));
			m_hitPositions.push_back(hitPos); // リストに保存

			enemy->OnDamage(m_pPlayer->GetAttackPower(),true);
			m_pPlayer->AddSpecialGauge(kSpecialGaugeIncrement);
		}
	}

	for (auto& enemy : m_pStrongEnemies)
	{
		if (enemy->IsDead() || enemy->GetInvincibilityByAttacker(true) > 0.0f) continue;
		float enemyColRadius = enemy->GetColRadius();
		hitInfo.m_deltaVector = VSub(enemy->GetPos(), m_pPlayer->GetAttackPos());
		hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
		if (hitInfo.m_distance < playerAttackRadius + enemyColRadius)
		{
			VECTOR normVec = VNorm(hitInfo.m_deltaVector);
			VECTOR hitPos = VAdd(m_pPlayer->GetAttackPos(), VScale(normVec, playerAttackRadius));
			m_hitPositions.push_back(hitPos); // リストに保存
			enemy->OnDamage(m_pPlayer->GetAttackPower(),true);
			m_pPlayer->AddSpecialGauge(kSpecialGaugeIncrement);
		}
	}

	if (!m_pBossEnemy->IsDead() && m_pBossEnemy->GetInvincibilityByAttacker(true) <= 0.0f)
	{
		float bossEnemyColRadius = m_pBossEnemy->GetColRadius();
		hitInfo.m_deltaVector = VSub(m_pBossEnemy->GetPos(), m_pPlayer->GetAttackPos());
		hitInfo.m_distance = VSize(hitInfo.m_deltaVector);

		if (hitInfo.m_distance < playerAttackRadius + bossEnemyColRadius)
		{
			VECTOR normVec = VNorm(hitInfo.m_deltaVector);
			VECTOR hitPos = VAdd(m_pPlayer->GetAttackPos(), VScale(normVec, playerAttackRadius));
			m_hitPositions.push_back(hitPos); // リストに保存
			m_pBossEnemy->OnDamage(m_pPlayer->GetAttackPower(),true);
			m_pPlayer->AddSpecialGauge(kSpecialGaugeIncrement);
		}
	}
}

void GameplayCollision::CheckCompanionAttack()
{
	if (!m_pCompanion->IsAttackActive())
	{
		return; // コンパニオンが攻撃中でなければ処理を終わる
	}
	// コンパニオンの現在の攻撃情報を取得
	float companionAttackRadius = m_pCompanion->GetAttackRadius();
	// HitDetectionInfo を一時的な計算用に作成
	HitDetectionInfo hitInfo;
	// 敵の当たり判定情報を取得
	for (auto& enemy : m_pNormalEnemies)
	{
		if (enemy->IsDead() || enemy->GetInvincibilityByAttacker(false) > 0.0f) continue;
		float enemyColRadius = enemy->GetColRadius();
		hitInfo.m_deltaVector = VSub(enemy->GetPos(), m_pCompanion->GetAttackPos());
		hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
		if (hitInfo.m_distance < companionAttackRadius + enemyColRadius)
		{
			VECTOR normVec = VNorm(hitInfo.m_deltaVector);
			VECTOR hitPos = VAdd(m_pCompanion->GetAttackPos(), VScale(normVec, companionAttackRadius));
			m_hitPositions.push_back(hitPos); // リストに保存
			enemy->OnDamage(m_pCompanion->GetAttackPower(),false);
			m_pCompanion->AddSpecialGauge(kSpecialGaugeIncrement);
		}
	}

	for (auto& enemy : m_pStrongEnemies)
	{
		if (enemy->IsDead() || enemy->GetInvincibilityByAttacker(false) > 0.0f) continue;
		float enemyColRadius = enemy->GetColRadius();
		hitInfo.m_deltaVector = VSub(enemy->GetPos(), m_pCompanion->GetAttackPos());
		hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
		if (hitInfo.m_distance < companionAttackRadius + enemyColRadius)
		{
			VECTOR normVec = VNorm(hitInfo.m_deltaVector);
			VECTOR hitPos = VAdd(m_pCompanion->GetAttackPos(), VScale(normVec, companionAttackRadius));
			m_hitPositions.push_back(hitPos); // リストに保存
			enemy->OnDamage(m_pCompanion->GetAttackPower(),false);
			m_pCompanion->AddSpecialGauge(kSpecialGaugeIncrement);
		}
	}

	if (!m_pBossEnemy->IsDead() && m_pBossEnemy->GetInvincibilityByAttacker(false) <= 0.0f)
	{
		float bossEnemyColRadius = m_pBossEnemy->GetColRadius();

		hitInfo.m_deltaVector = VSub(m_pBossEnemy->GetPos(), m_pCompanion->GetAttackPos());
		hitInfo.m_distance = VSize(hitInfo.m_deltaVector);

		if (hitInfo.m_distance < companionAttackRadius + bossEnemyColRadius)
		{
			VECTOR normVec = VNorm(hitInfo.m_deltaVector);
			VECTOR hitPos = VAdd(m_pCompanion->GetAttackPos(), VScale(normVec, companionAttackRadius));
			m_hitPositions.push_back(hitPos); // リストに保存
			m_pBossEnemy->OnDamage(m_pCompanion->GetAttackPower(),false);
			m_pCompanion->AddSpecialGauge(kSpecialGaugeIncrement);
		}
	}
}

void GameplayCollision::CheckNormalEnemyAttack()
{
	for (auto& enemy : m_pNormalEnemies)
	{
		if (!enemy->GetAttackInfo().active)
		{
			continue; // normalEnemyが攻撃中でなければ処理を終わる
		}

		// 共通の攻撃情報を取得
		float enemyAttackRadius = enemy->GetAttackInfo().radius;
		VECTOR enemyAttackPos = enemy->GetAttackInfo().pos;
		int attackPower = enemy->GetAttackPower();
		HitDetectionInfo hitInfo;

		// プレイヤーへの当たり判定
		// プレイヤーがダメージ中ではなく、かつ無敵時間でもない場合のみチェック
		if (!m_pPlayer->IsHitFlag() && m_pPlayer->GetInvincibilityTimer() <= 0.0f)
		{
			hitInfo.m_deltaVector = VSub(m_pPlayer->GetPos(), enemyAttackPos);
			hitInfo.m_distance = VSize(hitInfo.m_deltaVector);

			if (hitInfo.m_distance < enemyAttackRadius + m_pPlayer->GetColRadius())
			{
				VECTOR normVec = VNorm(hitInfo.m_deltaVector);
				VECTOR hitPos = VAdd(enemyAttackPos, VScale(normVec, enemyAttackRadius));
				m_hitPositions.push_back(hitPos); // リストに保存
				m_pPlayer->OnDamage(attackPower);
			}
		}

		// コンパニオンへの当たり判定
		// コンパニオンが無敵状態（ダメージ中含む）でない場合のみチェック
		if (!m_pCompanion->IsHitFlag() && m_pCompanion->GetInvincibilityTimer() <= 0.0f)
		{
			hitInfo.m_deltaVector = VSub(m_pCompanion->GetPos(), enemyAttackPos);
			hitInfo.m_distance = VSize(hitInfo.m_deltaVector);

			if (hitInfo.m_distance < enemyAttackRadius + m_pCompanion->GetColRadius())
			{
				VECTOR normVec = VNorm(hitInfo.m_deltaVector);
				VECTOR hitPos = VAdd(enemyAttackPos, VScale(normVec, enemyAttackRadius));
				m_hitPositions.push_back(hitPos); // リストに保存
				m_pCompanion->OnDamage(attackPower);
			}
		}
	}
}

void GameplayCollision::CheckStrongEnemyRangeAttack()
{
	for (auto& enemy : m_pStrongEnemies)
	{
		if (!enemy->GetAttackInfo().active)
		{
			continue; // normalEnemyが攻撃中でなければ処理を終わる
		}
		// 共通の攻撃情報を取得
		float enemyAttackRadius = enemy->GetAttackInfo().radius;
		VECTOR enemyAttackPos = enemy->GetAttackInfo().pos;
		int attackPower = enemy->GetAttackPower();
		HitDetectionInfo hitInfo;

		if (!m_pPlayer->IsHitFlag() && m_pPlayer->GetInvincibilityTimer() <= 0.0f)
		{
			hitInfo.m_deltaVector = VSub(m_pPlayer->GetPos(), enemyAttackPos);
			hitInfo.m_distance = VSize(hitInfo.m_deltaVector);

			if (hitInfo.m_distance < enemyAttackRadius + m_pPlayer->GetColRadius())
			{
				VECTOR normVec = VNorm(hitInfo.m_deltaVector);
				VECTOR hitPos = VAdd(enemyAttackPos, VScale(normVec, enemyAttackRadius));
				m_hitPositions.push_back(hitPos); // リストに保存
				m_pPlayer->OnDamage(attackPower);
			}
		}

		if (!m_pCompanion->IsHitFlag() && m_pCompanion->GetInvincibilityTimer() <= 0.0f)
		{
			hitInfo.m_deltaVector = VSub(m_pCompanion->GetPos(), enemyAttackPos);
			hitInfo.m_distance = VSize(hitInfo.m_deltaVector);

			if (hitInfo.m_distance < enemyAttackRadius + m_pCompanion->GetColRadius())
			{
				VECTOR normVec = VNorm(hitInfo.m_deltaVector);
				VECTOR hitPos = VAdd(enemyAttackPos, VScale(normVec, enemyAttackRadius));
				m_hitPositions.push_back(hitPos); // リストに保存
				m_pCompanion->OnDamage(attackPower);
			}
		}
	}
}

void GameplayCollision::CheckBossEnemyAttack()
{
	if (m_pBossEnemy->GetAttackInfo().active)
	{
		// 共通の攻撃情報を取得
		float enemyAttackRadius = m_pBossEnemy->GetAttackInfo().radius;
		VECTOR enemyAttackPos = m_pBossEnemy->GetAttackInfo().pos;
		int attackPower = m_pBossEnemy->GetAttackPower();
		HitDetectionInfo hitInfo;

		if (!m_pPlayer->IsHitFlag() && m_pPlayer->GetInvincibilityTimer() <= 0.0f)
		{
			hitInfo.m_deltaVector = VSub(m_pPlayer->GetPos(), enemyAttackPos);
			hitInfo.m_distance = VSize(hitInfo.m_deltaVector);

			if (hitInfo.m_distance < enemyAttackRadius + m_pPlayer->GetColRadius())
			{
				VECTOR normVec = VNorm(hitInfo.m_deltaVector);
				VECTOR hitPos = VAdd(enemyAttackPos, VScale(normVec, enemyAttackRadius));
				m_hitPositions.push_back(hitPos); // リストに保存
				m_pPlayer->OnDamage(attackPower);
			}
		}

		if (!m_pCompanion->IsHitFlag() && m_pCompanion->GetInvincibilityTimer() <= 0.0f)
		{
			hitInfo.m_deltaVector = VSub(m_pCompanion->GetPos(), enemyAttackPos);
			hitInfo.m_distance = VSize(hitInfo.m_deltaVector);

			if (hitInfo.m_distance < enemyAttackRadius + m_pCompanion->GetColRadius())
			{
				VECTOR normVec = VNorm(hitInfo.m_deltaVector);
				VECTOR hitPos = VAdd(enemyAttackPos, VScale(normVec, enemyAttackRadius));
				m_hitPositions.push_back(hitPos); // リストに保存
				m_pCompanion->OnDamage(attackPower);
			}
		}
	}
}

void GameplayCollision::PushBackCharacter(CharacterBase* pChar1, CharacterBase* pChar2)
{
	// どちらかがいない、または死んでいる場合は処理しない
	if (!pChar1 || !pChar2) return;
	 if (pChar1->IsDead() || pChar2->IsDead()) return; 

	// 1. 各キャラクターの情報を取得
	VECTOR pos1 = pChar1->GetPos();
	float radius = pChar1->GetColRadius();
	VECTOR pos2 = pChar2->GetPos();
	float radius2 = pChar2->GetColRadius();

	// 2. めり込み判定
	VECTOR vec = VSub(pos1, pos2);   // 2から1へのベクトル
	float dist = VSize(vec);         // 距離
	float totalRadius = radius + radius2; // 半径の合計

	// めり込んでいる場合 (距離 < 半径の合計)
	if (dist < totalRadius && dist > 0.0001f) // 0除算防止
	{
		float overlap = totalRadius - dist; // めり込んでいる量
		VECTOR pushDir = VNorm(vec);        // 押し戻す方向 (2 -> 1)
		//float pushPower = overlap * kPushRate;
		// 3. 押し戻し処理
		// ここで「誰が」「どれくらい」動くかを決める

		// BossEnemy判定 (Bossは動かない)
		bool isChar1Boss = (dynamic_cast<BossEnemy*>(pChar1) != nullptr);
		bool isChar2Boss = (dynamic_cast<BossEnemy*>(pChar2) != nullptr);

		if (isChar2Boss)
		{
			// 相手がボスなら、自分(Char1)が100%押し出される
			VECTOR pushVec = VScale(pushDir, overlap);
			pChar1->AddPos(pushVec);
		}
		else if (isChar1Boss)
		{
			// 自分がボスなら、相手(Char2)が100%押し出される (方向は逆)
			VECTOR pushVec = VScale(pushDir, -overlap);
			pChar2->AddPos(pushVec);
		}
		else
		{
			// 通常同士、またはPlayer vs Enemyなら、お互いに半分ずつ押し合う (50:50)
			// ※片方だけ動かすと、壁際などでプルプル震える原因になる
			VECTOR pushVec1 = VScale(pushDir, overlap * 0.5f);
			VECTOR pushVec2 = VScale(pushDir, -overlap * 0.5f); // 逆方向

			pChar1->AddPos(pushVec1);
			pChar2->AddPos(pushVec2);
		}
	}
}
