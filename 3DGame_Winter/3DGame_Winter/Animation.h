#pragma once
class Animation
{
public:
	Animation();
	~Animation();
	void Init();
	void AttachAnim(int modelHandle, int animNo);
	void UpdateAnim(int modelHandle);
	void ChangeAnim(int modelHandle, int animNo, bool isLoop, float increment);
	bool isNowPlaying() { return m_isNowPlaying; }
	bool GetIsAnimEnd();
	int GetAttachAnimNo() { return m_attachAnimNo; }
	float GetPlayTime() { return m_currentPlayTime; }
private:
	float m_playTime;
	float m_nextPlayTime;
	// 以前のアニメーションのアタッチ番号
	int m_oldAttachNo;
	float m_oldPlayTime;        // ブレンド元の再生時間
	float m_oldAnimTotalTime;   // ブレンド元の総再生時間
	bool  m_isOldLoop;          // ブレンド元のループ情報
	int m_currentAttachNo; // 現在のアニメーションのアタッチ番号
	int m_currentAnimNo; // 現在のアニメーションの番号
	float m_currentPlayTime; // 再生時間
	float m_animTotalTime; // 総再生時間
	// アニメーションをループさせるか否か
	bool m_isLoop;
	// 次のアニメーションのアタッチ番号
	int m_nextAttachNo;
	// modelのハンドル
	int m_modelHandle;
	// アニメーションが終了しているか
	bool m_isEnd;
	// 再生時間の増分
	float m_timeIncrement;
	bool m_isNowPlaying; // 現在アニメーションを再生中か
	int m_attachAnimNo; // 現在アタッチされているアニメーションの番号
	// ブレンド関係
	float m_blendRate;
	bool m_isBlending;
};

