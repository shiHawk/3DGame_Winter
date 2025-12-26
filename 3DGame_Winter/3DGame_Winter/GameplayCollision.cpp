#include "GameplayCollision.h"
namespace
{
	constexpr float kKnockbackPower = 5.0f;
}
GameplayCollision::GameplayCollision():
	m_overLapData({ 0.0f,0.0f,0.0f },0.0f, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f },0.0f)
{
}

GameplayCollision::~GameplayCollision()
{
}

void GameplayCollision::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::vector<std::shared_ptr<NormalEnemy>>& pNormalEnemies,
	const std::vector<std::shared_ptr<StrongEnemy>>& pStrongEnemies)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	/*m_pNormalEnemy = pNormalEnemy;
	m_pStrongEnemy = pStrongEnemy;*/
	m_pNormalEnemies = pNormalEnemies;
	m_pStrongEnemies = pStrongEnemies;
}

void GameplayCollision::End()
{
}

void GameplayCollision::Update()
{
	CheckPlayerAttack();
	CheckCompanionAttack();
	CheckNormalEnemyAttack();
	//CheckStrongEnemyRangeAttack(m_pStrongEnemy->GetAttackInfo().pos, m_pStrongEnemy->GetAttackInfo().radius, m_pStrongEnemy->GetStrongEnemyAttackPower());
	//PushBackCharacter(m_pPlayer->GetPos(),m_pPlayer->GetColRadius(),m_pNormalEnemy->GetPos(),m_pNormalEnemy->GetColRadius(),m_pPlayer.get());
	//PushBackCharacter(m_pCompanion->GetPos(), m_pCompanion->GetColRadius(),m_pNormalEnemy->GetPos(),m_pNormalEnemy->GetColRadius(),m_pCompanion.get());
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
		float enemyColRadius = enemy->GetColRadius();
		hitInfo.m_deltaVector = VSub(enemy->GetPos(), m_pPlayer->GetAttackPos());
		hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
		if (hitInfo.m_distance < playerAttackRadius + enemyColRadius)
		{
			enemy->OnDamage();
			if (m_pPlayer->IsComboFinish())
			{
				// 敵を吹き飛ばすベクトルと強さを計算し、敵に適用する
				VECTOR pushDirection = VNorm(hitInfo.m_deltaVector); // 攻撃の中心から敵への方向
				//m_pNormalEnemy->ApplyKnockback(pushDirection, kKnockbackPower);
				enemy->SetIsKnockbackFlag(true);
			}
		}
	}

	//float enemyColRadius = m_pNormalEnemy->GetColRadius();
	//hitInfo.m_deltaVector = VSub(m_pNormalEnemy->GetPos(), m_pPlayer->GetAttackPos());
	//hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
	//if (hitInfo.m_distance < playerAttackRadius + enemyColRadius)
	//{
	//	m_pNormalEnemy->OnDamage();
	//	if (m_pPlayer->IsComboFinish())
	//	{
	//		// 敵を吹き飛ばすベクトルと強さを計算し、敵に適用する
	//		VECTOR pushDirection = VNorm(hitInfo.m_deltaVector); // 攻撃の中心から敵への方向
	//		//m_pNormalEnemy->ApplyKnockback(pushDirection, kKnockbackPower);
	//		m_pNormalEnemy->SetIsKnockbackFlag(true);
	//	}
	//}
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
		float enemyColRadius = enemy->GetColRadius();
		hitInfo.m_deltaVector = VSub(enemy->GetPos(), m_pCompanion->GetAttackPos());
		hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
		if (hitInfo.m_distance < companionAttackRadius + enemyColRadius)
		{
			enemy->OnDamage();
		}
	}

	/*float enemyColRadius = m_pNormalEnemy->GetColRadius();
	hitInfo.m_deltaVector = VSub(m_pNormalEnemy->GetPos(), m_pCompanion->GetAttackPos());
	hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
	if (hitInfo.m_distance < companionAttackRadius + enemyColRadius)
	{
		m_pNormalEnemy->OnDamage();
	}*/
}

void GameplayCollision::CheckNormalEnemyAttack()
{
	for (auto& enemy : m_pNormalEnemies)
	{
		if (!enemy->GetAttackInfo().active)
		{
			return; // normalEnemyが攻撃中でなければ処理を終わる
		}
		//normalEnemyの現在の攻撃情報を取得
		float normalEnemyAttackRadius = enemy->GetAttackInfo().radius;
		//HitDetectionInfo を一時的な計算用に作成
		HitDetectionInfo hitInfo;
		//プレイヤーの当たり判定情報を取得
		float playerColRadius = m_pPlayer->GetColRadius();
		hitInfo.m_deltaVector = VSub(m_pPlayer->GetPos(), enemy->GetAttackInfo().pos);
		hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
		if (hitInfo.m_distance < normalEnemyAttackRadius + playerColRadius && !m_pPlayer->IsHitFlag())
		{
			m_pPlayer->OnDamage(enemy->GetAttackPower());
		}
	}
	
	//if (!m_pNormalEnemy->GetAttackInfo().active)
	//{
	//	return; // normalEnemyが攻撃中でなければ処理を終わる
	//}
	//// normalEnemyの現在の攻撃情報を取得
	//float normalEnemyAttackRadius = m_pNormalEnemy->GetAttackInfo().radius;
	//// HitDetectionInfo を一時的な計算用に作成
	//HitDetectionInfo hitInfo;
	//// プレイヤーの当たり判定情報を取得
	//float playerColRadius = m_pPlayer->GetColRadius();
	//hitInfo.m_deltaVector = VSub(m_pPlayer->GetPos(), m_pNormalEnemy->GetAttackInfo().pos);
	//hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
	//if (hitInfo.m_distance < normalEnemyAttackRadius + playerColRadius && !m_pPlayer->IsHitFlag())
	//{
	//	m_pPlayer->OnDamage(m_pNormalEnemy->GetAttackPower());
	//}
}

void GameplayCollision::CheckStrongEnemyRangeAttack(VECTOR attackCenter, float attackRadius, int enemyPower)
{
	for (auto& enemy : m_pStrongEnemies)
	{
		if (enemy->GetAttackInfo().active)
		{
			// 攻撃の中心とプレイヤーの位置の差分ベクトルを取得
			VECTOR deltaVector = VSub(m_pPlayer->GetPos(), attackCenter);
			// 距離を計算
			float distance = VSize(deltaVector);
			float totalRadius = attackRadius + m_pPlayer->GetColRadius();
			if (distance < totalRadius && !m_pPlayer->IsHitFlag())
			{
				m_pPlayer->OnDamage(enemyPower);
				//printfDx(L"Hit\n");
			}
		}
	}

	//if (m_pStrongEnemy->GetAttackInfo().active)
	//{
	//	// 攻撃の中心とプレイヤーの位置の差分ベクトルを取得
	//	VECTOR deltaVector = VSub(m_pPlayer->GetPos(), attackCenter);
	//	// 距離を計算
	//	float distance = VSize(deltaVector);
	//	float totalRadius = attackRadius + m_pPlayer->GetColRadius();
	//	if (distance < totalRadius && !m_pPlayer->IsHitFlag())
	//	{
	//		m_pPlayer->OnDamage(enemyPower);
	//		//printfDx(L"Hit\n");
	//	}
	//}
}

void GameplayCollision::PushBackCharacter(VECTOR pos1, float pos1Radius, VECTOR pos2, float pos2Radius, CharacterBase* pTargetCharacter)
{
	m_overLapData.m_penetrationVector = VSub(pos1, pos2);
	m_overLapData.m_penetrationVectorSize = VSize(m_overLapData.m_penetrationVector);
	m_overLapData.m_overLapSize = pos1Radius + pos2Radius - m_overLapData.m_penetrationVectorSize;
	if (m_overLapData.m_overLapSize > 0.0f)
	{
		m_overLapData.m_pushDir = VNorm(m_overLapData.m_penetrationVector);
		m_overLapData.m_pushBack = VScale(m_overLapData.m_pushDir, m_overLapData.m_overLapSize);
		if (pTargetCharacter)
		{
			pTargetCharacter->AddPos(m_overLapData.m_pushBack);
		}
	}
}
