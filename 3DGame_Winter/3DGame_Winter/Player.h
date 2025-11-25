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
	void OnCombFinishAttack();
	void OnSpecialSkil();
	void OnAvoidance();
	void SetEnemyPos(VECTOR enemyPos) { m_enemyPos = enemyPos; } 
	void SetControlMode(ControlMode mode) { m_controlMode = mode; }
	ControlMode GetControlMode() { return m_controlMode; }
	void SetFollowTargetPos(VECTOR targetPos) { m_followTargetPos = targetPos; } // AI操作時に追従する対象（＝プレイヤー）の座標をセットする
	float GetAttackRadius() { return m_attack.radius; }
	VECTOR GetAttackPos() { return m_attack.pos; }
	bool IsAttackActive() { return m_attack.active; }
	int GetAttackPower() { return m_attackPower; }
	// WorldCollisionから呼ばれる関数群
	void SetPos(VECTOR newPos) { m_pos = newPos; }
	void SetVecY(float newVecY) { m_vec.y = newVecY; }
	void SetIsJump(bool isJumping) { m_isJump = isJumping; }
	VECTOR GetVec() { return m_vec; }
	void AddPos(VECTOR offset);
	float GetColRadius();
	virtual VECTOR GetDir() override;
	bool IsSpecialSkilFlag() { return m_isSpecialSkilFlag; }
	void OnDamage(int enemyPower);
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
	bool IsComboFinish() 
	{
		if (m_playerState == PlayerState::ATTACKING_COMBOFINISH) return true;
		else return false;
	};
private:
	ControlMode m_controlMode;
	PlayerState m_playerState;
	VECTOR HandleInput();
	void UpdateMovement(const VECTOR& moveDir);
	void UpdatePlayerState();
	std::shared_ptr<Camera> m_pCamera;
	float m_angleY;
	VECTOR m_enemyPos;
	VECTOR m_followTargetPos; // AI時に追従する対象（プレイヤー）の座標
	AttackSphere m_attack;
	VECTOR m_dirToEnemy; 
	float m_distanceToEnemy; // エネミーまでの距離の長さ
	float m_distanceToFollowTarget;
	float m_avoidanceTimer; // 回避時間
	bool m_isAvoidanceFlag; // 回避のフラグ
	int m_comboStep; // コンボの段階
	float m_comboWindowTimer; // コンボの受付時間
	float m_comboCoolTimer; // AIのコンボ用クールタイム
	int m_specialGauge; // 必殺ゲージ
	bool m_isSpecialSkilFlag; // 必殺技のフラグ
	bool m_aiWillDo3HitCombo; // AIが3コンボ攻撃をするかどうかのフラグ

	// ステートごとのハンドラ関数
	void HandleStateNormal(bool aiWantsToAttack);
	void HandleStateAttacking();
	void HandleStateAttacking2();
	void HandleStateComboWindow();
	void HandleStateSpecialSkil();

	// AI / プレイヤーの分岐を持つハンドラ
	void HandleAIComboWindow();
	void HandlePlayerComboWindow();
	void RotatingToAttackAndAttack(void (Player::* attackFunc)(), PlayerState nextState);
	// 攻撃開始の共通処理（ヘルパー関数）
	void TryStartAttack(void(Player::* onAttackFunc)(), PlayerState rotationState, PlayerState attackState);
	void UpdateAttackState(AttackSphere& attackData, int animNo, float animInc, PlayerState nextState);
};

