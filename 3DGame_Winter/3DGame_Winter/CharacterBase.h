#pragma once
#include "DxLib.h"
#include "Animation.h"
class CharacterBase :public Animation
{
public:
	CharacterBase();
	virtual ~CharacterBase();
	virtual void Update() = 0;
	VECTOR GetPos() { return m_pos; }
	VECTOR GetVec() { return m_vec; }
	VECTOR GetPrevPos() { return m_prevPos; }
	void AddPos(VECTOR offset) { m_pos = VAdd(m_pos, offset); }
	void SetPos(VECTOR newPos) { m_pos = newPos; }
	void SetEnemyPos(VECTOR enemyPos) { m_enemyPos = enemyPos; };
	void SetVecX(float newVecX) { m_vec.x = newVecX; }
	void SetVecY(float newVecY) { m_vec.y = newVecY; }
	void SetVecZ(float newVecZ) { m_vec.z = newVecZ; }
	void SetIsJump(bool isJumping) { m_isJump = isJumping; }
	void SetVec(VECTOR vec) { m_vec = vec; }
	virtual VECTOR GetDir() = 0;
	bool IsDead() { return m_isDead; }
	bool IsSpecialSkilFlag() { return m_isSpecialSkilFlag; }
	bool IsJumpFlag() { return m_isJump; }
	bool IsHitFlag() { return m_isHitFlag; }
	int GetHp() { return m_hp; }
	int GetSpecialGauge() { return m_specialGauge; }
	void AddHp(int recovery);
	void AddSg(int sg);
	void PowerUp(int attackPowerValue) { m_powerUpBonus += attackPowerValue; }
	void ChangePowerUp(int attackPowerValue) { m_changePowerUpBonus += attackPowerValue; }
	void ResetPower() { m_changePowerUpBonus = 0; }
	int GetPowerUpBonus() { return m_powerUpBonus; }
	int GetChangePowerUpBonus() { return m_changePowerUpBonus; }
	float GetColRadius();
	enum class ControlMode
	{
		PLAYER,
		COMPANION
	};
	float GetInvincibilityTimer() { return m_invincibilityTimer; }
	void SetIsGrounded(bool grounded) { m_isGrounded = grounded; }
	bool GetIsGrounded() const { return m_isGrounded; }
protected:
	virtual void OnAttack() abstract;
	void ApplyMovement();
	VECTOR m_pos;
	VECTOR m_vec;
	VECTOR m_moveInput;
	VECTOR m_enemyPos;
	int m_hp;
	int m_attackPower;
	bool m_isDead;
	bool m_isDying;
	bool m_isInAttackSequence; // UŒ‚’†‚©•ûŒü•â³’†‚©
	int m_modelHandle;
	bool m_isJump;
	struct AttackSphere
	{
		float radius; // UŒ‚‚Ì”»’è‚Ì‘å‚«‚³
		VECTOR pos;
		bool active; // UŒ‚ó‘Ô
		float timer; // UŒ‚ŠÔ
		VECTOR dir;
	};
	bool m_isHitFlag;
	float m_invincibilityTimer;
	VECTOR m_forwardDir;
	float m_angleY;
	bool m_isSpecialSkilFlag;
	VECTOR m_prevPos;
	int m_specialGauge; // •KEƒQ[ƒW
	int m_baseAttackPower;   // UŒ‚‚Ìí—Ş‚²‚Æ‚ÌŠî‘bUŒ‚—Í
	int m_powerUpBonus;  // •ó” ‚Å“¾‚½‘‰Á•ªi‰Šú’l0j
	int m_changePowerUpBonus;  // ƒLƒƒƒ‰Œğ‘ã‚Å“¾‚½‘‰Á•ªi‰Šú’l0j
	float m_colRadius;
	bool m_isGrounded; // ^‰º‚É’n–Ê‚ª‚ ‚é‚©‚Ç‚¤‚©
	int m_maxHp;           // Å‘åHP
	int m_maxSpecialGauge; // Å‘åƒQ[ƒW
};

