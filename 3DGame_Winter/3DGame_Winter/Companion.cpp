#include "Companion.h"
#include <cmath>
namespace
{
	constexpr VECTOR kDefaultPos = { 100.0f,0.0f,-60.0f };
	constexpr VECTOR kDefaultVec = { 0.0f,0.0f,0.0f };
	constexpr VECTOR kDefaultDir = { 0.0,270.0f * DX_PI_F / 180.0f,0.0f };
	constexpr float kSphereRadius = 20.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x00f000;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 2.0f;
	constexpr float kJumpPower = 10.0f;
	constexpr float kGravity = -5.5f;
	constexpr float kMoveThreshold = 0.1f; // 移動とみなす閾値
	// 減速
	constexpr float kMoveDecRate = 0.80f;
	constexpr float kRotateSpeed = 0.4f; // 方向転換の速度
	constexpr float kAngleThreshold = 0.1f; // 角度の差の閾値
	constexpr float kFrontLimit = -1000.0f; // ステージ奥
	constexpr float kBackLimit = 1000.0f;   // ステージ手前
	constexpr float kLeftLimit = -1000.0f;  // ステージ左
	constexpr float kRightLimit = 1000.0f;  // ステージ右
	constexpr float kWallOffset = 0.001f;

	constexpr float kModelScale = 50.0f; // モデルのスケール
	constexpr int kIdleAnimNo = 1;
	constexpr int kWalkAnimNo = 3;
	constexpr int kAttackAnimNo = 31;
	constexpr int kStrongAttackAnimNo = 40;
	constexpr float kComboFinishAttackAnimNo = 41;
	constexpr float kWalkAnimIncrement = 0.6f; // 歩行アニメーションの再生速度
	constexpr float kIdleAnimIncrement = 0.4f; // 待機アニメーションの再生速度
	constexpr float kAttackAnimIncrement = 0.5f; // 攻撃アニメーションの再生速度

	constexpr int kAttackPower = 10;
	constexpr int kStrongAttackPower = 30;
	constexpr float kAttackDuration = 30.0f;
	constexpr float kStrongAttackDuration = 40.0f;
	constexpr float kAttackRange = 60.0f;
	constexpr float kAttackCoolTime = 40.0f;

	// 線分の長さ
	constexpr float kForwardLineLength = 100.0f;
	constexpr float kBackLineLength = 540.0f;
	constexpr float kAutoTurnDistance = 300.0f;

	constexpr float kStopDistance = 80.0f;
	constexpr float kWarpDistance = 800.0f;
	constexpr float kPostWarpPosZ = 100.0f;

	constexpr float kStrongAttackBulletSpeed = 5.0f; // 弾の速度
}

Companion::Companion():
	m_companionState(CompanionState::NORMAL),
	m_forwardDir({0.0f,0.0f,0.0f}),
	m_enemyPos({ 0.0f,0.0f,0.0f }),
	m_playerPos({ 0.0f,0.0f,0.0f }),
	m_dirToEnemy({ 0.0f,0.0f,0.0f }),
	m_dirToPlayer({ 0.0f,0.0f,0.0f }),
	m_companionToEnemy({ 0.0f,0.0f,0.0f }),
	m_companionToPlayer({ 0.0f,0.0f,0.0f }),
	m_attack(30.0f, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_strongAttack(30.0f, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_distanceToEnemy(0.0f),
	m_distanceToPlayer(0.0f),
	m_angleY(0.0f),
	m_attackCoolTimer(0.0f)
{
}

Companion::~Companion()
{
}

void Companion::Init()
{
	m_pos = kDefaultPos;
	m_vec = kDefaultVec;
	m_distanceToEnemy = 0.0f;
	m_attack.active = false;
	m_modelHandle = MV1LoadModel(L"Data/model/Mage.mv1");
	MV1SetScale(m_modelHandle, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetRotationXYZ(m_modelHandle,kDefaultDir);
	AttachAnim(m_modelHandle,kIdleAnimNo);
}

void Companion::End()
{
	MV1DeleteModel(m_modelHandle);
}

void Companion::Update()
{
	// 敵の位置が未設定（初期値）なら何もしない
	if (m_enemyPos.x == 0.0f && m_enemyPos.y == 0.0f && m_enemyPos.z == 0.0f)
	{
		return;
	}
	printfDx(L"m_attackCoolTimer:%f\n", m_attackCoolTimer);
	m_companionToEnemy = VSub(m_enemyPos, m_pos); 
	m_distanceToEnemy = VSize(m_companionToEnemy);
	m_companionToPlayer = VSub(m_playerPos, m_pos);
	m_distanceToPlayer = VSize(m_companionToPlayer);
	if (m_companionState == CompanionState::NORMAL)
	{
		if (m_distanceToEnemy <= kAutoTurnDistance) // 敵に向かって移動する
		{
			m_companionState = CompanionState::TRACK_ENEMY;
		}
		else // プレイヤーに追従する
		{
			m_companionState = CompanionState::FOLLOW_PLAYER;
		}
	}
	UpdateCompanionState();
	if (m_distanceToPlayer > kWarpDistance) // プレイヤーと離れすぎたらプレイヤーの近くにワープする
	{
		m_pos = VGet(m_playerPos.x,m_playerPos.y,m_playerPos.z - kPostWarpPosZ);
		m_companionState = CompanionState::FOLLOW_PLAYER;
	}
	m_vec.y += kGravity;
	if (m_pos.y + m_vec.y < 0.0f)
	{
		m_pos.y = 0.0f;   // 地面に固定
		m_vec.y = 0.0f;   // 縦速度をゼロ
	}
	VECTOR nextPos = VAdd(m_pos, m_vec); // 仮の次の位置
	// Z方向(前後)制限
	if (nextPos.z >= kBackLimit - kWallOffset)
	{
		nextPos.z = kBackLimit - kWallOffset;
		m_vec.z = 0.0f;
	}
	else if (nextPos.z <= kFrontLimit + kWallOffset)
	{
		nextPos.z = kFrontLimit + kWallOffset;
		m_vec.z = 0.0f;
	}

	// X方向(左右)制限
	if (nextPos.x <= kLeftLimit + kWallOffset)
	{
		nextPos.x = kLeftLimit + kWallOffset;
		m_vec.x = 0.0f;
	}
	else if (nextPos.x >= kRightLimit - kWallOffset)
	{
		nextPos.x = kRightLimit - kWallOffset;
		m_vec.x = 0.0f;
	}

	if (m_companionState != CompanionState::NORMAL_ATTACK)
	{
		if (VSize(VGet(m_vec.x, 0.0f, m_vec.z)) > kMoveThreshold)
		{
			// 入力がある場合 → 移動アニメーションへ変更
			ChangeAnim(m_modelHandle, kWalkAnimNo, true, kWalkAnimIncrement);
		}
		else
		{
			// 入力がない場合 → 待機アニメーションへ変更
			ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
		}
	}
	m_pos = nextPos;
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_angleY+DX_PI_F, 0.0f));
	MV1SetPosition(m_modelHandle,m_pos);
	UpdateAnim();
}

void Companion::Draw()
{
	// 向きに合わせて線分を描画
	m_forwardDir.x = sinf(m_angleY) * kForwardLineLength;
	m_forwardDir.y = 0.0f;
	m_forwardDir.z = cosf(m_angleY) * kForwardLineLength;
	//printfDx(L"m_forwardDir.z:%f\n", m_forwardDir.z);
	VECTOR lineStart = VGet(m_pos.x, m_pos.y + kSphereRadius / 2, m_pos.z);
	VECTOR lineEnd = VAdd(lineStart, m_forwardDir);

	//DrawSphere3D(m_pos, kSphereRadius, kDivNum, kSphereDifColor, kSphereSpcColor, true);
	if (m_attack.active)
	{
		DrawSphere3D(m_attack.pos, m_attack.radius, kDivNum, kSphereDifColor, kSphereSpcColor, false);
	}
	DrawLine3D(lineStart, lineEnd, kSphereDifColor);
	MV1DrawModel(m_modelHandle);
}

void Companion::OnAttack()
{
	m_attackPower = kAttackPower;
	m_attack.dir = VNorm(VGet(sinf(m_angleY ), 0.0f, cosf(m_angleY )));
	m_attack.active = true;
	m_attack.pos = VAdd(m_pos, VScale(m_attack.dir, kAttackRange));
	m_attack.timer = kAttackDuration;
}

void Companion::OnStrongAttack()
{
	m_attackPower = kStrongAttackPower;
	m_strongAttack.timer = kStrongAttackDuration;
	m_strongAttack.active = true;
	m_strongAttack.dir = m_dirToEnemy;
}

void Companion::UpdateCompanionState()
{
	if (m_attackCoolTimer > 0.0f)
	{
		m_attackCoolTimer--;
	}
	switch (m_companionState)
	{
	case Companion::CompanionState::NORMAL:
	{
		// 敵が遠い場合は減速して停止
		m_vec = VScale(m_vec, kMoveDecRate);
		break;
	}
	case Companion::CompanionState::FOLLOW_PLAYER:
	{
		m_dirToPlayer = VNorm(m_companionToPlayer);
		float targetAngle = atan2f(m_dirToPlayer.x, m_dirToPlayer.z);
		//targetAngle += DX_PI_F;
		float diff = targetAngle - m_angleY;
		if (diff > DX_PI_F)       diff -= 2.0f * DX_PI_F;
		else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;
		m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);

		if (m_angleY > DX_PI_F)       m_angleY -= 2.0f * DX_PI_F;
		else if (m_angleY < -DX_PI_F) m_angleY += 2.0f * DX_PI_F;

		if (m_distanceToPlayer > kStopDistance)
		{
			m_forwardDir.x = sinf(m_angleY);
			m_forwardDir.z = cosf(m_angleY);
			m_vec = VScale(m_forwardDir, kMoveSpeed);
		}
		else
		{
			m_vec = { 0.0f,0.0f,0.0f };
			m_companionState = CompanionState::NORMAL;
		}
		break;
	}
	case Companion::CompanionState::TRACK_ENEMY:
	{
		m_dirToEnemy = VNorm(m_companionToEnemy);
		float targetAngle = atan2f(m_dirToEnemy.x, m_dirToEnemy.z);
		//targetAngle += DX_PI_F;
		float diff = targetAngle - m_angleY;
		if (diff > DX_PI_F)       diff -= 2.0f * DX_PI_F;
		else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;
		m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);

		if (m_angleY > DX_PI_F)       m_angleY -= 2.0f * DX_PI_F;
		else if (m_angleY < -DX_PI_F) m_angleY += 2.0f * DX_PI_F;

		if (m_distanceToEnemy > kStopDistance)
		{
			m_forwardDir.x = sinf(m_angleY);
			m_forwardDir.z = cosf(m_angleY);
			m_vec = VScale(m_forwardDir, kMoveSpeed);
		}
		else
		{
			m_vec = { 0.0f,0.0f,0.0f };
			if (m_attackCoolTimer <= 0.0f) // クールタイムが終わっているかチェック
			{
				OnAttack();
				m_companionState = CompanionState::NORMAL_ATTACK;
			}
			else
			{
				m_companionState = CompanionState::NORMAL;
			}
		}
		break;
	}
	case Companion::CompanionState::NORMAL_ATTACK:
		if (m_attack.active)
		{
			m_attack.timer--;
			ChangeAnim(m_modelHandle, kAttackAnimNo, false, kAttackAnimIncrement);
			if (m_attack.timer <= 0.0f)
			{
				m_attack.active = false;
				m_companionState = CompanionState::NORMAL;
				m_attackCoolTimer = kAttackCoolTime;
			}
		}
		break;
	case Companion::CompanionState::STRONG_ATTACK:
		if (m_strongAttack.active)
		{
			m_strongAttack.timer--;
			m_strongAttack.dir = m_dirToEnemy;
			if (m_strongAttack.timer <= 0.0f)
			{
				m_strongAttack.active = false;
				m_companionState = CompanionState::NORMAL;
			}
		}
		break;
	case Companion::CompanionState::SPECIALSKIL:
		break;
	default:
		break;
	}
}
