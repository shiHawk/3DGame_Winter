#include "Enemy.h"
namespace
{
	constexpr VECTOR kDefaultPos = { 600.0f,0.0f,500.0f };
	constexpr VECTOR kInvalidValuePos = {10000.0f,10000.0f,10000.0f};
	constexpr float kSphereRadius = 30.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x000000;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 5.0f;
	constexpr float kMoveDecRate = 0.8f;
	constexpr float kHateDecayRate = 0.99f;
	constexpr float kDistanceWeight = 1000.0f;
}

Enemy::Enemy():
m_toPlayerDir({0.0f,0.0f,0.0f}),
m_toPlayerDistance(0.0f),
m_toCompanionDir({ 0.0f,0.0f,0.0f }),
m_toCompanionDistance(0.0f),
m_AttackCoolTime(0.0f),
m_targetPos({ 0.0f,0.0f,0.0f }),
m_playerHate(0),
m_companionHate(0),
m_isAttackCharge(false)
{
}

void Enemy::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
}

void Enemy::End()
{
}

void Enemy::OnAttack()
{

}

void Enemy::OnDamage(int damage, bool isHatePlayer)
{
}

void Enemy::SearchTarget()
{
	// プレイヤーとの距離
	float distToPlayer = VSize(VSub(m_pPlayer->GetPos(), m_pos));
	// コンパニオンとの距離
	float distToCompanion = VSize(VSub(m_pCompanion->GetPos(), m_pos));
	// ヘイト値の計算　敵との距離が近いほどヘイト値が上がりやすくなる
	float playerWeight = m_playerHate + (kDistanceWeight / (distToPlayer + 1.0f));
	float companionWeight = m_companionHate + (kDistanceWeight / (distToCompanion + 1.0f));

	if (m_pPlayer->IsDead()) playerWeight = -1.0f;
	if (m_pCompanion->IsDead()) companionWeight = -1.0f;

	// ヘイトが高いほうをターゲットにする
	// OnDamage関数が呼ばれたときにもヘイト値が上昇する
	if (playerWeight > companionWeight) 
	{
		m_targetPos = m_pPlayer->GetPos();
	}
	else 
	{
		m_targetPos = m_pCompanion->GetPos();
	}

	// ヘイトの自然減衰
	m_playerHate *= kHateDecayRate;
	m_companionHate *= kHateDecayRate;
	

	if (m_pPlayer->IsDead() && m_pCompanion->IsDead())
	{
		m_targetPos = kInvalidValuePos;
	}
}
