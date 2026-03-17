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
	constexpr float kAttackDuration = 200.0f;
	constexpr float kColRadius = 30.0f;
	constexpr float kMoveSpeed = 5.0f;
	constexpr float kMoveDecRate = 0.8f;
	constexpr float kAttackSpeed = 2.5f; // 弾の移動速度
	constexpr float kAcceleration = 0.2f; // 加速度（追尾する力）
	constexpr float kFriction = 0.96f;    // 空気抵抗（減速する力）
	constexpr float kMaxSpeed = 6.0f;     // 最高速度
}

FlyingEnemy::FlyingEnemy() :
	m_enemyAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_alpha(1.0f),
	m_targetAngle(0.0f),
	m_attackToPlayerDistance(0.0f),
	m_hoverTimer(0.0f),
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
	diff.y = 0.0f; // 高さを考慮しない

	m_toPlayerDistance = VSize(diff);
	m_attackToPlayerDistance = VSize(VSub(m_pCompanion->GetPos(), m_enemyAttack.pos));
	m_toPlayerDir = VNorm(diff);
	m_targetAngle = atan2f(m_toPlayerDir.x, m_toPlayerDir.z);
	if (m_toPlayerDistance > kAttackRange)
	{
		// ターゲットへの方向ベクトルを正規化
		VECTOR targetDir = VNorm(diff);

		// 現在の速度ベクトルに、ターゲットへ向かう力を加算する(直接代入しない)
		m_vec.x += targetDir.x * kAcceleration;
		m_vec.z += targetDir.z * kAcceleration;

		// 空気抵抗(減衰)をかける
		m_vec.x *= kFriction;
		m_vec.z *= kFriction;

		// 最高速度の制限(オプション）
		if (VSize(m_vec) > kMaxSpeed) {
			m_vec = VScale(VNorm(m_vec), kMaxSpeed);
		}

		// 座標更新
		m_pos = VAdd(m_pos, m_vec);

		// 向きの更新(移動方向を向くようにする)
		if (VSize(m_vec) > 0.1f) {
			m_toPlayerDir = VNorm(m_vec);
			m_targetAngle = atan2f(m_toPlayerDir.x, m_toPlayerDir.z);
		}
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
		m_enemyAttack.pos.x += m_enemyAttack.dir.x * kAttackSpeed;
		m_enemyAttack.pos.y += m_enemyAttack.dir.y * kAttackSpeed;
		m_enemyAttack.pos.z += m_enemyAttack.dir.z * kAttackSpeed;
	}
	
	if (m_attackToPlayerDistance < m_pCompanion->GetColRadius())
	{
		m_enemyAttack.active = false;
		return;
	}
	if (m_enemyAttack.timer < 0.0f)
	{
		m_enemyAttack.active = false;
		return;
	}
	//printfDx(L"m_attackToPlayerDistance:%f\n", m_attackToPlayerDistance);
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
