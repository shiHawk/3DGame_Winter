#pragma once
#include "DxLib.h"
#include <memory>
#include <vector>
#include "CharacterBase.h"
class Player;
class Companion;
class BossEnemy;
class GameplayCollision;
class EffectManager
{
public:
	EffectManager();
	~EffectManager();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion,std::shared_ptr<GameplayCollision> pGameplayCollision);
	void End();
	void Update();
	void Draw();
	void PlayChangeEffect(VECTOR pos);
	void PlayPlayerAttack1Effect(VECTOR pos, float angleY);
	void PlayPlayerAttack2Effect(VECTOR pos, float angleY);
	void PlayPlayerCombFinishAttackEffect(VECTOR pos, float angleY);
	void EnemyStrongAttackEffect(VECTOR pos);
	void EnemyStrongAttackChargeEffect(VECTOR pos);
	void StrongEnemyRangeAttackEffect(VECTOR pos);
	void BossEnemyRangeAttackEffect(VECTOR pos);
	void BattleAreaEffect(VECTOR pos, float scale);
	void StopBattleAreaEffect();
	void PlayChestEffect(int no);
	void PlayBossDeathEffect(VECTOR pos);
	void StopBossDeathEffect();
	int PlayShieldEffect(VECTOR pos, float angleY);
	void UpdateShieldEffect(int handle, VECTOR pos, float angleY); // エフェクトの向きの更新
	void StopShieldEffect(int handle); // 主に二重呼び出しを止める
private:
	void SetEffectPos(float x, float y, float z);
	int m_meleeSpecialEffectHandle; // 近接キャラの必殺技エフェクト
	int m_rangedSpecialEffectHandle; // 遠距離キャラの必殺技エフェクト
	int m_hitEffectHandle;
	int m_playerEffectHandle;
	int m_CompanionEffectHandle;
	int m_changeEffectHandle;
	int m_playerAttack1Handle;
	int m_playerAttack2Handle;
	int m_playerCombFinishAttackHandle;
	int m_enemyRangeAttackHandle;
	int m_enemyStrongAttackHandle;
	int m_enemyStrongAttackChargeHandle;
	int m_StrongEnemyRangeAttackHandle;
	int m_BossEnemyRangeAttackHandle;
	int m_BattleAreaHandle;
	int m_playingBattleAreaHandle;
	int m_HpChestHandle;
	int m_SgChestHandle;
	int m_BuffChestHandle;
	int m_bossDeathHandle;
	int m_shieldHandle;
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Companion> m_pCompanion;
	std::shared_ptr<GameplayCollision> m_pGameplayCollision;
	bool m_isMeleeSpecialEffect;
	bool m_isRangedSpecialEffect;
	bool m_isCompanionStrongEffect;
};

