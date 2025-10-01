#pragma once
#include "DxLib.h"
#include <memory>
class Camera;
class Player
{
public:
	Player();
	~Player() {};
	void Init(std::shared_ptr<Camera> pCamera);
	void End();
	void Update();
	void Draw();
	VECTOR GetPlayerPos() { return m_pos; }
	VECTOR GetBackLineEnd();
private:
	VECTOR HandleInput();
	void UpdateMovement(const VECTOR& moveDir);
	VECTOR m_pos;
	VECTOR m_vec;
	std::shared_ptr<Camera> m_pCamera;
	float m_angleY;
	bool m_isJump;
	VECTOR m_forwardDir;
	VECTOR m_backDir;
};

