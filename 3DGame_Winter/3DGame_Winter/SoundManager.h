#pragma once
#include "DxLib.h"
#include <memory>
class SceneManager;
class SoundManager
{
public:
	// シングルトンインスタンスを取得するメソッド
	static SoundManager* GetInstance()
	{
		static SoundManager instance; // 一度だけインスタンスが呼ばれる
		return &instance;
	}
	// コピーコンストラクタと代入演算子を禁止
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;

	void Init(SceneManager* pSceneManager);
	void End();
	void Update();
	void PlayBGM();
	void PlayWarriorAttackSE(int attackCount); // SEの再生
	void PlayEnemyAttackSE();
	void PlayWizardAttackSE();
	void PlayWarriorSpecialAttackSE();
	void PlayWizardSpecialAttackSE();
	int FadeBGMVol(); // フェードに合わせて音量を小さくする
	void StopBGM();
	void SwitchToBossBGM();
	void PlayChestSE(int no);
	void PlayGameStart();
private:
	SoundManager();
	~SoundManager() {};
	SceneManager* m_pSceneManager;
	int m_volumeReduction; // 音量の減少量
	int m_currentBgmHandle;
	int m_titleBgmHandle;
	int m_gameSceneBgmHandle;
	int m_resultBgmHandle;
	int m_playerFirstAttackSoundHandle; // 一段目の攻撃
	int m_playerSecondAttackSoundHandle; // 二段目の攻撃
	int m_playerThirdAttackSoundHandle; // 三段目の攻撃
	int m_playerSpecialSkilSoundHandle;
	int m_companionNormalAttackSoundHandle; 
	int m_companionStrongAttackSoundHandle;
	int m_companionSpecialSkilSoundHandle;
	int m_enemyAttackSoundHandle;
	int m_enterSEHandle;
	int m_PlayerChangeSEHandle;
	int m_wizardAttackSound;
	int m_bossBgmHandle;
	int m_recoverHpHandle;
	int m_addSpecialGaugeHandle;
	int m_powerUpHandle;
	int m_gameStartHandle;
	bool m_isBossBgmRequested; // ボスBGMへの切り替え中フラグ
};

