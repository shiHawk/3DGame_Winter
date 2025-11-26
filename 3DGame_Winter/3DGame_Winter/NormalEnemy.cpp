#include "NormalEnemy.h"
#include <cmath>
namespace
{
	constexpr VECTOR kDefaultPos = { -500.0f,0.0f,500.0f };
	constexpr VECTOR kDefaultDir = { 0.0,270.0f,0.0f };
	constexpr float kSphereRadius = 90.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x000000;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kColRadius = 30.0f;
	constexpr float kMoveSpeed = 5.0f;
	constexpr float kMoveDecRate = 0.8f;
	constexpr float kModelScale = 60.0f; // モデルのスケール
	constexpr float kMoveThreshold = 0.1f; // 移動とみなす閾値

	constexpr float kAttackRadius = 30.0f;
	constexpr float kAttackRange = 90.0f;
	constexpr float kAttackDuration = 40.0f;

	constexpr int kIdleAnimNo = 41;
	constexpr int kWalkAnimNo = 55;
	constexpr int kAttackAnimNo = 5;
	constexpr int kDamageAnimNo = 40;
	constexpr int kKnockbackAnimNo = 25;
	constexpr float kWalkAnimIncrement = 0.6f; // 歩行アニメーションの再生速度
	constexpr float kIdleAnimIncrement = 0.4f; // 待機アニメーションの再生速度
	constexpr float kAttackAnimIncrement = 0.5f; // 待機アニメーションの再生速度
	constexpr float kDamageAnimIncrement = 0.6f; // 被弾アニメーションの再生速度
	constexpr float kKnockbackAnimIncrement = 0.7f;

	constexpr float kInvincibilityTime = 30.0f;
	constexpr float kMaxCoolTime = 30.0f;
	constexpr int kMaxHp = 50;

	const float kKnockbackDuration = 0.5f;
	const float kKnockBackSpeed = 5.0f;
	// 秒数変換
	constexpr float kFramesPerSecond = 60.0f;
}

NormalEnemy::NormalEnemy():
	m_enemyAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_alpha(1.0f),
	m_targetAngle(0.0f),
	m_knockbackTimer(0.0f),
	m_knockbackDir({ 0.0f,0.0f,0.0f })
{
}

NormalEnemy::~NormalEnemy()
{
}

void NormalEnemy::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion)
{
	Enemy::Init(pPlayer, pCompanion);
	m_hp = kMaxHp;
	m_pos = kDefaultPos;
	m_modelHandle = MV1LoadModel(L"Data/model/Skeleton_Warrior.mv1");
	MV1SetScale(m_modelHandle, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetRotationXYZ(m_modelHandle, kDefaultDir);
	AttachAnim(m_modelHandle, kIdleAnimNo);
}

void NormalEnemy::End()
{
	MV1DeleteModel(m_modelHandle);
}

void NormalEnemy::Update()
{
	if (m_invincibilityTimer > 0.0f)
	{
		// 無敵時間タイマーを減らす
		//printfDx(L"m_invincibilityTimer:%f\n", m_invincibilityTimer);
		m_invincibilityTimer--;
		ChangeAnim(m_modelHandle, kDamageAnimNo, false, kDamageAnimIncrement);
		MV1SetDifColorScale(m_modelHandle, GetColorF(1.0f, 0.6f, 0.6f, 1.0f));
		if (m_invincibilityTimer <= 0.0f)
		{
			m_invincibilityTimer = 0.0f;
			m_isHitFlag = false;
			MV1SetDifColorScale(m_modelHandle, GetColorF(1.0f, 1.0f, 1.0f, m_alpha));
			// 無敵時間が終わったら、強制的に待機アニメーションに戻す
			ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
		}
	}
	else if (m_knockbackTimer > 0.0f)
	{
		
	}
	else
	{
		if (m_AttackCoolTime > 0.0f)
		{
			m_AttackCoolTime--; // クールタイムを減らす
		}
		MV1SetDifColorScale(m_modelHandle, GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
		m_toPlayerDistance = VSize(VSub(m_pPlayer->GetPos(), m_pos));
		m_toPlayerDir = VNorm(VSub(m_pPlayer->GetPos(), m_pos));
		m_targetAngle = atan2f(m_toPlayerDir.x, m_toPlayerDir.z);
		if (m_toPlayerDistance > kSphereRadius)
		{
			m_pos.x += m_toPlayerDir.x * kMoveSpeed * kMoveDecRate;
			m_pos.z += m_toPlayerDir.z * kMoveSpeed * kMoveDecRate;
			if (VSize(VGet(m_toPlayerDir.x, 0.0f, m_toPlayerDir.z)) > kMoveThreshold)
			{
				// 移動中→移動アニメーションへ変更
				ChangeAnim(m_modelHandle, kWalkAnimNo, true, kWalkAnimIncrement);
			}
		}
		else
		{
			// 停止後→待機アニメーションへ変更
			// ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
			if (m_enemyAttack.timer <= 0.0f && m_enemyAttack.active)
			{
				m_enemyAttack.active = false;
				// 停止後→待機アニメーションへ変更
				ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
				m_AttackCoolTime = kMaxCoolTime;
			}
			else if (m_enemyAttack.timer <= 0.0f && m_AttackCoolTime <= 0.0f)
			{
				OnAttack();
			}
		}

		if (m_enemyAttack.active)
		{
			m_enemyAttack.timer--;
		}
		//printfDx(L"m_AttackCoolTime:%f\n", m_AttackCoolTime);
		MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_targetAngle + DX_PI_F, 0.0f));
	}
	/*if (m_knockbackTimer > 0.0f)
	{
		ChangeAnim(m_modelHandle, kKnockbackAnimNo,false, kKnockbackAnimIncrement);
		m_pos = VAdd(m_pos, VScale(m_knockbackDir, kKnockBackSpeed));
		m_knockbackTimer -= 1.0f / kFramesPerSecond;
		MV1SetPosition(m_modelHandle, m_pos);
	}*/
	/*if (m_hp < 0)
	{
		m_alpha -= 0.01f;
	}*/
	MV1SetPosition(m_modelHandle,m_pos);
	UpdateAnim();
}

void NormalEnemy::Draw()
{
	//DrawSphere3D(m_pos, kSphereRadius, kDivNum, kSphereDifColor, kSphereSpcColor, true);
	//MV1SetDifColorScale(m_modelHandle, GetColorF(1.0f, 1.0f, 1.0f, m_alpha));
	MV1DrawModel(m_modelHandle);
}

void NormalEnemy::OnAttack()
{
	m_enemyAttack.active = true;
	m_enemyAttack.radius = kAttackRadius;
	m_enemyAttack.dir = VNorm(VGet(sinf(m_targetAngle), 0.0f, cosf(m_targetAngle)));
	m_enemyAttack.pos = VAdd(m_pos, VScale(m_enemyAttack.dir, kAttackRange));
	m_enemyAttack.timer = kAttackDuration;
	ChangeAnim(m_modelHandle,kAttackAnimNo,false,kAttackAnimIncrement);
}

void NormalEnemy::OnDamage()
{
	if (m_invincibilityTimer > 0.0f) return;
	m_isHitFlag = true;
	m_hp -= m_pPlayer->GetAttackPower();
	m_invincibilityTimer = kInvincibilityTime;
	//printfDx(L"m_hp:%d\n",m_hp);
}

float NormalEnemy::GetColRadius()
{
	return kColRadius;
}

VECTOR NormalEnemy::GetDir()
{
	return VECTOR();
}

void NormalEnemy::ApplyKnockback(VECTOR direction, float power)
{
	m_knockbackDir = direction;
	m_knockbackTimer = kKnockbackDuration;
}
