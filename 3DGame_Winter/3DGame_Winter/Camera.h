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
	// カメラの位置
	VECTOR m_cameraPos;
	// カメラの注視点(見ている場所)
	VECTOR m_cameraTarget;
	// カメラの目標移動位置
	VECTOR m_cameraMoveTargetPos;
	float m_cameraAngleHorizontal;
	float m_cameraAngleVertical;
	float m_targetAngleHorizontal; // 入力で決定された目標の水平角度
	float m_targetAngleVertical;   // 入力で決定された目標の垂直角度
	DINPUT_JOYSTATE m_input;
	float m_viewRadianAngle;
};

