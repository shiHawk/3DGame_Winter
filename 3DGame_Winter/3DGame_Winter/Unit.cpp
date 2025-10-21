#include "Unit.h"

Unit::Unit():
	m_baseStatus(0,0,0,0,0,0,0,0,0,0),
	m_currentStatus(0,0,0,0,0,0,0,0,0,0),
	m_unitClass(NONE)
{
	m_currentStatus.currentHp = m_baseStatus.maxHp;
	m_currentStatus.currentActivePoint = m_baseStatus.maxActivePoint;
	m_currentStatus.currentPassivePont = m_baseStatus.maxPassivePont;
}

void Unit::TakeDamage(int damage)
{
	m_currentStatus.currentHp -= damage;
	if (m_currentStatus.currentHp < 0)
	{
		m_currentStatus.currentHp = 0;
	}
}

void Unit::ResetForBattle()
{
	m_currentStatus.currentActivePoint = m_baseStatus.maxActivePoint;
	m_currentStatus.currentPassivePont = m_baseStatus.maxPassivePont;
}
