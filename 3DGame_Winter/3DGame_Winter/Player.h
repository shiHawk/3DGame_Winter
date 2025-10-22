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
	virtual void OnAttack() override;
	void OnAvoidance();
	void SetEnemyPos(VECTOR enemyPos) { m_enemyPos = enemyPos; };
	VECTOR GetPlayerDir() { return m_forwardDir; }
private:
	enum class PlayerState
	{
		NORMAL,
		ROTATING_TO_ATTACK,
		ATTACKING,
	};
	PlayerState m_playerState;
	VECTOR HandleInput();
	void UpdateMovement(const VECTOR& moveDir);
	void UpdatePlayerState();
	std::shared_ptr<Camera> m_pCamera;
	float m_angleY;
	bool m_isJump;
	VECTOR m_forwardDir;
	VECTOR m_enemyPos;
	AttackSphere m_attack;
	VECTOR m_dirToEnemy;
	VECTOR m_moveInput;
	float m_distanceToEnemy;
	bool m_isInAttackSequence; // ÉvÉåÉCÉÑÅ[Ç™çUåÇíÜÇ©ï˚å¸ï‚ê≥íÜÇ©
	float m_avoidanceTimer;
	bool m_isAvoidanceFlag;
};

