#pragma once
#include <memory>
#include "DxLib.h"
#include "CharacterBase.h"
#include "Camera.h"
#include "Pad.h"

class Companion :public CharacterBase
{
public:
	Companion();
	~Companion();
	void Init(std::shared_ptr<Camera> pCamera);
	void End();
	virtual void Update()override;
	void Draw();
	void SetEnemyPos(VECTOR enemyPos) { m_enemyPos = enemyPos; };
	void SetPlayerPos(VECTOR playerPos) { m_playerPos = playerPos; };
	virtual void OnAttack() override;
	void OnStrongAttack();
	void OnSpecialSkil();
	void UpdateAIState();
	void UpdatePlayerControlState(); // プレイヤー時のステート
	void SetControlMode(ControlMode mode) { m_controlMode = mode; }
private:
	void UpdateMovement(const VECTOR& moveDir);
	VECTOR HandleInput();
	enum class CompanionState
	{
		NORMAL,
		FOLLOW_PLAYER,
		TRACK_ENEMY,
		NORMAL_ATTACK,
		STRONG_ATTACK,
		SPECIALSKIL
	};
	ControlMode m_controlMode;
	CompanionState m_companionState;
	std::shared_ptr<Camera> m_pCamera;
	VECTOR m_forwardDir;
	VECTOR m_enemyPos;
	VECTOR m_playerPos;
	VECTOR m_dirToEnemy;
	VECTOR m_dirToPlayer;
	VECTOR m_companionToEnemy;
	VECTOR m_companionToPlayer;
	AttackSphere m_attack;
	AttackSphere m_strongAttack;
	AttackSphere m_specialSkil;
	float m_distanceToEnemy;
	float m_distanceToPlayer;
	float m_angleY;
	float m_specialGauge;
	float m_attackCoolTimer;
};

