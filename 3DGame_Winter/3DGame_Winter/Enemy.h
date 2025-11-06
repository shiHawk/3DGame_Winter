#pragma once
#include "CharacterBase.h"
#include "Player.h"
#include "DxLib.h"
#include <memory>
class Enemy :public CharacterBase
{
public:
	Enemy();
	virtual ~Enemy() = default;
	virtual void Init(std::shared_ptr<Player> pPlayer);
	virtual void End();
	virtual void Update()= 0;
	virtual void Draw() = 0;
	virtual void OnAttack();
protected:
	std::shared_ptr<Player> m_pPlayer;
	// プレイヤーに向かうベクトル
	VECTOR m_toPlayerDir;
	float m_toPlayerDistance;
};

