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
	void OnAttack2();
	void OnSpecialSkil();
	void OnAvoidance();
	void SetEnemyPos(VECTOR enemyPos) { m_enemyPos = enemyPos; };
	VECTOR GetPlayerDir() { return m_forwardDir; }
private:
	enum class PlayerState
	{
		NORMAL,
		ROTATING_TO_ATTACK,
		ATTACKING,
		ROTATING_TO_ATTACK2,
		ATTACKING2
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
	AttackSphere m_attack2;
	AttackSphere m_specialSkil;
	VECTOR m_dirToEnemy;
	VECTOR m_moveInput; 
	float m_distanceToEnemy; // エネミーまでの距離の長さ
	bool m_isInAttackSequence; // プレイヤーが攻撃中か方向補正中か
	float m_avoidanceTimer; // 回避時間
	bool m_isAvoidanceFlag; // 回避のフラグ
	int m_specialGauge; // 必殺ゲージ
	bool m_isSpecialSkilFlag; // 必殺技のフラグ
};

