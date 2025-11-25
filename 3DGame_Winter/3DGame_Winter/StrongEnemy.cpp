#include "StrongEnemy.h"
#include "DxLib.h"
namespace
{
	constexpr float kAttackRadius = 30.0f;
	constexpr float kColRadius = 40.0f;
	constexpr VECTOR kDefaultPos = { 500.0f,0.0f,500.0f };
	constexpr float kModelScale = 70.0f; // モデルのスケール
	constexpr float kRangeAttackRadius = 150.0f;
	constexpr float kMoveSpeed = 5.0f;
}

StrongEnemy::StrongEnemy():
	m_enemyAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_alpha(1.0f),
	m_targetAngle(0.0f),
	m_state(DEFAULT)
{
}

StrongEnemy::~StrongEnemy()
{
}

void StrongEnemy::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion)
{
	Enemy::Init(pPlayer, pCompanion);
	m_pos = kDefaultPos;
}

void StrongEnemy::End()
{
}

void StrongEnemy::Update()
{
	m_toPlayerDistance = VSize(VSub(m_pPlayer->GetPos(), m_pos));
	m_toPlayerDir = VNorm(VSub(m_pPlayer->GetPos(), m_pos));
	m_targetAngle = atan2f(m_toPlayerDir.x, m_toPlayerDir.z);
	if (m_toPlayerDistance > kColRadius)
	{
		m_pos.x += m_toPlayerDir.x * kMoveSpeed;
		m_pos.z += m_toPlayerDir.z * kMoveSpeed;
	}
}

void StrongEnemy::Draw()
{
}

void StrongEnemy::OnAttack()
{
}

void StrongEnemy::OnRangeAttack()
{
	m_enemyAttack.pos = m_pos;
	m_enemyAttack.active = true;
	m_enemyAttack.dir = VNorm(VSub(m_pPlayer->GetPos(), m_pos));
	VECTOR playerPos = m_pPlayer->GetPos();
	float distance = VSize(VSub(playerPos,m_enemyAttack.pos));
}

void StrongEnemy::OnDamage()
{
}

float StrongEnemy::GetColRadius()
{
	return kColRadius;
}

VECTOR StrongEnemy::GetDir()
{
	return VECTOR();
}

int StrongEnemy::GetStrongEnemyAttackPower()
{
	return m_attackPower;
}
