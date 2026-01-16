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
	VECTOR GetPlayerDir() { return m_forwardDir; }
	virtual void OnAttack() override;
	void OnStrongAttack();
	void OnSpecialSkil();
	void UpdateAIState();
	void UpdatePlayerControlState(); // プレイヤー時のステート
	void SetControlMode(ControlMode mode) { m_controlMode = mode; }
	ControlMode GetControlMode() { return m_controlMode; }
	float GetAttackRadius() { return m_attack.radius; }
	VECTOR GetAttackPos() { return m_attack.pos; }
	bool IsAttackActive() { return m_attack.active; }
	int GetAttackPower() { return m_attackPower; }
	void AddSpecialGauge(int increment);
	float GetColRadius();
	int GetMaxHp();
	void OnDamage(int damage);
	virtual VECTOR GetDir() override;
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
		SPECIALSKIL,
		TURN_FOR_STRONG_ATTACK, // 強攻撃前の回転
		TURN_FOR_SPECIALSKIL,   // 必殺技前の回転
		PREPARE_STRONG_ATTACK,  // 遠距離攻撃準備
		DAMAGE,
		DEATH
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
	float m_distanceToEnemy;
	float m_distanceToPlayer;
	float m_angleY;
	float m_attackCoolTimer;
	float m_avoidanceTimer;
	bool m_isAvoidanceFlag;
	float m_damageTimer;      // 硬直タイマー
	bool  m_isHitFlag;    // 無敵フラグ
	void RotatingToAttack();
	void OnAvoidance();
};

