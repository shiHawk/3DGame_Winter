#pragma once
#include "CharacterBase.h"
#include "Player.h"
#include "Companion.h"
#include "DxLib.h"
#include <memory>
class Enemy :public CharacterBase
{
public:
	Enemy();
	virtual ~Enemy() = default;
	virtual void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion);
	virtual void End();
	virtual void Update()= 0;
	virtual void Draw() = 0;
	virtual void OnAttack();
	virtual void OnDamage(int damage);
protected:
	void SearchTarget();
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Companion> m_pCompanion;
	// プレイヤーに向かうベクトル
	VECTOR m_toPlayerDir;
	float m_toPlayerDistance;
	// コンパニオンに向かうベクトル
	VECTOR m_toCompanionDir;
	VECTOR m_targetPos;
	float m_toCompanionDistance;
	float m_AttackCoolTime;
};

