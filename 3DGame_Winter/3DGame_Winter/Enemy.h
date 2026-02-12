#pragma once
#include "CharacterBase.h"
#include "Player.h"
#include "Companion.h"
#include "DxLib.h"
#include <memory>
#include <vector>
struct DamageResult
{
	VECTOR pos;
	int damage;
	unsigned int color;
};
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
	virtual void OnDamage(int damage,bool isFromPlayerDamage);
	bool IsAttackCharge() { return m_isAttackCharge; }
	// 攻撃者別の無敵時間を取得
	float GetInvincibilityByAttacker(bool isPlayer); 
	std::vector<DamageResult> PopDamageResults();
protected:
	void SearchTarget();
	bool CheckAndSetInvincibility(bool isPlayer);
	void UpdateInvincibilityTimer();
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Companion> m_pCompanion;
	std::vector<DamageResult> m_damageResults; // 発生したダメージのリスト
	// プレイヤーに向かうベクトル
	VECTOR m_toPlayerDir;
	float m_toPlayerDistance;
	// コンパニオンに向かうベクトル
	VECTOR m_toCompanionDir;
	VECTOR m_targetPos;
	float m_toCompanionDistance;
	float m_AttackCoolTime;
	float m_playerHate;
	float m_companionHate;
	float m_playerInvincibilityTimer;
	float m_companionInvincibilityTimer;
	bool m_isAttackCharge;
	float m_deathTimer;
	int m_finalDamage;  // ダメージの減衰や累加を計算した後実際に与えるダメージの値
	unsigned int m_damageColor;
};

