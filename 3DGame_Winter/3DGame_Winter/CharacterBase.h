#pragma once
#include "DxLib.h"
class CharacterBase
{
public:
	CharacterBase();
	virtual ~CharacterBase();
	virtual void Update() = 0;
	bool IsDead() { return m_isDead; }
	VECTOR GetPos() { return m_pos; }
protected:
	virtual void OnAttack() abstract;
	VECTOR m_pos;
	VECTOR m_vec;
	int m_hp;
	int m_attackPower;
	bool m_isDead;
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

