#pragma once
#include "DxLib.h"
#include "Animation.h"
class CharacterBase :public Animation
{
public:
	CharacterBase();
	virtual ~CharacterBase();
	virtual void Update() = 0;
	bool IsDead() { return m_isDead; }
	VECTOR GetPos() { return m_pos; }
	void AddPos(VECTOR offset) { m_pos = VAdd(m_pos, offset); }
	void SetPos(VECTOR newPos) { m_pos = newPos; }
	void SetVecY(float newVecY) { m_vec.y = newVecY; }
	void SetIsJump(bool isJumping) { m_isJump = isJumping; }
	VECTOR GetVec() { return m_vec; }
	enum class ControlMode
	{
		PLAYER,
		COMPANION
	};
protected:
	virtual void OnAttack() abstract;
	VECTOR m_pos;
	VECTOR m_vec;
	VECTOR m_moveInput;
	int m_hp;
	int m_attackPower;
	bool m_isDead;
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
};

