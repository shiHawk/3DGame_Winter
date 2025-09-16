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
	int m_lightHandle;
	// �J�����̈ʒu
	VECTOR m_cameraPos;
	// �J�����̒����_(���Ă���ꏊ)
	VECTOR m_cameraTarget;
	// �J�����̖ڕW�ړ��ʒu
	VECTOR m_cameraMoveTargetPos;
	float m_cameraAngle;
};

