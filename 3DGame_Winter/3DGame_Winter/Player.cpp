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
	// ����
	constexpr float kMoveDecRate = 0.80f;
    // �����̒���
    constexpr float kForwardLineLength = 100.0f;
    // �A�i���O�X�e�B�b�N�̃f�b�h�]�[��
    constexpr double kAnalogDeadZone = 0.25;
    constexpr float kRotateSpeed = 0.2f;
}

Player::Player():
	m_pos({0.0f,0.0f,0.0f}),
	m_vec({ 0.0f,0.0f,0.0f }),
    m_angleY(0.0f),
    m_isJump(false)
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
    // �A�i���O�X�e�B�b�N�̓��͂��擾
    int stickX = 0;
    int stickY = 0;
    GetJoypadAnalogInput(&stickX, &stickY, DX_INPUT_PAD1);
    // ���͒l��-1.0f����1.0f�͈̔͂ɐ��K��
    float inputX = stickX / 1000.0f;
    // �Q�[���̍��W�n�ɍ��킹��Y���̕����𔽓]�i�X�e�B�b�N�オ�������j
    float inputZ = -stickY / 1000.0f;
    // �f�b�h�]�[������
    // �X�e�B�b�N�̂킸���ȌX���𖳎�����
    VECTOR rawInput = VGet(inputX, 0.0f, inputZ);
    SetJoypadDeadZone(DX_INPUT_PAD1, kAnalogDeadZone);
    // ���͂𐳋K��
    VECTOR inputVec = VNorm(VGet(inputX, 0.0f, inputZ));
    VECTOR moveDir = VGet(0.0f,0.0f,0.0f);
    if (inputX != 0.0f || inputZ != 0.0f)
    {
        // �J�����p�x�ŉ�]�␳
        float cameraYaw = -m_pCamera->GetHorizonrtalAngle();
        float cosY = cosf(cameraYaw);
        float sinY = sinf(cameraYaw);
        moveDir.x = inputVec.x * cosY - inputVec.z * sinY;
        moveDir.z = inputVec.x * sinY + inputVec.z * cosY;
        // 1. �X�e�B�b�N�̓��͕�������u�ڕW�p�x�v���v�Z
        float targetAngle = atan2f(moveDir.x, moveDir.z);
        // 2. ���݂̊p�x�ƖڕW�p�x�̍����v�Z
        float diff = targetAngle - m_angleY;
        // 3. �p�x�̍���180�x (-DX_PI_F) �` 180�x (DX_PI_F) �͈̔͂Ɏ��܂�悤�ɕ␳�i�ŒZ�o�H���v�Z�j
        if (diff > DX_PI_F)
        {
            diff -= 2.0f * DX_PI_F;
        }
        else if (diff < -DX_PI_F)
        {
            diff += 2.0f * DX_PI_F;
        }
        // 4. ���`��Ԃ��s���A���݂̊p�x�����炩�ɖڕW�p�x�֋߂Â���
        m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);
        // 5. m_angleY �̒l�� -DX_PI_F �` DX_PI_F �͈̔͂ɐ��K������
        if (m_angleY > DX_PI_F)
        {
            m_angleY -= 2.0f * DX_PI_F;
        }
        else if (m_angleY < -DX_PI_F)
        {
            m_angleY += 2.0f * DX_PI_F;
        }
        // �ړ��x�N�g�����X�V
        m_vec.x = moveDir.x * kMoveSpeed;
        m_vec.z = moveDir.z * kMoveSpeed;
    }
    else
    {
        m_vec.x *= kMoveDecRate;
        m_vec.z *= kMoveDecRate;
    }

    if (Pad::isTrigger(PAD_INPUT_1) && m_pos.y <= 0.0f)
    {
        m_vec.y = kJumpPower;
        m_isJump = true;
    }
    m_vec.y += kGravity;
    if (m_pos.y + m_vec.y < 0.0f)
    {
        m_pos.y = 0.0f;   // �n�ʂɌŒ�
        m_vec.y = 0.0f;   // �c���x���[��
        m_isJump = false; // ���n
    }
    else
    {
        m_pos.y += m_vec.y;
    }
    
    m_pos.x += m_vec.x;
    m_pos.z += m_vec.z;
}

void Player::Draw()
{
    // �����ɍ��킹�Đ�����`��
    VECTOR forwardDir;
    forwardDir.x = sinf(m_angleY) * kForwardLineLength;
    forwardDir.y = 0.0f;
    forwardDir.z = cosf(m_angleY) * kForwardLineLength;

    VECTOR lineStart = VGet(m_pos.x, m_pos.y + kSphereRadius / 2, m_pos.z);
    VECTOR lineEnd = VAdd(lineStart, forwardDir);

	DrawSphere3D(m_pos,kSphereRadius,kDivNum,kSphereDifColor,kSphereSpcColor,true);
    DrawLine3D(lineStart, lineEnd, kSphereDifColor);
}
