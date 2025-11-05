#pragma once
#include "DxLib.h"
#include <memory>
#include "CharacterBase.h"
class Camera;
enum class ControlMode
{
	PLAYER,
	COMPANION
};
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
	void OnCombFinishAttack();
	void OnSpecialSkil();
	void OnAvoidance();
	void SetEnemyPos(VECTOR enemyPos) { m_enemyPos = enemyPos; }
	VECTOR GetPlayerDir() { return m_forwardDir; }
	void SetControlMode(ControlMode mode) { m_controlMode = mode; }
	void SetFollowTargetPos(VECTOR targetPos) { m_followTargetPos = targetPos; } // AI操作時に追従する対象（＝プレイヤー）の座標をセットする
private:
	enum class PlayerState
	{
		NORMAL,
		ROTATING_TO_ATTACK,
		ATTACKING,
		ROTATING_TO_ATTACK2,
		ATTACKING2,
		ROTATING_TO_COMBOFINISH,
		ATTACKING_COMBOFINISH,
		COMBO_WINDOW,
		SPECIALSKIL
	};
	ControlMode m_controlMode;
	PlayerState m_playerState;
	VECTOR HandleInput();
	void UpdateMovement(const VECTOR& moveDir);
	void UpdatePlayerState();
	std::shared_ptr<Camera> m_pCamera;
	float m_angleY;
	bool m_isJump;
	VECTOR m_forwardDir;
	VECTOR m_enemyPos;
	VECTOR m_followTargetPos; // AI時に追従する対象（プレイヤー）の座標
	AttackSphere m_attack;
	AttackSphere m_attack2;
	AttackSphere m_comboFinish;
	AttackSphere m_specialSkil;
	VECTOR m_dirToEnemy;
	VECTOR m_moveInput; 
	float m_distanceToEnemy; // エネミーまでの距離の長さ
	bool m_isInAttackSequence; // プレイヤーが攻撃中か方向補正中か
	float m_avoidanceTimer; // 回避時間
	bool m_isAvoidanceFlag; // 回避のフラグ
	int m_comboStep; // コンボの段階
	float m_comboWindowTimer; // コンボの受付時間
	int m_specialGauge; // 必殺ゲージ
	bool m_isSpecialSkilFlag; // 必殺技のフラグ
	void RotatingToAttackAndAttack(void (Player::* attackFunc)(), PlayerState nextState);
	void UpdateAttackState(AttackSphere& attackData, int animNo, float animInc, PlayerState nextState);
};

