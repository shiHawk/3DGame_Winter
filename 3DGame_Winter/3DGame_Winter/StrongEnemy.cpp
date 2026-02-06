#include "StrongEnemy.h"
#include "EffectManager.h"
#include "DxLib.h"
namespace
{
	constexpr float kAttackRadius = 30.0f;
	constexpr float kColRadius = 25.0f;
	constexpr VECTOR kDefaultPos = { 500.0f,-59.0f,500.0f };
	constexpr float kModelScale = 1.3f;
	constexpr float kChageTime = 1.5f;
	constexpr float kCoolDownTime = 1.5f;
	constexpr float kRangeAttackRadius = 150.0f;
	constexpr float kRangeAttackDuration = 30.0f;
	constexpr float kTrackingRange = 600.0f;
	constexpr float kActionCheckInterval = 0.5f; // 抽選頻度
	constexpr float kNormalAttackDuration = 20.0f;
	constexpr float kNormalAttackRange = 90.0f;
	constexpr float kMoveSpeed = 5.0f;
	// 各アニメーション番号
	constexpr int kIdleAnimNo = 9;
	constexpr int kWalkAnimNo = 15;
	constexpr int kAttackAnimNo = 13;
	constexpr int kRangeAttackAnimNo = 0;
	constexpr int kRangeAttackChargeAnimNo = 4;
	constexpr int kDamageAnimNo = 7;
	constexpr int kDeathAnimNo = 8;
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
	constexpr float kInvincibilityTime = 40.0f;
	constexpr int kMaxHp = 200;
	constexpr int kAttackPower = 75;
	constexpr float kAttenuationRate = 0.65f; // 被ダメージの減衰率
	constexpr float kCumulativeRate = 1.5f; // 被ダメージの累加率
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

void StrongEnemy::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<EffectManager> pEffectManager, VECTOR pos)
{
	Enemy::Init(pPlayer, pCompanion);
	m_pEffectManager = pEffectManager;
	m_modelHandle = MV1LoadModel(L"Data/model/StrongEnemy.mv1");
	AttachAnim(m_modelHandle, kIdleAnimNo);
	MV1SetScale(m_modelHandle,VGet(kModelScale, kModelScale, kModelScale));
	m_pos = pos;
	m_hp = kMaxHp;
	m_attackPower = kAttackPower;
	m_colRadius = kColRadius;
	MV1SetPosition(m_modelHandle,m_pos);
	m_deathTimer = 0.0f;
}

void StrongEnemy::End()
{
	MV1DeleteModel(m_modelHandle);
}

void StrongEnemy::Update()
{
	SearchTarget();
	m_toPlayerDistance = VSize(VSub(m_targetPos, m_pos));
	m_toPlayerDir = VNorm(VSub(m_targetPos, m_pos));
	m_targetAngle = atan2f(m_toPlayerDir.x, m_toPlayerDir.z);
	if (m_actionCheckTimer > 0.0f)
	{
		m_actionCheckTimer -= 1.0f / kFramesPerSecond;
	}
	if (m_state != StrongEnemyState::DEAD)
	{
		if (m_invincibilityTimer > 0.0f)
		{
			m_invincibilityTimer--;
			if (kMaxHp * 0.5f >= m_hp)
			{
				// HPが半分以下なら怯む(ダメージリアクションをとる)
				//ChangeAnim(m_modelHandle, kDamageAnimNo, true, kDamageAnimIncrement);
			}
			if (m_invincibilityTimer < 0.0f)
			{
				m_invincibilityTimer = 0.0f;
				m_isHitFlag = false;
			}
		}
	}
	
	switch (m_state)
	{
	case StrongEnemy::DEFAULT:
		// 範囲攻撃の判断 攻撃半径より遠く、範囲攻撃の最大半径に近い距離の場合
		if (m_toPlayerDistance > kAttackRadius + 10.0f && m_toPlayerDistance < kRangeAttackRadius * 0.8f) 
		{
			int rand = GetRand(kRandMax);
			//printfDx(L"rand:%d\n", rand);
			if (rand < kRangeAttackProbability)
			{
				m_state = StrongEnemyState::RANGEATTACK_CHARGE;
				m_attackTimer = kChageTime;
				break;
			}
		}
		// 通常攻撃の判断(キャラクターの衝突半径のすぐ外側くらい)
		if (m_toPlayerDistance <= kAttackRadius + kColRadius)
		{
			OnAttack();
			m_state = StrongEnemyState::NORMALATTACK;
			break;
		}

		// 移動
		if (m_toPlayerDistance > kColRadius && m_toPlayerDistance < kTrackingRange)
		{
			if (!m_pPlayer->IsDead() || !m_pCompanion->IsDead())
			{
				m_pos.x += m_toPlayerDir.x * kMoveSpeed;
				m_pos.z += m_toPlayerDir.z * kMoveSpeed;
				ChangeAnim(m_modelHandle, kWalkAnimNo, true, kWalkAnimIncrement);
			}
		}
		else 
		{
			ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
		}
		MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_targetAngle + DX_PI_F, 0.0f));
		break;
	case StrongEnemy::NORMALATTACK:
		m_enemyAttack.timer--;
		ChangeAnim(m_modelHandle, kAttackAnimNo, false, kAttackAnimIncrement);
		if (m_enemyAttack.timer < 0.0f)
		{
			m_state = StrongEnemyState::DEFAULT;
		}
		break;
	case StrongEnemy::RANGEATTACK_CHARGE:
		m_isAttackCharge = true;
		m_attackTimer -= 1.0f / kFramesPerSecond;
		ChangeAnim(m_modelHandle, kRangeAttackChargeAnimNo, false, kAttackAnimIncrement);
		if (m_attackTimer < 0.0f)
		{
			m_isAttackCharge = false;
			OnRangeAttack();
			m_state = StrongEnemyState::RANGEATTACK;
		}
		break;
	case StrongEnemy::RANGEATTACK:
		m_enemyAttack.timer--;
		ChangeAnim(m_modelHandle,kRangeAttackAnimNo,false,kAttackAnimIncrement);
		if (m_enemyAttack.timer < 0.0f)
		{
			m_state = StrongEnemyState::COOLDOWN;
			m_attackTimer = kCoolDownTime;
			m_enemyAttack.active = false;
		}
		break;
	case StrongEnemy::COOLDOWN:
		m_attackTimer -= 1.0f / kFramesPerSecond;
		ChangeAnim(m_modelHandle, kIdleAnimNo, false, kAttackAnimIncrement);
		if (m_attackTimer < 0.0f)
		{
			m_state = StrongEnemyState::DEFAULT;
		}
		break;
	case StrongEnemy::DEAD:
		ChangeAnim(m_modelHandle, kDeathAnimNo, false, kDamageAnimIncrement);
		m_vec = { 0.0f,0.0f,0.0f };
		m_deathTimer += 1.0f / kFramesPerSecond;
		if (GetIsAnimEnd() || m_deathTimer > 3.0f)
		{
			End();
			m_isDead = true;
			return;
		}
		break;
	}
	
	if (!m_isDead)
	{
		UpdateAnim(m_modelHandle);
		MV1SetPosition(m_modelHandle, m_pos);
	}
}

void StrongEnemy::Draw()
{
	MV1DrawModel(m_modelHandle);
	if (m_state == StrongEnemyState::RANGEATTACK_CHARGE)
	{
		float progress = 1.0f - (m_attackTimer / kChageTime);
		float currentRadius = kRangeAttackRadius * progress; // 現在の半径を計算(最大半径*進行率)
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		VECTOR drawPos = VAdd(m_pos, VGet(0.0f, 1.0f, 0.0f));
		DrawCone3D(VAdd(drawPos, VGet(0.0f, 0.1f, 0.0f)), drawPos, currentRadius, kDivNum, kAreaColor, kAreaColor, true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawCone3D(VAdd(drawPos, VGet(0.0f, 0.2f, 0.0f)), drawPos, kRangeAttackRadius, kDivNum, kOutLineColor, kOutLineColor, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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
	m_pEffectManager->StrongEnemyRangeAttackEffect(m_enemyAttack.pos);
}

void StrongEnemy::OnDamage(int damage, bool isHatePlayer)
{
	// すでに死んでいる、または無敵なら無視
	if (m_state == StrongEnemyState::DEAD || m_invincibilityTimer > 0.0f) return;

	// 1. 先にダメージ計算を行う
	//m_hp -= damage;
	if (isHatePlayer)
	{
		m_hp -= damage * kCumulativeRate;
		m_playerHate += (float)damage;
	}
	else
	{
		m_hp -= damage * kAttenuationRate;
		m_companionHate += (float)damage * 3;
	}
	m_enemyAttack.timer = 0.0f;
	m_isHitFlag = true;
	
	// 2. 計算後のHPで死亡判定を行う
	if (m_hp <= 0)
	{
		m_hp = 0;
		m_state = StrongEnemyState::DEAD;
		m_enemyAttack.active = false; // 攻撃判定を消す
		m_invincibilityTimer = 0.0f;
		// ループ再生は false にする
		ChangeAnim(m_modelHandle, kDeathAnimNo, false, kDamageAnimIncrement);
	}
	else
	{
		m_invincibilityTimer = kInvincibilityTime;
	}
}

float StrongEnemy::GetColRadius()
{
	return kColRadius;
}

VECTOR StrongEnemy::GetDir()
{
	return VECTOR();
}

int StrongEnemy::GetAttackPos()
{
	return kAttackPower;
}

int StrongEnemy::GetMaxHp()
{
	return kMaxHp;
}
