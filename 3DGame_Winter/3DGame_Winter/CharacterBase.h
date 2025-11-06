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
protected:
	virtual void OnAttack() abstract;
	enum class ControlMode
	{
		PLAYER,
		COMPANION
	};
	VECTOR m_pos;
	VECTOR m_vec;
	VECTOR m_moveInput;
	int m_hp;
	int m_attackPower;
	bool m_isDead;
	bool m_isInAttackSequence; // UŒ‚’†‚©•ûŒü•â³’†‚©
	int m_modelHandle;
	struct AttackSphere
	{
		float radius; // UŒ‚‚Ì”»’è‚Ì‘å‚«‚³
		VECTOR pos;
		bool active; // UŒ‚ó‘Ô
		float timer; // UŒ‚ŠÔ
		VECTOR dir;
	};
};

