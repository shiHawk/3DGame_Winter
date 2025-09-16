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
private:
	VECTOR m_pos;
	VECTOR m_vec;
	std::shared_ptr<Camera> m_pCamera;
};

