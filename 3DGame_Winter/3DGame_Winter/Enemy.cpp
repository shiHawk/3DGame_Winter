#include "Enemy.h"
namespace
{
	constexpr VECTOR kDefaultPos = { 600.0f,0.0f,500.0f };
	constexpr float kSphereRadius = 30.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x000000;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 5.0f;
	constexpr float kMoveDecRate = 0.8f;
}

Enemy::Enemy():
m_toPlayerDir({0.0f,0.0f,0.0f}),
m_toPlayerDistance(0.0f),
m_toCompanionDir({ 0.0f,0.0f,0.0f }),
m_toCompanionDistance(0.0f),
m_AttackCoolTime(0.0f)
{
}

void Enemy::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion)
{
	m_pPlayer = pPlayer;
}

void Enemy::End()
{
}

void Enemy::OnAttack()
{

}
