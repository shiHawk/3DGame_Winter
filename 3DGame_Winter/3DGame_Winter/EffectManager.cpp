#include "EffectManager.h"
#include "DxLib.h"
#include <EffekseerForDXLib.h>
#include "Player.h"
#include "Companion.h"
#include "BossEnemy.h"
#include "GameplayCollision.h"
namespace
{
	constexpr int kParticleMax = 8000;
	constexpr float kSpecialSkilEffectSize = 100.0f;
	constexpr float kSpecialSkilSpped = 10.0f;
	constexpr float kPlayerHitEffectSize = 30.0f;
	constexpr float kFireEffectSize = 50.0f;
	constexpr float kChangeEffectSize = 50.0f;
	constexpr float kChangeEffectOffsetY = 50.0f;
	constexpr float kPlayerAttack1Size = 50.0f;
	constexpr float kPlayerAttack2Size = 30.0f;
	constexpr float kPlayerAttack2OffsetY = 30.0f;
	constexpr float kEnemyStrongAttackSize = 60.0f;
	constexpr float kEnemyStrongAttackChargeSize = 200.0f;
	constexpr float kStrongEnemyRangeAttackSize = 35.0f;
	constexpr float kStrongEnemyRangeAttackOffsetY = 80.0f;
	constexpr float kBossEnemyRangeAttackSize = 200.0f;
	constexpr float kBossEnemyRangeAttackOffsetY = 40.0f;
	constexpr float kBattleAreaSize = 120.0f;
}

EffectManager::EffectManager() :
	m_meleeSpecialEffectHandle(-1),
	m_playerEffectHandle(-1),
	m_hitEffectHandle(-1),
	m_isMeleeSpecialEffect(false),
	m_rangedSpecialEffectHandle(-1),
	m_isRangedSpecialEffect(false),
	m_CompanionEffectHandle(-1),
	m_isCompanionStrongEffect(false),
	m_changeEffectHandle(-1),
	m_playerAttack1Handle(-1),
	m_playerAttack2Handle(-1),
	m_playerCombFinishAttackHandle(-1),
	m_enemyRangeAttackHandle(-1),
	m_enemyStrongAttackHandle(-1),
	m_enemyStrongAttackChargeHandle(-1),
	m_StrongEnemyRangeAttackHandle(-1),
	m_BossEnemyRangeAttackHandle(-1),
	m_BattleAreaHandle(-1),
	m_playingBattleAreaHandle(-1)
{
}

EffectManager::~EffectManager()
{
}

void EffectManager::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<GameplayCollision> pGameplayCollision)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	m_pGameplayCollision = pGameplayCollision;
	m_meleeSpecialEffectHandle = LoadEffekseerEffect(L"Data/effect/Aura01.efkefc");
	m_rangedSpecialEffectHandle = LoadEffekseerEffect(L"Data/effect/MagicMeteo.efkefc");
	m_hitEffectHandle = LoadEffekseerEffect(L"Data/effect/AttackHit.efkefc");
	m_CompanionEffectHandle = LoadEffekseerEffect(L"Data/effect/fire.efkefc");
	m_changeEffectHandle = LoadEffekseerEffect(L"Data/effect/ChangeEffect.efkefc");
	m_playerAttack1Handle = LoadEffekseerEffect(L"Data/effect/PlayerAttack1.efkefc");
	m_playerAttack2Handle = LoadEffekseerEffect(L"Data/effect/PlayerAttack2.efkefc");
	m_playerCombFinishAttackHandle = LoadEffekseerEffect(L"Data/effect/playerCombFinishAttack.efkefc");
	m_enemyStrongAttackHandle = LoadEffekseerEffect(L"Data/effect/EnemyStrongAttack.efkefc");
	m_enemyStrongAttackChargeHandle = LoadEffekseerEffect(L"Data/effect/EnemyStrongAttackCharge.efkefc");
	m_StrongEnemyRangeAttackHandle = LoadEffekseerEffect(L"Data/effect/StrongEnemyRangeAttack.efkefc");
	m_BossEnemyRangeAttackHandle = LoadEffekseerEffect(L"Data/effect/BossEnemyRangeAttack.efkefc");
	m_BattleAreaHandle = LoadEffekseerEffect(L"Data/effect/BattleArea.efkefc");
	m_playerEffectHandle = -1;
}

void EffectManager::End()
{
	DeleteEffekseerEffect(m_meleeSpecialEffectHandle);
	DeleteEffekseerEffect(m_rangedSpecialEffectHandle);
	DeleteEffekseerEffect(m_hitEffectHandle);
	DeleteEffekseerEffect(m_CompanionEffectHandle);
	DeleteEffekseerEffect(m_changeEffectHandle);
	DeleteEffekseerEffect(m_playerAttack1Handle);
	DeleteEffekseerEffect(m_playerAttack2Handle);
	DeleteEffekseerEffect(m_playerCombFinishAttackHandle);
	DeleteEffekseerEffect(m_enemyRangeAttackHandle);
	DeleteEffekseerEffect(m_enemyStrongAttackHandle);
	DeleteEffekseerEffect(m_enemyStrongAttackChargeHandle);
	DeleteEffekseerEffect(m_StrongEnemyRangeAttackHandle);
	DeleteEffekseerEffect(m_BossEnemyRangeAttackHandle);
	DeleteEffekseerEffect(m_BattleAreaHandle);
	StopBattleAreaEffect();
}

void EffectManager::Update()
{
	const auto& hitPosList = m_pGameplayCollision->GetHitPositions();
	for (const auto& pos : hitPosList)
	{
		// 1回限りの再生（PlayEffekseer3DEffect）
		int handle = PlayEffekseer3DEffect(m_hitEffectHandle);
		SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y, pos.z);
		SetScalePlayingEffekseer3DEffect(handle, kPlayerHitEffectSize, kPlayerHitEffectSize, kPlayerHitEffectSize);
	}

	if (m_pPlayer->IsSpecialSkilFlag() && !m_isMeleeSpecialEffect)
	{
		m_playerEffectHandle = PlayEffekseer3DEffect(m_meleeSpecialEffectHandle);
		SetScalePlayingEffekseer3DEffect(m_playerEffectHandle, kSpecialSkilEffectSize, kSpecialSkilEffectSize, kSpecialSkilEffectSize);
		m_isMeleeSpecialEffect = true;
	}
	if (m_isMeleeSpecialEffect)
	{
		SetEffectPos(m_pPlayer->GetPos().x, m_pPlayer->GetPos().y, m_pPlayer->GetPos().z);
		if (!m_pPlayer->IsSpecialSkilFlag())
		{
			m_playerEffectHandle = -1;
			m_isMeleeSpecialEffect = false;
		}
	}

	if (m_pCompanion->IsSpecialSkilFlag() && !m_isRangedSpecialEffect)
	{
		m_playerEffectHandle = PlayEffekseer3DEffect(m_rangedSpecialEffectHandle);
		SetScalePlayingEffekseer3DEffect(m_playerEffectHandle, kSpecialSkilEffectSize, kSpecialSkilEffectSize, kSpecialSkilEffectSize);
		m_isRangedSpecialEffect = true;
	}
	if (m_isRangedSpecialEffect)
	{
		SetEffectPos(m_pCompanion->GetAttackPos().x, m_pCompanion->GetAttackPos().y, m_pCompanion->GetAttackPos().z);
		if (!m_pCompanion->IsSpecialSkilFlag())
		{
			m_playerEffectHandle = -1;
			m_isRangedSpecialEffect = false;
		}
	}

	if (m_pCompanion->IsStrongAttackFlag() && !m_isCompanionStrongEffect)
	{
		// エフェクト開始（最初の1回だけ通る）
		m_playerEffectHandle = PlayEffekseer3DEffect(m_CompanionEffectHandle);
		SetScalePlayingEffekseer3DEffect(m_playerEffectHandle, kFireEffectSize, kFireEffectSize, kFireEffectSize);
		m_isCompanionStrongEffect = true;
	}

	if (m_isCompanionStrongEffect)
	{
		// エフェクトの位置を更新し続ける
		SetEffectPos(m_pCompanion->GetAttackPos().x, m_pCompanion->GetAttackPos().y, m_pCompanion->GetAttackPos().z);

		// 攻撃が終わったら管理を終了する
		if (!m_pCompanion->IsStrongAttackFlag())
		{
			m_playerEffectHandle = -1;
			m_isCompanionStrongEffect = false;
		}
	}
	UpdateEffekseer3D(); // エフェクトの更新
}

void EffectManager::Draw()
{
	Effekseer_Sync3DSetting(); // Effekseerの表示設定をDXライブラリの設定に同期させる
	DrawEffekseer3D(); // エフェクトの描画
}

void EffectManager::PlayChangeEffect(VECTOR pos)
{
	// チェンジ用エフェクトを再生
	int handle = PlayEffekseer3DEffect(m_changeEffectHandle);

	// 足元に表示するため,y座標を調整
	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y+ kChangeEffectOffsetY, pos.z);

	SetScalePlayingEffekseer3DEffect(handle, kChangeEffectSize, kChangeEffectSize, kChangeEffectSize);
}

void EffectManager::PlayPlayerAttack1Effect(VECTOR pos, float angleY)
{
	// 攻撃用エフェクトを再生
	int handle = PlayEffekseer3DEffect(m_playerAttack1Handle);

	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y+kChangeEffectOffsetY, pos.z);

	SetRotationPlayingEffekseer3DEffect(handle, 0.0f, angleY, -DX_PI_F);
	SetScalePlayingEffekseer3DEffect(handle, kPlayerAttack1Size, kPlayerAttack1Size, kPlayerAttack1Size);
}

void EffectManager::PlayPlayerAttack2Effect(VECTOR pos, float angleY)
{
	// 攻撃用エフェクトを再生
	int handle = PlayEffekseer3DEffect(m_playerAttack2Handle);

	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y + kPlayerAttack2OffsetY, pos.z);

	SetRotationPlayingEffekseer3DEffect(handle, 0.0f, angleY + DX_PI_F, 0.0f);

	SetScalePlayingEffekseer3DEffect(handle, kPlayerAttack2Size, kPlayerAttack2Size, kPlayerAttack2Size);
}

void EffectManager::PlayPlayerCombFinishAttackEffect(VECTOR pos, float angleY)
{
	// 攻撃用エフェクトを再生
	int handle = PlayEffekseer3DEffect(m_playerCombFinishAttackHandle);

	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y + kPlayerAttack2OffsetY, pos.z);

	SetRotationPlayingEffekseer3DEffect(handle, 0.0f, angleY, 0.0f);

	SetScalePlayingEffekseer3DEffect(handle, kPlayerAttack2Size, kPlayerAttack2Size, kPlayerAttack2Size);
}

void EffectManager::EnemyStrongAttackEffect(VECTOR pos)
{
	// 攻撃用エフェクトを再生
	int handle = PlayEffekseer3DEffect(m_enemyStrongAttackHandle);

	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y + kPlayerAttack2OffsetY, pos.z);

	SetScalePlayingEffekseer3DEffect(handle, kEnemyStrongAttackSize, kEnemyStrongAttackSize, kEnemyStrongAttackSize);
}

void EffectManager::EnemyStrongAttackChargeEffect(VECTOR pos)
{
	// 攻撃用エフェクトを再生
	int handle = PlayEffekseer3DEffect(m_enemyStrongAttackChargeHandle);

	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y + kPlayerAttack2OffsetY, pos.z);

	SetScalePlayingEffekseer3DEffect(handle, kEnemyStrongAttackChargeSize, kEnemyStrongAttackSize, kEnemyStrongAttackChargeSize);
}

void EffectManager::StrongEnemyRangeAttackEffect(VECTOR pos)
{
	// 攻撃用エフェクトを再生
	int handle = PlayEffekseer3DEffect(m_StrongEnemyRangeAttackHandle);

	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y+ kStrongEnemyRangeAttackOffsetY, pos.z);

	SetScalePlayingEffekseer3DEffect(handle, kStrongEnemyRangeAttackSize, kStrongEnemyRangeAttackSize, kStrongEnemyRangeAttackSize);
}

void EffectManager::BossEnemyRangeAttackEffect(VECTOR pos)
{
	// 攻撃用エフェクトを再生
	int handle = PlayEffekseer3DEffect(m_BossEnemyRangeAttackHandle);

	SetPosPlayingEffekseer3DEffect(handle, pos.x, pos.y + kBossEnemyRangeAttackOffsetY, pos.z);

	SetScalePlayingEffekseer3DEffect(handle, kBossEnemyRangeAttackSize, kBossEnemyRangeAttackSize, kBossEnemyRangeAttackSize);
}

void EffectManager::BattleAreaEffect(VECTOR pos)
{
	if (m_playingBattleAreaHandle != -1)
	{
		// 必要であれば位置だけ更新する
		return;
	}

	m_playingBattleAreaHandle = PlayEffekseer3DEffect(m_BattleAreaHandle);

	SetPosPlayingEffekseer3DEffect(m_playingBattleAreaHandle, pos.x, pos.y + kBossEnemyRangeAttackOffsetY, pos.z);
	SetScalePlayingEffekseer3DEffect(m_playingBattleAreaHandle, kBattleAreaSize, kBattleAreaSize, kBattleAreaSize);
}

void EffectManager::StopBattleAreaEffect()
{
	// 再生中なら停止する
	if (m_playingBattleAreaHandle != -1)
	{
		StopEffekseer3DEffect(m_playingBattleAreaHandle);
		m_playingBattleAreaHandle = -1; // ハンドルを無効値に戻す
	}
}

void EffectManager::SetEffectPos(float x, float y, float z)
{
	SetPosPlayingEffekseer3DEffect(m_playerEffectHandle, x, y, z);
}
