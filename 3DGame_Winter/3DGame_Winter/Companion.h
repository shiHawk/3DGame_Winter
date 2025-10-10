#pragma once
#include <memory>
#include "DxLib.h"
#include "CharacterBase.h"
class Companion :public CharacterBase
{
public:
	Companion();
	~Companion();
	void Init();
	void End();
	virtual void Update()override;
	void Draw();
	void SetEnemyPos(VECTOR enemyPos) { m_enemyPos = enemyPos; };
	virtual void OnAttack() override;
private:
	VECTOR m_forwardDir;
	VECTOR m_enemyPos;
	VECTOR m_dirToEnemy;
	VECTOR m_companionToEnemy;
	AttackSphere m_attack;
	float m_distanceToEnemy;
	float m_angleY;
};

