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
	// カメラの位置
	VECTOR m_cameraPos;
	// カメラの注視点(見ている場所)
	VECTOR m_cameraTarget;
	// カメラの目標移動位置
	VECTOR m_cameraMoveTargetPos;
	float m_cameraAngle;
};

