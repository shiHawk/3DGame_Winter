#include "SoundManager.h"
#include "SceneManager.h"

namespace
{
	// BGMは少し控えめ（70%程度）に設定
	constexpr int kBgmMasterVol = 140;
	// SEははっきり聞こえるように大きく設定
	constexpr int kSeMasterVol = 255;
	constexpr int kMaxVol = 255;
	constexpr int kFadeStepVolume = 4; // フェードアウト時に1フレームで下げる音量
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
	m_companionSpecialSkilSoundHandle(-1),
	m_enemyAttackSoundHandle(-1),
	m_PlayerChangeSEHandle(-1),
	m_wizardAttackSound(-1),
	m_bossBgmHandle(-1),
	m_recoverHpHandle(-1),
	m_addSpecialGaugeHandle(-1),
	m_powerUpHandle(-1),
	m_gameStartHandle(-1),
	m_dramrollHandle(-1),
	m_dramrollEndHandle(-1),
	m_isBossBgmRequested(false)
{
}

void SoundManager::Init(SceneManager* pSceneManager)
{
	m_pSceneManager = pSceneManager;
	m_titleBgmHandle = LoadSoundMem(L"Data/sound/titlebgm.mp3");
	m_resultBgmHandle = LoadSoundMem(L"Data/sound/resultbgm.mp3");
	m_gameSceneBgmHandle = LoadSoundMem(L"Data/sound/gamebgm.mp3");
	m_playerFirstAttackSoundHandle = LoadSoundMem(L"Data/sound/firstattack.mp3");
	m_playerSecondAttackSoundHandle = LoadSoundMem(L"Data/sound/secondattack.mp3");
	m_playerThirdAttackSoundHandle = LoadSoundMem(L"Data/sound/thirdattack.mp3");
	m_enemyAttackSoundHandle = LoadSoundMem(L"Data/sound/enemyattack.mp3");
	m_wizardAttackSound = LoadSoundMem(L"Data/sound/wizardattack.mp3");
	m_companionSpecialSkilSoundHandle = LoadSoundMem(L"Data/sound/wizardspecialskil.mp3");
	m_playerSpecialSkilSoundHandle = LoadSoundMem(L"Data/sound/warriorspesialskil.mp3");
	m_bossBgmHandle = LoadSoundMem(L"Data/sound/bossbattle.mp3");
	m_recoverHpHandle = LoadSoundMem(L"Data/sound/recoverhp.mp3");
	m_addSpecialGaugeHandle = LoadSoundMem(L"Data/sound/specialgauge.mp3");
	m_powerUpHandle = LoadSoundMem(L"Data/sound/powerup.mp3");
	m_gameStartHandle = LoadSoundMem(L"Data/sound/gamestart.mp3");
	m_dramrollHandle = LoadSoundMem(L"Data/sound/dramroll.mp3");
	m_dramrollEndHandle = LoadSoundMem(L"Data/sound/dramrollend.mp3");
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
	DeleteSoundMem(m_wizardAttackSound);
	DeleteSoundMem(m_companionSpecialSkilSoundHandle);
	DeleteSoundMem(m_playerSpecialSkilSoundHandle);
	DeleteSoundMem(m_bossBgmHandle);
	DeleteSoundMem(m_recoverHpHandle);
	DeleteSoundMem(m_addSpecialGaugeHandle);
	DeleteSoundMem(m_powerUpHandle);
	DeleteSoundMem(m_gameStartHandle);
	DeleteSoundMem(m_dramrollHandle);
	DeleteSoundMem(m_dramrollEndHandle);
	SoundManager::GetInstance()->StopBGM();
}

void SoundManager::Update()
{
	if (m_currentBgmHandle != -1)
	{
		// フェードアウト処理
		if (m_isBossBgmRequested)
		{
			m_volumeReduction += kFadeStepVolume;
			if (m_volumeReduction >= kBgmMasterVol) // 完全に音が消えたら
			{
				StopBGM();
				m_currentBgmHandle = m_bossBgmHandle;
				m_volumeReduction = 0;
				m_isBossBgmRequested = false;
				PlaySoundMem(m_currentBgmHandle, DX_PLAYTYPE_LOOP, true);
			}
		}

		// kBgmMasterVol(180) からフェード分を引く
		int vol = kBgmMasterVol - m_volumeReduction;
		if (vol < 0) vol = 0;
		if (m_currentBgmHandle == m_gameSceneBgmHandle)
		{
			ChangeVolumeSoundMem(vol, m_currentBgmHandle);
		}
		else
		{
			ChangeVolumeSoundMem(kMaxVol - m_volumeReduction, m_currentBgmHandle);
		}
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
		PlaySoundMem(m_currentBgmHandle, DX_PLAYTYPE_LOOP, true); // 現在のBGMを再生
	}
}

void SoundManager::PlayWarriorAttackSE(int attackCount)
{
	switch (attackCount)
	{
	case 0:
		PlaySoundMem(m_playerFirstAttackSoundHandle, DX_PLAYTYPE_BACK);
		break;
	case 1:
		PlaySoundMem(m_playerSecondAttackSoundHandle, DX_PLAYTYPE_BACK);
		break;
	case 2:
		PlaySoundMem(m_playerThirdAttackSoundHandle, DX_PLAYTYPE_BACK);
		break;
	}
	
}

void SoundManager::PlayEnemyAttackSE()
{
	ChangeVolumeSoundMem(kSeMasterVol, m_enemyAttackSoundHandle);
	PlaySoundMem(m_enemyAttackSoundHandle, DX_PLAYTYPE_BACK);
}

void SoundManager::PlayWizardAttackSE()
{
	PlaySoundMem(m_playerThirdAttackSoundHandle, DX_PLAYTYPE_BACK);
}

void SoundManager::PlayWarriorSpecialAttackSE()
{
	PlaySoundMem(m_playerSpecialSkilSoundHandle, DX_PLAYTYPE_BACK);
}

void SoundManager::PlayWizardSpecialAttackSE()
{
	PlaySoundMem(m_companionSpecialSkilSoundHandle, DX_PLAYTYPE_BACK);
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

void SoundManager::SwitchToBossBGM()
{
	if (m_currentBgmHandle == m_bossBgmHandle) return; // 既にボス戦なら何もしない
	m_isBossBgmRequested = true;
}

void SoundManager::PlayChestSE(int no)
{
	int soundHandle = -1;
	if (no == 0) soundHandle = m_addSpecialGaugeHandle;
	else if (no == 1) soundHandle = m_recoverHpHandle;
	else soundHandle = m_powerUpHandle;

	PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK);
}

void SoundManager::PlayGameStart()
{
	PlaySoundMem(m_gameStartHandle, DX_PLAYTYPE_BACK);
}

void SoundManager::PlayDramroll(bool finFlag)
{
	if (!finFlag)
	{
		if (CheckSoundMem(m_dramrollHandle) == 0) 
		{
			PlaySoundMem(m_dramrollHandle, DX_PLAYTYPE_LOOP);
		}
	}
	else
	{
		StopSoundMem(m_dramrollHandle); // カウント中のループ音を止める
		PlaySoundMem(m_dramrollEndHandle, DX_PLAYTYPE_BACK); // 終了音を再生
	}
}

bool SoundManager::IsPlayingFinishSE()
{
	return CheckSoundMem(m_dramrollEndHandle) == 1;
}
