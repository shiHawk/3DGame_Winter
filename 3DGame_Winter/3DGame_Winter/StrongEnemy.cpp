#include "StrongEnemy.h"
#include "DxLib.h"
namespace
{
	constexpr float kAttackRadius = 30.0f;
	constexpr float kColRadius = 25.0f;
	constexpr VECTOR kDefaultPos = { 500.0f,0.0f,500.0f };
	constexpr float kModelScale = 70.0f; // ÉÇÉfÉãÇÃÉXÉPÅ[Éã
	constexpr float kChageTime = 1.5f;
	constexpr float kCoolDownTime = 1.5f;
	constexpr float kRangeAttackRadius = 150.0f;
	constexpr float kRangeAttackDuration = 30.0f;
	constexpr float kActionCheckInterval = 0.5f; // íäëIïpìx
	constexpr float kNormalAttackDuration = 20.0f;
	constexpr float kNormalAttackRange = 90.0f;
	constexpr float kMoveSpeed = 5.0f;
	constexpr int kRandMax = 100;
	constexpr int kRangeAttackProbability = 30;
	constexpr float kFramesPerSecond = 60.0f; // ïbêîïœä∑
	constexpr unsigned int kAreaColor = 0xff4500;
	constexpr unsigned int kOutLineColor = 0xff0000;
	constexpr int kDivNum = 32;
}

StrongEnemy::StrongEnemy():
	m_enemyAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_alpha(1.0f),
	m_targetAngle(0.0f),
	m_state(DEFAULT),
	m_attackTimer(0.0f),
	m_actionCheckTimer(0.0f)
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
	if (m_actionCheckTimer > 0.0f)
	{
		m_actionCheckTimer -= 1.0f / kFramesPerSecond;
	}
	switch (m_state)
	{
	case StrongEnemy::DEFAULT:
		// îÕàÕçUåÇÇÃîªíf çUåÇîºåaÇÊÇËâìÇ≠ÅAîÕàÕçUåÇÇÃç≈ëÂîºåaÇ…ãﬂÇ¢ãóó£ÇÃèÍçá
		if (m_toPlayerDistance > kAttackRadius + 10.0f && m_toPlayerDistance < kRangeAttackRadius * 0.8f) 
		{
			int rand = GetRand(kRandMax);
			printfDx(L"rand:%d\n", rand);
			if (rand < kRangeAttackProbability)
			{
				m_state = StrongEnemyState::RANGEATTACK_CHARGE;
				m_attackTimer = kChageTime;
				break;
			}
		}
		// í èÌçUåÇÇÃîªíf(ÉLÉÉÉâÉNÉ^Å[ÇÃè’ìÀîºåaÇÃÇ∑ÇÆäOë§Ç≠ÇÁÇ¢)
		if (m_toPlayerDistance <= kAttackRadius + kColRadius)
		{
			OnAttack();
			m_state = StrongEnemyState::NORMALATTACK;
			break;
		}

		// à⁄ìÆ
		if (m_toPlayerDistance > kColRadius)
		{
			m_pos.x += m_toPlayerDir.x * kMoveSpeed;
			m_pos.z += m_toPlayerDir.z * kMoveSpeed;
		}
		break;
	case StrongEnemy::NORMALATTACK:
		m_enemyAttack.timer--;
		if (m_enemyAttack.timer < 0.0f)
		{
			m_state = StrongEnemyState::DEFAULT;
		}
		break;
	case StrongEnemy::RANGEATTACK_CHARGE:
		m_attackTimer -= 1.0f / kFramesPerSecond;
		if (m_attackTimer < 0.0f)
		{
			OnRangeAttack();
			m_state = StrongEnemyState::RANGEATTACK;
		}
		break;
	case StrongEnemy::RANGEATTACK:
		m_enemyAttack.timer--;
		if (m_enemyAttack.timer < 0.0f)
		{
			m_state = StrongEnemyState::COOLDOWN;
			m_attackTimer = kCoolDownTime;
		}
		break;
	case StrongEnemy::COOLDOWN:
		m_attackTimer -= 1.0f / kFramesPerSecond;
		if (m_attackTimer < 0.0f)
		{
			m_state = StrongEnemyState::DEFAULT;
		}
		break;
	}
}

void StrongEnemy::Draw()
{
	DrawSphere3D(m_pos,kColRadius,kDivNum,0x000000,0xffffff,true);
	if (m_state == StrongEnemyState::RANGEATTACK_CHARGE)
	{
		float progress = 1.0f - (m_attackTimer / kChageTime);
		float currentRadius = kRangeAttackRadius * progress; // åªç›ÇÃîºåaÇåvéZ(ç≈ëÂîºåa*êiçsó¶)
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		VECTOR drawPos = VAdd(m_pos, VGet(0.0f, 1.0f, 0.0f));
		DrawCone3D(VAdd(drawPos, VGet(0.0f, 0.1f, 0.0f)),drawPos, currentRadius, kDivNum, kAreaColor, kAreaColor,true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawCone3D(VAdd(drawPos, VGet(0.0f, 0.2f, 0.0f)), drawPos, kRangeAttackRadius, kDivNum, kOutLineColor, kOutLineColor, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	if (m_state == StrongEnemyState::NORMALATTACK)
	{
		DrawSphere3D(m_enemyAttack.pos, kColRadius, kDivNum, 0xffffff, 0xffffff, false);
	}
}

void StrongEnemy::OnAttack()
{
	m_enemyAttack.timer = kNormalAttackDuration;
	m_enemyAttack.active = true;
	m_enemyAttack.dir = VNorm(VGet(sinf(m_targetAngle), 0.0f, cosf(m_targetAngle)));
	m_enemyAttack.pos = VAdd(m_pos, VScale(m_enemyAttack.dir, kNormalAttackRange));
}

void StrongEnemy::OnRangeAttack()
{
	m_enemyAttack.pos = m_pos;
	m_enemyAttack.active = true;
	m_enemyAttack.dir = VNorm(VSub(m_pPlayer->GetPos(), m_pos));
	m_enemyAttack.timer = kRangeAttackDuration;
	m_enemyAttack.radius = kRangeAttackRadius;
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
