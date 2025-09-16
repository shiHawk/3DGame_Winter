#include "Camera.h"
#include <cmath>
#include "Pad.h"
namespace
{
	constexpr float kLerpSpeed = 0.015f;
	constexpr float kOffSetPos = 200.0f;
	constexpr VECTOR kSecondLight = { -0.577f, -0.577f, 0.577f };
	constexpr float kRightLimitCamera = 4807.0f;
	constexpr float kLeftLimitCamera = -2355.0f;
	// �J�����̈ʒu�ƒ����_
	constexpr VECTOR kDefaultCameraPos = { 0.0f,300.0f,-840.0f };
	constexpr VECTOR kCameraTarget = { 0.0f,50.0f,0.0f };
	// �J�����̎���p
	constexpr float kViewAngle = 0.447f;
	// near��far�̈ʒu
	constexpr float kCameraNearClip = 10.0f;
	constexpr float kCameraFarClip = 3000.0f;
	// ���C�g�̃J���[
	constexpr float kRed = 1.0f;
	constexpr float kGreen = 0.647f;
	constexpr float kBlue = 0.0f;
	// �J�����̐���
	constexpr float kCameraAngleSpeed = 0.02f;
	constexpr float kCameraSphereLength = 840.0f;

}
Camera::Camera():
	m_cameraPos({0.0f,0.0f,0.0f}),
	m_cameraMoveTargetPos({ 0.0f,0.0f,0.0f }),
	m_cameraTarget({ 0.0f,0.0f,0.0f }),
	m_lightHandle(-1),
	m_cameraAngle(0.0f)
{
}

void Camera::Init()
{
	// 3D�\���̐ݒ�
	SetUseZBuffer3D(true);	  // Z�o�b�t�@���w�肷��
	SetWriteZBuffer3D(true);  // Z�o�b�t�@�ւ̏������݂��s��

	SetUseBackCulling(true);  // �|���S���̗��ʂ�\�����Ȃ�

	// ���C�g�̐F��ύX����
	SetLightSpcColor(GetColorF(kRed, kGreen, kBlue, 0.0f));

	// �J�����̈ʒu�̏��������s��

	// �J����(�n�_)�̈ʒu
	m_cameraPos = kDefaultCameraPos;

	// �J�������ǂ������Ă��邩(�����_)
	m_cameraTarget = kCameraTarget;
	// �J�����̈ʒu�ƒ����_���w�肷��
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, m_cameraTarget);

	// �J�����̎���p��ݒ肷��
	SetupCamera_Perspective(kViewAngle);

	// �J������near,far��ݒ肷��
	// ��ʂɕ\������鋗���͈̔͂�ݒ肷��
	// �J��������near�ȏ㗣��Ă���far���߂��ɂ�����̂�
	// �Q�[����ʂɕ\�������
	// far�͂��܂�傫�����鐔����ݒ肵�Ȃ��悤�ɋC��t����(�\���o�O�Ɍq����)
	SetCameraNearFar(kCameraNearClip, kCameraFarClip);
}

void Camera::End()
{
	m_cameraPos = kDefaultCameraPos;
	m_cameraTarget = kCameraTarget;
}

void Camera::Update()
{
	DINPUT_JOYSTATE input;
	// ���͏�Ԃ��擾
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
	// �J�����̊p�x�̌v�Z
	if (input.Rx > 0)
	{
		m_cameraAngle += kCameraAngleSpeed;
		if (m_cameraAngle > DX_PI_F) // �p�x�l��傫���������Ȃ��悤��-180���𒴂�����360������
		{
			m_cameraAngle -= DX_TWO_PI_F;
		}
	}
	else if(input.Rx < 0)
	{
		m_cameraAngle -= kCameraAngleSpeed;
		if (m_cameraAngle < -DX_PI_F)
		{
			m_cameraAngle += DX_TWO_PI_F;
		}
	}

	MATRIX rotY; // �J�����̉�]�s��
	float cameraToPlayerLength;
	rotY = MGetRotY(m_cameraAngle); // ���������̉�]��Y����]
	cameraToPlayerLength = kCameraSphereLength; // �J��������v���C���[�܂ł̏����̋������Z�b�g
	// �J�����̍��W���Z�o
	// Z���ɃJ�����ƃv���C���[�Ƃ̋����������L�т��x�N�g����
	// ����������]( X����] )���������Ɛ���������]( �x����] )���čX��
	// �����_�̍��W�𑫂������̂��J�����̍��W
	m_cameraPos = VAdd(VTransform(VGet(0.0f,0.0f, -cameraToPlayerLength),rotY), m_cameraTarget);
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, m_cameraTarget); // �J�������v�Z�����ʒu�ɐݒ肷��
}
