#include "CharacterBase.h"
#include <cmath>

CharacterBase::CharacterBase():
	m_attackPower(0),
	m_hp(0),
	m_isDead(false),
	m_isDying(false),
	m_isInAttackSequence(false),
	m_modelHandle(-1),
	m_pos({0.0f,0.0f,0.0f}),
	m_vec({ 0.0f,0.0f,0.0f }),
	m_moveInput({ 0.0f,0.0f,0.0f }),
	m_isJump(false),
	m_isHitFlag(false),
	m_invincibilityTimer(0.0f),
	m_forwardDir({ 0.0f,0.0f,0.0f }),
	m_angleY(0.0f),
	m_isSpecialSkilFlag(false),
	m_prevPos({ 0.0f,0.0f,0.0f }),
	m_specialGauge(0),
	m_baseAttackPower(0),
	m_powerUpBonus(0)
{
}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::ApplyMovement()
{
	m_prevPos = m_pos;           // Å© à⁄ìÆëO
	m_pos = VAdd(m_pos, m_vec);  // Å© à⁄ìÆ
}
