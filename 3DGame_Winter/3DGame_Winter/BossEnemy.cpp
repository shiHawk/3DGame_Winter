#include "BossEnemy.h"
namespace
{
	constexpr float kAttackRadius = 30.0f;
	constexpr float kColRadius = 25.0f;
	constexpr VECTOR kDefaultPos = { 500.0f,-59.0f,500.0f };
	constexpr float kModelScale = 70.0f; // モデルのスケール
	constexpr float kChageTime = 1.5f;
	constexpr float kCoolDownTime = 1.5f;
	constexpr float kRangeAttackRadius = 150.0f;
	constexpr float kRangeAttackDuration = 30.0f;
	constexpr float kTrackingRange = 400.0f;
	constexpr float kActionCheckInterval = 0.5f; // 抽選頻度
	constexpr float kNormalAttackDuration = 20.0f;
	constexpr float kNormalAttackRange = 90.0f;
	constexpr float kMoveSpeed = 5.0f;
	// 各アニメーション番号
	constexpr int kIdleAnimNo = 9;
	constexpr int kWalkAnimNo = 15;
	constexpr int kAttackAnimNo = 13;
	constexpr int kRangeAttackAnimNo = 0;
	constexpr int kDamageAnimNo = 8;
	// アニメーション速度
	constexpr float kWalkAnimIncrement = 0.15f; // 歩行アニメーションの再生速度
	constexpr float kIdleAnimIncrement = 0.4f; // 待機アニメーションの再生速度
	constexpr float kAttackAnimIncrement = 0.5f; // 攻撃アニメーションの再生速度
	constexpr float kDamageAnimIncrement = 0.6f; // 被弾アニメーションの再生速度

	constexpr int kRandMax = 100;
	constexpr int kRangeAttackProbability = 30;
	constexpr float kFramesPerSecond = 60.0f; // 秒数変換
	constexpr unsigned int kAreaColor = 0xff4500;
	constexpr unsigned int kOutLineColor = 0xff0000;
	constexpr int kDivNum = 32;
	constexpr float kInvincibilityTime = 30.0f;
	constexpr int kMaxHp = 400;
}

BossEnemy::BossEnemy():
	m_state(BossEnemyState::DEFAULT),
	m_enemyAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_alpha(0.0f),
	m_targetAngle(0.0f),
	m_attackTimer(0.0f),
	m_actionCheckTimer(0.0f)
{
}

BossEnemy::~BossEnemy()
{
}

void BossEnemy::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, VECTOR pos)
{
}

void BossEnemy::End()
{
}

void BossEnemy::Update()
{
}

void BossEnemy::Draw()
{
}

void BossEnemy::OnAttack()
{
}

void BossEnemy::OnRangeAttack()
{
}

void BossEnemy::OnDamage()
{
}

float BossEnemy::GetColRadius()
{
	return 0.0f;
}

VECTOR BossEnemy::GetDir()
{
	return VECTOR();
}

int BossEnemy::GetStrongEnemyAttackPower()
{
	return 0;
}
