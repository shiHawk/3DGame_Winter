#include "Companion.h"
#include <cmath>
namespace
{
	constexpr VECTOR kDefaultPos = { 100.0f,0.0f,-60.0f };
	constexpr VECTOR kDefaultVec = { 0.0f,0.0f,0.0f };
	constexpr float kSphereRadius = 20.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x00f000;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 2.0f;
	constexpr float kJumpPower = 10.0f;
	constexpr float kGravity = -0.5f;
	// 減速
	constexpr float kMoveDecRate = 0.80f;
	constexpr float kRotateSpeed = 0.4f; // 方向転換の速度
	constexpr float kAngleThreshold = 0.1f; // 角度の差の閾値
	constexpr float kFrontLimit = -1000.0f; // ステージ奥
	constexpr float kBackLimit = 1000.0f;   // ステージ手前
	constexpr float kLeftLimit = -1000.0f;  // ステージ左
	constexpr float kRightLimit = 1000.0f;  // ステージ右
	constexpr float kWallOffset = 0.001f;
	// 線分の長さ
	constexpr float kForwardLineLength = 100.0f;
	constexpr float kBackLineLength = 540.0f;
	constexpr float kAutoTurnDistance = 300.0f;

	constexpr float kStopDistance = 20.0f * 4.0f;
}

Companion::Companion():
	m_forwardDir({0.0f,0.0f,0.0f}),
	m_enemyPos({ 0.0f,0.0f,0.0f }),
	m_dirToEnemy({ 0.0f,0.0f,0.0f }),
	m_companionToEnemy({ 0.0f,0.0f,0.0f }),
	m_attack(30.0f, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_distanceToEnemy(0.0f),
	m_angleY(0.0f)
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
}

void Companion::End()
{
	
}

void Companion::Update()
{
	// 敵の位置が未設定（初期値）なら何もしない
	if (m_enemyPos.x == 0.0f && m_enemyPos.y == 0.0f && m_enemyPos.z == 0.0f)
	{
		return;
	}

	m_companionToEnemy = VSub(m_enemyPos, m_pos); 
	m_distanceToEnemy = VSize(m_companionToEnemy);
	if (m_distanceToEnemy <= kAutoTurnDistance)
	{
		m_dirToEnemy = VNorm(m_companionToEnemy);
		float targetAngle = atan2f(m_dirToEnemy.x, m_dirToEnemy.z);
		float diff = targetAngle - m_angleY;
		if (diff > DX_PI_F)       diff -= 2.0f * DX_PI_F;
		else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;
		m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);

		if (m_angleY > DX_PI_F)       m_angleY -= 2.0f * DX_PI_F;
		else if (m_angleY < -DX_PI_F) m_angleY += 2.0f * DX_PI_F;

		/*if (m_distanceToEnemy > kStopDistance)
		{
			m_forwardDir.x = sinf(m_angleY);
			m_forwardDir.z = cosf(m_angleY);
			m_vec = VScale(m_forwardDir,kMoveSpeed);
			m_pos = VAdd(m_pos, m_vec);
		}
		else
		{
			m_vec = { 0.0f,0.0f,0.0f };
		}*/
	}
	else
	{
		// 敵が遠い場合は減速して停止
		m_vec = VScale(m_vec, kMoveDecRate);
		m_pos = VAdd(m_pos, m_vec);
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
	m_pos = nextPos;
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

	DrawSphere3D(m_pos, kSphereRadius, kDivNum, kSphereDifColor, kSphereSpcColor, true);
	DrawLine3D(lineStart, lineEnd, kSphereDifColor);
}

void Companion::OnAttack()
{

}
