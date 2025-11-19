#include "FlyingEnemy.h"
#include <cmath>
namespace
{
	constexpr float kAttackRadius = 30.0f;
	constexpr VECTOR kDefaultPos = { 600.0f,200.0f,500.0f };
	constexpr float kSphereRadius = 40.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x000000;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kColRadius = 30.0f;
	constexpr float kMoveSpeed = 4.0f;
	constexpr float kMoveDecRate = 0.8f;
}

FlyingEnemy::FlyingEnemy() :
	m_enemyAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_alpha(1.0f),
	m_targetAngle(0.0f)
{
}

FlyingEnemy::~FlyingEnemy()
{
}

void FlyingEnemy::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion)
{
	Enemy::Init(pPlayer, pCompanion);
	m_pos = kDefaultPos;
}

void FlyingEnemy::End()
{
}

void FlyingEnemy::Update()
{
	m_toPlayerDistance = VSize(VSub(m_pPlayer->GetPos(), m_pos));
	m_toPlayerDir = VNorm(VSub(m_pPlayer->GetPos(), m_pos));
	m_targetAngle = atan2f(m_toPlayerDir.x, m_toPlayerDir.z);
	if (m_toPlayerDistance > kSphereRadius)
	{
		m_pos.x += m_toPlayerDir.x * kMoveSpeed * kMoveDecRate;
		m_pos.z += m_toPlayerDir.z * kMoveSpeed * kMoveDecRate;
	}
}

void FlyingEnemy::Draw()
{
	DrawSphere3D(m_pos, kSphereRadius, kDivNum, kSphereDifColor, kSphereSpcColor,true);
}

void FlyingEnemy::OnAttack()
{
}

void FlyingEnemy::OnDamage()
{
}

float FlyingEnemy::GetColRadius()
{
	return 0.0f;
}

VECTOR FlyingEnemy::GetDir()
{
	return VECTOR();
}
