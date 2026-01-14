#include "EffectManager.h"
#include "DxLib.h"
#include <EffekseerForDXLib.h>
#include "Player.h"
#include "Companion.h"
#include "GameplayCollision.h"
namespace
{
	constexpr int kParticleMax = 8000;
	constexpr float kSpecialSkilEffectSize = 100.0f;
	constexpr float kSpecialSkilSpped = 10.0f;
	constexpr float kPlayerHitEffectSize = 30.0f;
}

EffectManager::EffectManager() :
	m_meleeSpecialEffectHandle(-1),
	m_playerEffectHandle(-1),
	m_hitEffectHandle(-1),
	m_isMeleeSpecialEffect(false),
	m_rangedSpecialEffectHandle(-1),
	m_isRangedSpecialEffect(false)
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
	m_playerEffectHandle = -1;
}

void EffectManager::End()
{
	DeleteEffekseerEffect(m_meleeSpecialEffectHandle);
	DeleteEffekseerEffect(m_rangedSpecialEffectHandle);
	DeleteEffekseerEffect(m_hitEffectHandle);
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
		SetSpecialSkilEffect(m_pPlayer->GetPos().x, m_pPlayer->GetPos().y, m_pPlayer->GetPos().z);
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
		SetSpecialSkilEffect(m_pCompanion->GetAttackPos().x, m_pCompanion->GetAttackPos().y, m_pCompanion->GetAttackPos().z);
		if (!m_pCompanion->IsSpecialSkilFlag())
		{
			m_playerEffectHandle = -1;
			m_isRangedSpecialEffect = false;
		}
	}
	UpdateEffekseer3D(); // エフェクトの更新
}

void EffectManager::Draw()
{
	Effekseer_Sync3DSetting(); // Effekseerの表示設定をDXライブラリの設定に同期させる
	DrawEffekseer3D(); // エフェクトの描画
}

void EffectManager::SetSpecialSkilEffect(float x, float y, float z)
{
	SetPosPlayingEffekseer3DEffect(m_playerEffectHandle,x, y, z);
}
