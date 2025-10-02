#pragma once
#include "DxLib.h"
#include <memory>
#include "CharacterBase.h"
class Camera;
class Player:public CharacterBase
{
public:
	Player();
	~Player() {};
	void Init(std::shared_ptr<Camera> pCamera);
	void End();
	virtual void Update()override;
	void Draw();
	VECTOR GetPlayerPos() { return m_pos; }
	VECTOR GetBackLineEnd();
private:
	VECTOR HandleInput();
	void UpdateMovement(const VECTOR& moveDir);
	std::shared_ptr<Camera> m_pCamera;
	float m_angleY;
	bool m_isJump;
	VECTOR m_forwardDir;
	VECTOR m_backDir;
};

