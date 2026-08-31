#pragma once
#include "DxLib.h"
#include <memory>
class WorldCollision;
class Camera
{
public:
	Camera();
	~Camera() {};
	void Init(std::shared_ptr<WorldCollision> pWorldCollision);
	void End();
	void Update();
	void Draw();
	void SetControlledCharacterPosition(const VECTOR& pos) { m_playerPos = pos; };
	void SetLockOnPosition(VECTOR lockOnPos);
	void SetPlayerDir(const VECTOR& dir) { m_playerDir = VNorm(dir); }
	float GetHorizonrtalAngle() { return m_cameraAngleHorizontal; }
	bool IsLockOn() { return m_isLockOn; }
	VECTOR GetCameraPos() { return m_cameraPos; }
	void SetIsBossBattle(bool isBossBattle) { m_isBossBattle = isBossBattle; }
	void SetIsLockOn(bool lockFlag) { m_isLockOn = lockFlag; }
	bool IsBossBattle() { return m_isBossBattle; }
private:
	std::shared_ptr<WorldCollision> m_pWorldCollision;
	// 度数をラジアンに変換する
	void RadianTranslation(); 
	int m_lightHandle;
	// カメラの位置
	VECTOR m_cameraPos;
	// カメラの注視点(見ている場所)
	VECTOR m_cameraTarget;
	// カメラの目標移動位置
	VECTOR m_cameraMoveTargetPos;
	float m_cameraAngleHorizontal; // 水平角度
	float m_cameraAngleVertical; // 垂直角度
	float m_targetAngleHorizontal; // 入力で決定された目標の水平角度
	float m_targetAngleVertical;   // 入力で決定された目標の垂直角度
	DINPUT_JOYSTATE m_input; // コントローラーの入力
	float m_viewRadianAngle; // 視野角
	VECTOR m_playerPos; // プレイヤーの位置
	VECTOR m_playerDir; // プレイヤーの向いている方向
	VECTOR m_cameraDrawPos; // 実際に描画・SetCameraPositionAndTargetに使う位置(Lerp後)
	bool m_isLockOn; // ロックオン中か
	VECTOR m_targetToPlayer;
	float m_targetToPlayerDistance;
	VECTOR m_lockOnCameraPos;
	int m_lockOnHandle;
	float m_lockOnRotateAngle;
	bool m_isBossBattle;
};

