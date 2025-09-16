#pragma once
#include "DxLib.h"
class Camera
{
public:
	Camera();
	~Camera() {};
	void Init();
	void End();
	void Update();
private:
	void RadianTranslation();
	int m_lightHandle;
	// �J�����̈ʒu
	VECTOR m_cameraPos;
	// �J�����̒����_(���Ă���ꏊ)
	VECTOR m_cameraTarget;
	// �J�����̖ڕW�ړ��ʒu
	VECTOR m_cameraMoveTargetPos;
	float m_cameraAngleHorizontal;
	float m_cameraAngleVertical;
	float m_targetAngleHorizontal; // ���͂Ō��肳�ꂽ�ڕW�̐����p�x
	float m_targetAngleVertical;   // ���͂Ō��肳�ꂽ�ڕW�̐����p�x
	DINPUT_JOYSTATE m_input;
	float m_viewRadianAngle;
};

