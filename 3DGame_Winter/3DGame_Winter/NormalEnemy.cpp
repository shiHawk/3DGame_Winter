#include "NormalEnemy.h"
namespace
{
	constexpr VECTOR kSpherePos = { 600.0f,0.0f,500.0f };
	constexpr float kSphereRadius = 30.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x000000;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 5.0f;
	constexpr float kMoveDecRate = 0.8f;
}

NormalEnemy::NormalEnemy()
{
}

NormalEnemy::~NormalEnemy()
{
}

void NormalEnemy::Init(std::shared_ptr<Player> pPlayer)
{
	Enemy::Init(pPlayer);
	m_pos = kSpherePos;
}

void NormalEnemy::End()
{
}

void NormalEnemy::Update()
{
	m_toPlayerDistance = VSize(VSub(m_pPlayer->GetPos(), m_pos));
	m_toPlayerDir = VNorm(VSub(m_pPlayer->GetPos(), m_pos));
	if (m_toPlayerDistance > kSphereRadius)
	{
		m_pos.x += m_toPlayerDir.x * kMoveSpeed * kMoveDecRate;
		m_pos.z += m_toPlayerDir.z * kMoveSpeed * kMoveDecRate;
	}
}

void NormalEnemy::Draw()
{
	DrawSphere3D(m_pos, kSphereRadius, kDivNum, kSphereDifColor, kSphereSpcColor, true);
}
