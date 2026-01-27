#pragma once
#include "DxLib.h"
#include <memory>
#include "CharacterBase.h"
class Camera;
class EffectManager;

class Player :public CharacterBase
{
public:
	Player();
	~Player() {};
	void Init(std::shared_ptr<Camera> pCamera, std::shared_ptr<EffectManager> pEffectManager);
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
	void OnDamage(int enemyPower);
	bool IsDamageFlag() { return m_isHitFlag; }
	int GetMaxHp();
	void AddSpecialGauge(int increment);
	void SetEnemyAttackSensingFlag(bool isEnemyAttack) { m_isEnemyAttackSensing = isEnemyAttack; }
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
		SPECIALSKIL,
		AUTO_EVADE,
		DAMAGE,
		DEATH
	};
	bool IsComboFinish() 
	{
		if (m_playerState == PlayerState::ATTACKING_COMBOFINISH) return true;
		else return false;
	};
private:
	enum JumpState
	{
		None,   // ジャンプしていない
		Start,  // 飛び上がり
		Loop,   // 空中
		Land    // 着地
	};
	JumpState m_jumpState;
	ControlMode m_controlMode;
	PlayerState m_playerState;
	VECTOR HandleInput();
	void UpdateMovement(const VECTOR& moveDir);
	void UpdatePlayerState();
	void UpdateJumpState();
	std::shared_ptr<Camera> m_pCamera;
	std::shared_ptr<EffectManager> m_pEffectManager;
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
	bool m_aiWillDo3HitCombo; // AIが3コンボ攻撃をするかどうかのフラグ
	bool m_isEnemyAttackSensing;
	VECTOR m_retreatDir;
	float m_evadeCooldown;
	float m_jumpDelayTimer;
	bool  m_isJumpPending; // ジャンプの力を出すのを待っている状態か

	// ステートごとのハンドラ関数
	void HandleStateNormal(bool aiWantsToAttack);
	void HandleStateAttacking();
	void HandleStateAttacking2();
	void HandleStateComboWindow();
	void HandleStateSpecialSkil();
	void HandleStateDamage();
	void HandleStateDeath();
	void HandleStateAutoEvade();
	// AI / プレイヤーの分岐を持つハンドラ
	void HandleAIComboWindow();
	void HandlePlayerComboWindow();
	void RotatingToAttackAndAttack(void (Player::* attackFunc)(), PlayerState nextState);
	// 攻撃開始の共通処理（ヘルパー関数）
	void TryStartAttack(void(Player::* onAttackFunc)(), PlayerState rotationState, PlayerState attackState);
	void UpdateAttackState(AttackSphere& attackData, int animNo, float animInc, PlayerState nextState);
};

