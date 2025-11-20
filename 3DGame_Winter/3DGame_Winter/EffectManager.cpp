#include "EffectManager.h"
#include "DxLib.h"
#include <EffekseerForDXLib.h>
#include "Player.h"
namespace
{
	constexpr int kParticleMax = 8000;
	constexpr float kSpecialSkilEffectSize = 80.0f;
}

EffectManager::EffectManager() :
	m_effectHandle(-1),
	m_playerEffectHandle(-1),
	m_isPlayingSpecialEffect(false)
{
}

EffectManager::~EffectManager()
{
}

void EffectManager::Init(std::shared_ptr<Player> pPlayer)
{
	m_pPlayer = pPlayer;
	m_effectHandle = LoadEffekseerEffect(L"Data/effect/SpecialSkil.efk");
	m_playerEffectHandle = -1;
}

void EffectManager::End()
{
	DeleteEffekseerEffect(m_effectHandle);
}

void EffectManager::Update()
{
	if (m_pPlayer->IsSpecialSkilFlag() && !m_isPlayingSpecialEffect)
	{
		m_playerEffectHandle = PlayEffekseer3DEffect(m_effectHandle);
		SetScalePlayingEffekseer3DEffect(m_playerEffectHandle, kSpecialSkilEffectSize, kSpecialSkilEffectSize, kSpecialSkilEffectSize);
		m_isPlayingSpecialEffect = true;
	}
	if (m_isPlayingSpecialEffect)
	{
		SetSpecialSkilEffect();
		if (!m_pPlayer->IsSpecialSkilFlag())
		{
			m_playerEffectHandle = -1;
			m_isPlayingSpecialEffect = false;
		}
	}
	UpdateEffekseer3D(); // エフェクトの更新
}

void EffectManager::Draw()
{
	Effekseer_Sync3DSetting(); // Effekseerの表示設定をDXライブラリの設定に同期させる

	DrawEffekseer3D(); // エフェクトの描画
}

void EffectManager::SetSpecialSkilEffect()
{
	SetPosPlayingEffekseer3DEffect(m_playerEffectHandle,m_pPlayer->GetPos().x, m_pPlayer->GetPos().y, m_pPlayer->GetPos().z);
}
