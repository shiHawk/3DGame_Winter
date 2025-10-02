#pragma once
#include "CharacterBase.h"
#include "Player.h"
#include <memory>
class Enemy :public CharacterBase
{
public:
	Enemy();
	~Enemy() {};
	void Init(std::shared_ptr<Player> pPlayer);
	void End();
	virtual void Update() override;
	void Draw();
private:
	std::shared_ptr<Player> m_pPlayer;
	// プレイヤーに向かうベクトル
	VECTOR m_toPlayerDir;
	float m_toPlayerDistance;
};

