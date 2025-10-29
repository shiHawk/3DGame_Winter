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
	void SetPlayerPos(VECTOR playerPos) { m_playerPos = playerPos; };
	virtual void OnAttack() override;
	void UpdateCompanionState();
private:
	enum class CompanionState
	{
		NORMAL,
		FOLLOW_PLAYER,
		TRACK_ENEMY,
		NORML_ATTACK,
		STRONG_ATTACK,
		SPECIALSKIL
	};
	CompanionState m_companionState;
	VECTOR m_forwardDir;
	VECTOR m_enemyPos;
	VECTOR m_playerPos;
	VECTOR m_dirToEnemy;
	VECTOR m_dirToPlayer;
	VECTOR m_companionToEnemy;
	VECTOR m_companionToPlayer;
	AttackSphere m_attack;
	float m_distanceToEnemy;
	float m_distanceToPlayer;
	float m_angleY;
};

