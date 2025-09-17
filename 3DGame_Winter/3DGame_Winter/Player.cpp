#include "Player.h"
#include "Pad.h"
#include "Camera.h"
namespace
{
	constexpr float kSphereRadius = 20.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x000fff;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 7.0f;
	// ����
	constexpr float kMoveDecRate = 0.80f;
}

Player::Player():
	m_pos({0.0f,0.0f,0.0f}),
	m_vec({ 0.0f,0.0f,0.0f })
{
}

void Player::Init(std::shared_ptr<Camera> pCamera)
{
	m_pCamera = pCamera;
	m_pos = { 0.0f,0.0f,0.0f };
	m_vec = { 0.0f,0.0f,0.0f };
}

void Player::End()
{
}

void Player::Update()
{
    // ���̓x�N�g�����쐬
    float inputX = 0.0f;
    float inputZ = 0.0f;

    if (Pad::isPress(PAD_INPUT_RIGHT)) inputX += 1.0f;
    if (Pad::isPress(PAD_INPUT_LEFT))  inputX -= 1.0f;
    if (Pad::isPress(PAD_INPUT_UP))    inputZ += 1.0f;
    if (Pad::isPress(PAD_INPUT_DOWN))  inputZ -= 1.0f;

    // ���͂��Ȃ���Ό����������ďI��
    if (inputX == 0.0f && inputZ == 0.0f)
    {
        m_vec.x *= kMoveDecRate;
        m_vec.z *= kMoveDecRate;
        m_pos = VAdd(m_pos, m_vec);
        return;
    }
    else
    {
        // ���͂𐳋K��
        VECTOR inputVec = VNorm(VGet(inputX, 0.0f, inputZ));

        // �J�����p�x�ŉ�]�␳
        float cameraYaw = -m_pCamera->GetHorizonrtalAngle();
        float cosY = cosf(cameraYaw);
        float sinY = sinf(cameraYaw);

        VECTOR moveDir;
        moveDir.x = inputVec.x * cosY - inputVec.z * sinY;
        moveDir.z = inputVec.x * sinY + inputVec.z * cosY;
        moveDir.y = 0.0f;

        // �ړ��x�N�g�����X�V
        m_vec = VScale(moveDir, kMoveSpeed);
    }
    
	
	m_pos = VAdd(m_pos, m_vec);
}

void Player::Draw()
{
	DrawSphere3D(m_pos,kSphereRadius,kDivNum,kSphereDifColor,kSphereSpcColor,true);
}
