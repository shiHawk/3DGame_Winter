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
	void PlayDramroll(bool finFlag);
	// SEの鳴りっぱなし現象を起こさせないためにシーンの切り替わりでSEは止める
	void StopAllSE();
	bool IsPlayingFinishSE();
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
	int m_playerSpecialSkilSoundHandle; // 必殺技
	int m_companionNormalAttackSoundHandle; // 杖殴り
	int m_companionSpecialSkilSoundHandle; // 必殺技
	int m_enemyAttackSoundHandle; // 敵の攻撃
	int m_PlayerChangeSEHandle; // キャラチェンジ
	int m_wizardAttackSound;  // 魔法攻撃
	int m_bossBgmHandle; // ボス戦
	int m_recoverHpHandle; // 回復
	int m_addSpecialGaugeHandle; // SpecialGauge
	int m_powerUpHandle; // バフ
	int m_gameStartHandle; // 決定音
	int m_dramrollHandle;
	int m_dramrollEndHandle;
	bool m_isBossBgmRequested; // ボスBGMへの切り替え中フラグ
};

