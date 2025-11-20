#include "GameplayCollision.h"

GameplayCollision::GameplayCollision():
	m_overLapData({ 0.0f,0.0f,0.0f },0.0f, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f },0.0f)
{
}

GameplayCollision::~GameplayCollision()
{
}

void GameplayCollision::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<NormalEnemy> pNormalEnemy)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	m_pNormalEnemy = pNormalEnemy;
}

void GameplayCollision::End()
{
}

void GameplayCollision::Update()
{
	CheckPlayerAttack();
	ChaeckCompanionAttack();
	PushBackCharacter(m_pPlayer->GetPos(),m_pPlayer->GetColRadius(),m_pNormalEnemy->GetPos(),m_pNormalEnemy->GetColRadius(),m_pPlayer.get());
	PushBackCharacter(m_pCompanion->GetPos(), m_pCompanion->GetColRadius(),m_pNormalEnemy->GetPos(),m_pNormalEnemy->GetColRadius(),m_pCompanion.get());
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
	// コンパニオンの現在の攻撃情報を取得
	float playerAttackRadius = m_pPlayer->GetAttackRadius();
	// HitDetectionInfo を一時的な計算用に作成
	HitDetectionInfo hitInfo;
	// 敵の当たり判定情報を取得
	float enemyColRadius = m_pNormalEnemy->GetColRadius();
	hitInfo.m_deltaVector = VSub(m_pNormalEnemy->GetPos(), m_pPlayer->GetAttackPos());
	hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
	if (hitInfo.m_distance < playerAttackRadius + enemyColRadius)
	{
		m_pNormalEnemy->OnDamage();
	}
}

void GameplayCollision::ChaeckCompanionAttack()
{
	if (!m_pCompanion->IsAttackActive())
	{
		return; // プレイヤーが攻撃中でなければ処理を終わる
	}
	// プレイヤーの現在の攻撃情報を取得
	float companionAttackRadius = m_pCompanion->GetAttackRadius();
	// HitDetectionInfo を一時的な計算用に作成
	HitDetectionInfo hitInfo;
	// 敵の当たり判定情報を取得
	float enemyColRadius = m_pNormalEnemy->GetColRadius();
	hitInfo.m_deltaVector = VSub(m_pNormalEnemy->GetPos(), m_pCompanion->GetAttackPos());
	hitInfo.m_distance = VSize(hitInfo.m_deltaVector);
	if (hitInfo.m_distance < companionAttackRadius + enemyColRadius)
	{
		m_pNormalEnemy->OnDamage();
	}
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
