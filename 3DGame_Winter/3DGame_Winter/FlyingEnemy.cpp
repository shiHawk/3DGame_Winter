#include "FlyingEnemy.h"
#include <cmath>
namespace
{
	constexpr float kAttackRadius = 30.0f;
	constexpr VECTOR kDefaultPos = { 600.0f,200.0f,500.0f };
	constexpr float kSphereRadius = 40.0f;
	constexpr float kAttackRange = 200.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0xfffff0;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kAttackDuration = 80.0f;
	constexpr float kColRadius = 30.0f;
	constexpr float kMoveSpeed = 5.0f;
	constexpr float kMoveDecRate = 0.8f;
	constexpr float kAttackSpeed = 2.5f; // ˆÚ“®‘¬“x
}

FlyingEnemy::FlyingEnemy() :
	m_enemyAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_alpha(1.0f),
	m_targetAngle(0.0f),
	m_toPlayerDir({ 0.0f,0.0f,0.0f }),
	m_attackDir({ 0.0f,0.0f,0.0f })
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
	VECTOR diff = VSub(m_pCompanion->GetPos(), m_pos);
	diff.y = 0.0f; // ‚‚³‚ðl—¶‚µ‚È‚¢
	m_toPlayerDistance = VSize(diff);
	m_toPlayerDir = VNorm(diff);
	m_targetAngle = atan2f(m_toPlayerDir.x, m_toPlayerDir.z);
	if (m_toPlayerDistance > kAttackRange)
	{
		m_pos.x += m_toPlayerDir.x * kMoveSpeed * kMoveDecRate;
		m_pos.z += m_toPlayerDir.z * kMoveSpeed * kMoveDecRate;
	}
	else
	{
		if (!m_enemyAttack.active)
		{
			OnAttack();
		}
	}
	if (m_enemyAttack.active)
	{
		m_enemyAttack.timer--;
		m_enemyAttack.pos.x += m_enemyAttack.dir.x * kAttackSpeed * kMoveDecRate;
		m_enemyAttack.pos.y += m_enemyAttack.dir.y * kAttackSpeed * kMoveDecRate;
		m_enemyAttack.pos.z += m_enemyAttack.dir.z * kAttackSpeed * kMoveDecRate;
	}
	if (m_enemyAttack.timer < 0.0f)
	{
		m_enemyAttack.active = false;
	}
}

void FlyingEnemy::Draw()
{
	DrawSphere3D(m_pos, kSphereRadius, kDivNum, kSphereDifColor, kSphereSpcColor,true);
	if (m_enemyAttack.active)
	{
		DrawSphere3D(m_enemyAttack.pos, 20.0f, kDivNum, 0xff0000, kSphereSpcColor, true);
	}
}

void FlyingEnemy::OnAttack()
{
	m_enemyAttack.active = true;
	m_enemyAttack.pos = m_pos;
	m_attackDir = VSub(m_pCompanion->GetPos(),m_pos);
	m_enemyAttack.dir = VNorm(m_attackDir);
	m_enemyAttack.timer = kAttackDuration;
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
