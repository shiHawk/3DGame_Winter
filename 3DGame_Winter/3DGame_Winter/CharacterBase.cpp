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
	m_enemyPos({ 0.0f,0.0f,0.0f }),
	m_isJump(false),
	m_isHitFlag(false),
	m_invincibilityTimer(0.0f),
	m_forwardDir({ 0.0f,0.0f,0.0f }),
	m_angleY(0.0f),
	m_isSpecialSkilFlag(false),
	m_prevPos({ 0.0f,0.0f,0.0f }),
	m_specialGauge(0),
	m_baseAttackPower(0),
	m_powerUpBonus(0),
	m_colRadius(0.0f),
	m_isGrounded(true),
	m_maxHp(0),
	m_maxSpecialGauge(0),
	m_changePowerUpBonus(0)
{
}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::AddHp(int recovery)
{
	m_hp += recovery;
	if (m_hp > m_maxHp) m_hp = m_maxHp;
}

void CharacterBase::AddSg(int sg)
{
	m_specialGauge += sg;
	if (m_specialGauge > m_maxSpecialGauge) m_specialGauge = m_maxSpecialGauge;
}

float CharacterBase::GetColRadius()
{
	return m_colRadius;
}