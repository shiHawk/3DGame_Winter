#include "NormalEnemy.h"
#include <cmath>
#include "SoundManager.h"
namespace
{
	constexpr VECTOR kDefaultPos = { -500.0f,-59.0f,300.0f };
	constexpr VECTOR kDefaultDir = { 0.0,270.0f,0.0f };
	constexpr float kSphereRadius = 90.0f;
	constexpr int kDivNum = 8;
	constexpr unsigned int kSphereDifColor = 0x000000;
	constexpr unsigned int kSphereSpcColor = 0xffffff;
	constexpr unsigned int kNormalDamageColor = 0xffffff;
	constexpr unsigned int kWeekDamageColor = 0xff2a2a;
	constexpr float kColRadius = 30.0f;
	constexpr float kMoveSpeed = 5.0f;
	constexpr float kMoveDecRate = 0.8f;
	constexpr float kModelScale = 60.0f; // モデルのスケール
	constexpr float kMoveThreshold = 0.1f; // 移動とみなす閾値
	constexpr float kTrackingRange = 600.0f;

	constexpr float kAttackRadius = 30.0f;
	constexpr float kAttackRange = 90.0f;
	constexpr float kAttackDuration = 40.0f;

	constexpr int kIdleAnimNo = 41;
	constexpr int kWalkAnimNo = 55;
	constexpr int kAttackAnimNo = 5;
	constexpr int kDamageAnimNo = 40;
	constexpr int kKnockbackAnimNo = 25;
	constexpr int kDeathAnimNo = 25;
	constexpr float kWalkAnimIncrement = 0.6f; // 歩行アニメーションの再生速度
	constexpr float kIdleAnimIncrement = 0.4f; // 待機アニメーションの再生速度
	constexpr float kAttackAnimIncrement = 0.5f; // 攻撃アニメーションの再生速度
	constexpr float kDamageAnimIncrement = 0.6f; // 被弾アニメーションの再生速度
	constexpr float kDeathAnimIncrement = 0.4f; // 死亡アニメーションの再生速度
	constexpr float kKnockbackAnimIncrement = 0.7f;

	constexpr float kInvincibilityTime = 30.0f;
	constexpr float kMaxCoolTime = 60.0f;
	constexpr float kAttackHitStartTime = 25.0f; // 判定が出始めるタイミング
	constexpr float kAttackHitEndTime = 10.0f;   // 判定が終わるタイミング
	constexpr int kMaxHp = 100;
	constexpr int kAttackPower = 45;

	constexpr float kKnockbackDuration = 0.4f;
	constexpr float kKnockBackSpeed = 5.0f;
	constexpr float kRecoveryTime = 15.0f;
	constexpr float kAttenuationRate = 0.8f; // 被ダメージの減衰率
	constexpr float kCumulativeRate = 1.75f; // 被ダメージの累加率
	// 秒数変換
	constexpr float kFramesPerSecond = 60.0f;
}

NormalEnemy::NormalEnemy():
	m_enemyAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_alpha(1.0f),
	m_targetAngle(0.0f),
	m_knockbackTimer(0.0f),
	m_knockbackDir({ 0.0f,0.0f,0.0f }),
	m_isKnockbackFlag(false),
	m_recoveryTimer(0.0f)
{
}

NormalEnemy::~NormalEnemy()
{
}

void NormalEnemy::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, VECTOR pos, float startAngle)
{
	Enemy::Init(pPlayer, pCompanion);
	m_hp = kMaxHp;
	m_attackPower = kAttackPower;
	m_pos = pos;
	m_colRadius = kColRadius;
	m_modelHandle = MV1LoadModel(L"Data/model/Skeleton_Warrior.mv1");
	MV1SetScale(m_modelHandle, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f,startAngle,0.0f));
	MV1SetPosition(m_modelHandle, m_pos);
	AttachAnim(m_modelHandle, kIdleAnimNo);
}

void NormalEnemy::End()
{
	m_vec.x = 0.0f;
	m_vec.z = 0.0f;
	MV1DeleteModel(m_modelHandle);
}

void NormalEnemy::Update()
{
	if (m_hp <= 0)
	{
		m_enemyAttack.active = false;
		m_alpha -= 0.02f;
		ChangeAnim(m_modelHandle, kDeathAnimNo, false, kDeathAnimIncrement);
		if (GetIsAnimEnd())
		{
			m_isDead = true; 
		}

		if (m_isDead)
		{
			End();
			return;
		}
	}
	else
	{
		// 移動ベクトル(m_vec)をリセット 
		m_vec.x = 0.0f;
		m_vec.z = 0.0f;
		m_vec.y -= 0.5f;
		SearchTarget();
		if (m_knockbackTimer > 0.0f)
		{
			ChangeAnim(m_modelHandle, kKnockbackAnimNo, false, kKnockbackAnimIncrement);
			m_pos = VAdd(m_pos, VScale(m_knockbackDir, kKnockBackSpeed));
			m_knockbackTimer -= 1.0f / kFramesPerSecond;
			//MV1SetPosition(m_modelHandle, m_pos);
			if (m_knockbackTimer <= 0.0f)
			{
				m_knockbackTimer = 0.0f;
				m_isKnockbackFlag = false;
				m_recoveryTimer = kRecoveryTime; // 復帰時間を設定
				// ノックバック終了後、待機状態へ
				ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
			}
		}
		else if (m_recoveryTimer > 0.0f) // 復帰待機処理
		{
			m_recoveryTimer--; // タイマーを減らす
			// 待機アニメーションを継続 
			ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
		}
		else if (m_invincibilityTimer > 0.0f)
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
		else
		{
			if (m_AttackCoolTime > 0.0f)
			{
				m_AttackCoolTime--;
			}
			MV1SetDifColorScale(m_modelHandle, GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

			// 距離を計算
			m_toPlayerDistance = VSize(VSub(m_targetPos, m_pos));

			// 追跡範囲内（kTrackingRange未満）の場合のみ回転と行動を行う
			if (m_toPlayerDistance < kTrackingRange)
			{
				// 方向と角度を計算して回転を適用
				m_toPlayerDir = VNorm(VSub(m_targetPos, m_pos));
				m_targetAngle = atan2f(m_toPlayerDir.x, m_toPlayerDir.z);
				MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_targetAngle + DX_PI_F, 0.0f));

				if (m_toPlayerDistance > kSphereRadius) // 追跡中
				{
					if ((!m_pPlayer->IsDead() || !m_pCompanion->IsDead()) && !m_isDead)
					{
						m_vec.x = m_toPlayerDir.x * kMoveSpeed * kMoveDecRate;
						m_vec.z = m_toPlayerDir.z * kMoveSpeed * kMoveDecRate;

						if (VSize(VGet(m_toPlayerDir.x, 0.0f, m_toPlayerDir.z)) > kMoveThreshold)
						{
							ChangeAnim(m_modelHandle, kWalkAnimNo, true, kWalkAnimIncrement);
						}
					}
				}
				else // 攻撃範囲内（密着時）
				{
					if (m_enemyAttack.timer <= 0.0f && m_enemyAttack.active)
					{
						m_enemyAttack.active = false;
						ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
						m_AttackCoolTime = kMaxCoolTime;
					}
					else if (m_enemyAttack.timer <= 0.0f && m_AttackCoolTime <= 0.0f)
					{
						OnAttack();
					}
				}
			}
			else // 追跡範囲外の場合
			{
				ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
				m_enemyAttack.active = false;
				// ここで回転処理を呼んでいないため、以前の向きを維持
			}

			if (m_enemyAttack.active)
			{
				m_enemyAttack.timer--;
			}
		}
	}
	
	MV1SetPosition(m_modelHandle,m_pos);
	UpdateAnim(m_modelHandle);
}

void NormalEnemy::Draw()
{
	if (m_enemyAttack.active)
	{
		//DrawSphere3D(m_enemyAttack.pos, kAttackRadius, kDivNum, kSphereDifColor, kSphereSpcColor, false);
	}
	
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

void NormalEnemy::OnDamage(int damage, bool isHatePlayer)
{
	if (m_invincibilityTimer > 0.0f) return;
	m_enemyAttack.active = false;
	m_enemyAttack.timer = 0.0f;
	m_isHitFlag = true;
	//m_hp -= damage;
	if (isHatePlayer) 
	{
		m_finalDamage = static_cast<float>(damage) *kAttenuationRate;
		m_damageColor = kNormalDamageColor;
		m_playerHate += static_cast<float>(damage);
	}
	else 
	{
		m_finalDamage = static_cast<float>(damage) * kCumulativeRate;
		m_damageColor = kWeekDamageColor;
		m_companionHate += static_cast<float>(damage) *3;
	}
	m_hp -= m_finalDamage;
	if (m_hp <= 0)
	{
		m_hp = 0;
		m_vec.x = 0.0f;
		m_vec.z = 0.0f;
		ChangeAnim(m_modelHandle,kDeathAnimNo,false, kDeathAnimIncrement);
	}
	
	DamageResult result{};
	result.pos = m_pos;
	result.damage = m_finalDamage;
	result.color = m_damageColor;
	m_damageResults.push_back(result); // ダメージの情報を表示用のリストに登録
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

int NormalEnemy::GetMaxHp()
{
	return kMaxHp;
}
