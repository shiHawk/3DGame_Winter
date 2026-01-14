#include "SoundManager.h"
#include "SceneManager.h"

namespace
{
	constexpr int kMaxVol = 255;
	constexpr int kFadeStepVolume = 4;        // フェードアウト時に1フレームで下げる音量
}

SoundManager::SoundManager() :
	m_pSceneManager(nullptr),
	m_volumeReduction(0),
	m_currentBgmHandle(-1),
	m_titleBgmHandle(-1),
	m_gameSceneBgmHandle(-1),
	m_resultBgmHandle(-1),
	m_playerFirstAttackSoundHandle(-1),
	m_playerSecondAttackSoundHandle(-1),
	m_playerThirdAttackSoundHandle(-1),
	m_playerSpecialSkilSoundHandle(-1),
	m_companionNormalAttackSoundHandle(-1),
	m_companionStrongAttackSoundHandle(-1),
	m_companionSpecialSkilSoundHandle(-1),
	m_enemyAttackSoundHandle(-1),
	m_enterSEHandle(-1)
{
}

void SoundManager::Init(SceneManager* pSceneManager)
{
	m_pSceneManager = pSceneManager;
	m_titleBgmHandle = LoadSoundMem(L"Data/sound/titlebgm.mp3");
	m_resultBgmHandle = LoadSoundMem(L"Data/sound/resultbgm.mp3");
}

void SoundManager::End()
{
	DeleteSoundMem(m_titleBgmHandle);
	DeleteSoundMem(m_gameSceneBgmHandle);
	DeleteSoundMem(m_resultBgmHandle);
	DeleteSoundMem(m_playerFirstAttackSoundHandle);
	DeleteSoundMem(m_playerSecondAttackSoundHandle);
	DeleteSoundMem(m_playerThirdAttackSoundHandle);
	DeleteSoundMem(m_enemyAttackSoundHandle);
	SoundManager::GetInstance()->StopBGM();
}

void SoundManager::Update()
{
	if (m_currentBgmHandle != -1)
	{
		ChangeVolumeSoundMem(kMaxVol - m_volumeReduction, m_currentBgmHandle); // 音量を調整
	}
}

void SoundManager::PlayBGM()
{
	m_volumeReduction = 0;
	// 各シーンに合わせてm_currentBgmHandleを変更する
	if (m_pSceneManager->GetCurrentSceneID() == SceneID::TitleScene)
	{
		m_currentBgmHandle = m_titleBgmHandle;
	}
	if (m_pSceneManager->GetCurrentSceneID() == SceneID::GameScene)
	{
		m_currentBgmHandle = m_gameSceneBgmHandle;
	}
	if (m_pSceneManager->GetCurrentSceneID() == SceneID::ResultScene)
	{
		m_currentBgmHandle = m_resultBgmHandle;
	}
	if (m_currentBgmHandle != -1)
	{
		PlaySoundMem(m_currentBgmHandle, DX_PLAYTYPE_LOOP, true);// 現在のBGMを再生
	}
}

void SoundManager::PlayPlayerAttackSE(int attackCount)
{
	switch (attackCount)
	{
	case 1:
		PlaySoundMem(m_playerFirstAttackSoundHandle, DX_PLAYTYPE_BACK);
		break;
	case 2:
		PlaySoundMem(m_playerSecondAttackSoundHandle, DX_PLAYTYPE_BACK);
		break;
	case 3:
		PlaySoundMem(m_playerThirdAttackSoundHandle, DX_PLAYTYPE_BACK);
		break;
	}
}

void SoundManager::PlayCompanionAttackSE(int attackCount)
{
	switch (attackCount)
	{
	case 1:
		PlaySoundMem(m_companionNormalAttackSoundHandle, DX_PLAYTYPE_BACK);
		break;
	case 2:
		PlaySoundMem(m_companionStrongAttackSoundHandle, DX_PLAYTYPE_BACK);
		break;
	case 3:
		PlaySoundMem(m_companionSpecialSkilSoundHandle, DX_PLAYTYPE_BACK);
		break;
	}
}

void SoundManager::PlayEnemyAttackSE()
{
	PlaySoundMem(m_enemyAttackSoundHandle, DX_PLAYTYPE_BACK);
}

int SoundManager::FadeBGMVol()
{
	// 徐々に音量を下げる
	m_volumeReduction += kFadeStepVolume;
	if (m_volumeReduction > kMaxVol)
	{
		m_volumeReduction = kMaxVol;
	}
	return m_volumeReduction;
}

void SoundManager::StopBGM()
{
	if (m_currentBgmHandle != -1) 
	{
		StopSoundMem(m_currentBgmHandle);
		m_currentBgmHandle = -1; // ハンドルをクリアしておく
	}
}
