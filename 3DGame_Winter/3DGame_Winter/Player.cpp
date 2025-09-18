#include "Player.h"
#include "Pad.h"
#include "Camera.h"
#include <cmath>
namespace
{
	constexpr float kSphereRadius = 20.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x000fff;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 7.0f;
    constexpr float kJumpPower = 7.0f;
    constexpr float kGravity = -0.5f;
	// 減速
	constexpr float kMoveDecRate = 0.80f;
    // 線分の長さ
    constexpr float kForwardLineLength = 100.0f;
    // アナログスティックのデッドゾーン
    constexpr double kAnalogDeadZone = 0.25;
    constexpr float kRotateSpeed = 0.1f;
}

Player::Player():
	m_pos({0.0f,0.0f,0.0f}),
	m_vec({ 0.0f,0.0f,0.0f }),
    m_angleY(0.0f)
{
}

void Player::Init(std::shared_ptr<Camera> pCamera)
{
	m_pCamera = pCamera;
	m_pos = { 0.0f,0.0f,0.0f };
	m_vec = { 0.0f,0.0f,0.0f };
    m_angleY = 0.0f;
}

void Player::End()
{
}

void Player::Update()
{
    // アナログスティックの入力を取得
    int stickX = 0;
    int stickY = 0;
    GetJoypadAnalogInput(&stickX, &stickY, DX_INPUT_PAD1);
    // 入力値を-1.0fから1.0fの範囲に正規化
    float inputX = static_cast<float>(stickX) / 1000.0f;
    // ゲームの座標系に合わせてY軸の符号を反転（スティック上が奥方向）
    float inputZ = -static_cast<float>(stickY) / 1000.0f;

    // デッドゾーン処理
    // スティックのわずかな傾きを無視する
    VECTOR rawInput = VGet(inputX, 0.0f, inputZ);
    SetJoypadDeadZone(DX_INPUT_PAD1, kAnalogDeadZone);
    // 入力がなければ減速だけして終了
    if (inputX == 0.0f && inputZ == 0.0f)
    {
        m_vec.x *= kMoveDecRate;
        m_vec.z *= kMoveDecRate;
        m_pos = VAdd(m_pos, m_vec);
        return;
    }
    // 入力を正規化
    VECTOR inputVec = VNorm(VGet(inputX, 0.0f, inputZ));

    // カメラ角度で回転補正
    float cameraYaw = -m_pCamera->GetHorizonrtalAngle();
    float cosY = cosf(cameraYaw);
    float sinY = sinf(cameraYaw);

    VECTOR moveDir;
    moveDir.x = inputVec.x * cosY - inputVec.z * sinY;
    moveDir.z = inputVec.x * sinY + inputVec.z * cosY;
    moveDir.y = 0.0f;
    // 1. スティックの入力方向から「目標角度」を計算
    float targetAngle = atan2f(moveDir.x, moveDir.z);

    // 2. 現在の角度と目標角度の差を計算
    float diff = targetAngle - m_angleY;

    // 3. 角度の差が180度 (-DX_PI_F) ～ 180度 (DX_PI_F) の範囲に収まるように補正（最短経路を計算）
    if (diff > DX_PI_F) 
    {
        diff -= 2.0f * DX_PI_F;
    }
    else if (diff < -DX_PI_F) 
    {
        diff += 2.0f * DX_PI_F;
    }

    // 4. 差に一定の割合を掛けて、現在の角度を滑らかに目標角度へ近づける
    m_angleY += diff * kRotateSpeed;
    // 5. m_angleY の値を -DX_PI_F ～ DX_PI_F の範囲に正規化する
    if (m_angleY > DX_PI_F) 
    {
        m_angleY -= 2.0f * DX_PI_F;
    }
    else if (m_angleY < -DX_PI_F)
    {
        m_angleY += 2.0f * DX_PI_F;
    }
    
    // 移動ベクトルを更新
    m_vec = VScale(moveDir, kMoveSpeed);
	m_pos = VAdd(m_pos, m_vec);
}

void Player::Draw()
{
    // 向きに合わせて線分を描画
    VECTOR forwardDir;
    forwardDir.x = sinf(m_angleY) * kForwardLineLength;
    forwardDir.y = 0.0f;
    forwardDir.z = cosf(m_angleY) * kForwardLineLength;

    VECTOR lineStart = VGet(m_pos.x, m_pos.y + kSphereRadius / 2, m_pos.z);
    VECTOR lineEnd = VAdd(lineStart, forwardDir);

	DrawSphere3D(m_pos,kSphereRadius,kDivNum,kSphereDifColor,kSphereSpcColor,true);
    DrawLine3D(lineStart, lineEnd, kSphereDifColor);
}
