#include "CharacterBase.h"

CharacterBase::CharacterBase():
	m_attackPower(0),
	m_hp(0),
	m_isDead(false),
	m_isInAttackSequence(false),
	m_modelHandle(-1),
	m_pos({0.0f,0.0f,0.0f}),
	m_vec({ 0.0f,0.0f,0.0f }),
	m_moveInput({ 0.0f,0.0f,0.0f })
{
}

CharacterBase::~CharacterBase()
{
}
