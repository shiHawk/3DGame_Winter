#pragma once
#include "Enemy.h"
#include "Player.h"
#include "Companion.h"
#include "DxLib.h"
class FlyingEnemy :public Enemy
{
public:
	FlyingEnemy();
	~FlyingEnemy();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion);
	void End();
	void Update() override;
	void Draw() override;
	virtual void OnAttack() override;
	void OnDamage();
	float GetColRadius();
	virtual VECTOR GetDir() override;
private:
	AttackSphere m_enemyAttack;
	float m_alpha;
	float m_targetAngle;
	float m_attackToPlayerDistance;
	float m_hoverTimer; // 浮遊アニメーション用タイマー
	VECTOR m_toPlayerDir;
	VECTOR m_attackDir; 
};

