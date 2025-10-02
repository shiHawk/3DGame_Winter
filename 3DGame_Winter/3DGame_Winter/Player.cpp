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
    constexpr float kJumpPower = 10.0f;
    constexpr float kGravity = -0.5f;
	// 減速
	constexpr float kMoveDecRate = 0.80f;
    // 線分の長さ
    constexpr float kForwardLineLength = 100.0f;
    constexpr float kBackLineLength = 540.0f;
    // アナログスティックのデッドゾーン
    constexpr double kAnalogDeadZone = 0.25;
    constexpr float kRotateSpeed = 0.2f;
    constexpr float kFrontLimit = -1000.0f; // ステージ奥
    constexpr float kBackLimit = 1000.0f;   // ステージ手前
    constexpr float kLeftLimit = -1000.0f;   // ステージ左
    constexpr float kRightLimit = 1000.0f;   // ステージ右
    constexpr float kWallOffset = 0.001f;
}

Player::Player():
    m_angleY(0.0f),
    m_isJump(false),
    m_forwardDir({0.0f,0.0f,0.0f}),
    m_backDir({ 0.0f,0.0f,0.0f })
{
}

void Player::Init(std::shared_ptr<Camera> pCamera)
{
	m_pCamera = pCamera;
	m_pos = { 0.0f,0.0f,0.0f };
	m_vec = { 0.0f,0.0f,0.0f };
    m_angleY = 0.0f;
    m_isJump = false;
}

void Player::End()
{
}

void Player::Update()
{
    VECTOR moveInput = HandleInput();
    UpdateMovement(moveInput);
    if (Pad::isTrigger(PAD_INPUT_1) && m_pos.y <= 0.0f)
    {
        m_vec.y = kJumpPower;
        m_isJump = true;
    }
    m_vec.y += kGravity;
    if (m_pos.y + m_vec.y < 0.0f)
    {
        m_pos.y = 0.0f;   // 地面に固定
        m_vec.y = 0.0f;   // 縦速度をゼロ
        m_isJump = false; // 着地
    }
    else
    {
        m_pos.y += m_vec.y;
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
    if(nextPos.x <= kLeftLimit + kWallOffset)
    {
        nextPos.x = kLeftLimit + kWallOffset;
        m_vec.x = 0.0f;
    }
    else if(nextPos.x >= kRightLimit - kWallOffset) 
    {
        nextPos.x = kRightLimit - kWallOffset;
        m_vec.x = 0.0f;
    }
    m_pos = nextPos;
    //DrawFormatString(0, 15, 0xffffff, L"m_pos.z:%f", m_pos.z);
}

void Player::Draw()
{
    // 向きに合わせて線分を描画
    m_forwardDir.x = sinf(m_angleY) * kForwardLineLength;
    m_forwardDir.y = 0.0f;
    m_forwardDir.z = cosf(m_angleY) * kForwardLineLength;
    //printfDx(L"m_forwardDir.z:%f\n", m_forwardDir.z);
    VECTOR lineStart = VGet(m_pos.x, m_pos.y + kSphereRadius / 2, m_pos.z);
    VECTOR lineEnd = VAdd(lineStart, m_forwardDir);

	DrawSphere3D(m_pos,kSphereRadius,kDivNum,kSphereDifColor,kSphereSpcColor,true);
    DrawLine3D(lineStart, lineEnd, kSphereDifColor);
}

VECTOR Player::GetBackLineEnd()
{
    VECTOR lineStart = VGet(m_pos.x, m_pos.y + kSphereRadius / 2, m_pos.z);
    m_backDir.x = -sinf(m_angleY) * kBackLineLength;
    m_backDir.y = 200.0f;
    m_backDir.z = -cosf(m_angleY) * kBackLineLength;
    return VAdd(lineStart, m_backDir);
}

VECTOR Player::HandleInput()
{
    // アナログスティックの入力を取得
    int stickX = 0;
    int stickY = 0;
    GetJoypadAnalogInput(&stickX, &stickY, DX_INPUT_PAD1);
    SetJoypadDeadZone(DX_INPUT_PAD1, kAnalogDeadZone);

    // 入力値を-1.0fから1.0fの範囲に正規化
    float inputX = stickX / 1000.0f;
    float inputZ = -stickY / 1000.0f; // Y軸をZ軸に(奥方向)

    // 入力がない場合はゼロベクトルを返す
    if (inputX == 0.0f && inputZ == 0.0f)
    {
        return VGet(0.0f, 0.0f, 0.0f);
    }

    // 入力ベクトルを正規化
    VECTOR inputVec = VNorm(VGet(inputX, 0.0f, inputZ));

    // カメラの向きに合わせて入力ベクトルを回転
    float cameraYaw = -m_pCamera->GetHorizonrtalAngle();
    float cosY = cosf(cameraYaw);
    float sinY = sinf(cameraYaw);

    VECTOR moveDir;
    moveDir.x = inputVec.x * cosY - inputVec.z * sinY;
    moveDir.z = inputVec.x * sinY + inputVec.z * cosY;
    moveDir.y = 0.0f;

    return moveDir;
}

void Player::UpdateMovement(const VECTOR& moveDir)
{
    // moveDirの長さが0より大きいか(入力があるか)で判断
    if (VSize(moveDir) > 0.0f)
    {
        // 1.スティックの入力方向から「目標角度」を計算
        float targetAngle = atan2f(moveDir.x, moveDir.z);

        // 2.現在の角度と目標角度の差を最短で計算
        float diff = targetAngle - m_angleY;
        if (diff > DX_PI_F)
        {
            diff -= 2.0f * DX_PI_F;
        }
        else if (diff < -DX_PI_F)
        {
            diff += 2.0f * DX_PI_F;
        }

        // 3.線形補間で滑らかに回転
        m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);

        // 4.角度を -PI ~ PI の範囲に正規化
        if (m_angleY > DX_PI_F)
        {
            m_angleY -= 2.0f * DX_PI_F;
        }
        else if (m_angleY < -DX_PI_F) 
        {
            m_angleY += 2.0f * DX_PI_F;
        } 

        // 5.移動ベクトルを更新
        m_vec.x = moveDir.x * kMoveSpeed;
        m_vec.z = moveDir.z * kMoveSpeed;
    }
    else // 入力がない場合
    {
        // 減速処理
        m_vec.x *= kMoveDecRate;
        m_vec.z *= kMoveDecRate;
    }
}
