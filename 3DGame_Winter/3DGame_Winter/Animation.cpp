#include "Animation.h"
#include "DxLib.h"

namespace
{
	constexpr float kBlendRateIncrement = 0.09f;
}

Animation::Animation() :
	m_animTotalTime(0),
	m_playTime(0.0f),
	m_oldAttachNo(-1),
	m_currentAttachNo(-1),
	m_nextAttachNo(-1),
	m_isLoop(true),
	m_isEnd(false),
	m_modelHandle(-1),
	m_timeIncrement(0.0f),
	m_isNowPlaying(false),
	m_currentPlayTime(0.0f),
	m_nextPlayTime(0.0f),
	m_attachAnimNo(-1),
	m_blendRate(0.0f),
	m_isBlending(false),
	m_currentAnimNo(0),
	m_isOldLoop(false),
	m_oldAnimTotalTime(0.0f),
	m_oldPlayTime(0.0f)
{
}

Animation::~Animation()
{
}

void Animation::Init()
{
	m_animTotalTime = 0.0f;
	m_playTime = 0.0f;
	m_isLoop = true;
	m_isEnd = false;
	m_timeIncrement = 0.0f;
	m_isNowPlaying = false;
}

void Animation::AttachAnim(int modelHandle, int animNo)
{
	m_modelHandle = modelHandle;
	m_currentAttachNo = MV1AttachAnim(modelHandle, animNo);
	m_animTotalTime = MV1GetAttachAnimTotalTime(modelHandle, m_currentAttachNo);
	m_currentAnimNo = animNo;
}

void Animation::UpdateAnim()
{
	// 再生時間をセットする
	MV1SetAttachAnimTime(m_modelHandle, m_currentAttachNo, m_currentPlayTime);
	if (m_isBlending)
	{
		// ブレンド中の処理 
		m_blendRate += kBlendRateIncrement; // 任意の値でブレンド率を上げる 
		// (1)ブレンド元(old)の再生時間を進める
		if (m_oldAttachNo != -1)
		{
			m_oldPlayTime += m_timeIncrement;
			if (m_oldPlayTime >= m_oldAnimTotalTime)
			{
				m_oldPlayTime = m_isOldLoop ? 0.0f : m_oldAnimTotalTime;
			}
			MV1SetAttachAnimTime(m_modelHandle, m_oldAttachNo, m_oldPlayTime);
		}

		// (2)現在(current)の再生時間を進める
		if (m_currentAttachNo != -1)
		{
			m_currentPlayTime += m_timeIncrement;
			if (m_currentPlayTime >= m_animTotalTime)
			{
				if (m_isLoop)
				{
					m_currentPlayTime = 0.0f;
					m_isEnd = false;
				}
				else
				{
					m_currentPlayTime = m_animTotalTime;
					m_isEnd = true;
				}
			}
			MV1SetAttachAnimTime(m_modelHandle, m_currentAttachNo, m_currentPlayTime);
		}
		// (3)ブレンド率を更新し、完了判定を行う
		if (m_blendRate >= 1.0f)
		{
			m_blendRate = 1.0f;
			m_isBlending = false;
			if (m_oldAttachNo != -1)
			{
				MV1DetachAnim(m_modelHandle, m_oldAttachNo);
				m_oldAttachNo = -1;
			}
			MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAttachNo, 1.0f);
		}
		else
		{
			MV1SetAttachAnimBlendRate(m_modelHandle, m_oldAttachNo, 1.0f - m_blendRate);
			MV1SetAttachAnimBlendRate(m_modelHandle, m_currentAttachNo, m_blendRate);
		}
	}
	else
	{
		// 通常再生の処理
		if (m_currentAttachNo == -1) return;
		m_currentPlayTime += m_timeIncrement;
		if (m_currentPlayTime >= m_animTotalTime)
		{
			if (m_isLoop)
			{
				m_currentPlayTime = 0.0f;
				m_isEnd = false;
			}
			else
			{
				m_currentPlayTime = m_animTotalTime; // 最後のフレームで止める
				m_isEnd = true;
			}
		}
		MV1SetAttachAnimTime(m_modelHandle, m_currentAttachNo, m_currentPlayTime);
	}
	
}

void Animation::ChangeAnim(int modelHandle, int animNo, bool isLoop, float increment)
{
	// アタッチされているアニメーションと同じアニメーションがすでに再生されている場合はアニメーションの変更を行わない
	if ((m_currentAnimNo == animNo && m_timeIncrement != 0.0f) || m_isBlending) return; 
	m_currentAnimNo = animNo;
	// 現在の再生情報を「ブレンド元(old)」として保持
	m_oldAttachNo = m_currentAttachNo;
	m_oldPlayTime = m_currentPlayTime;      // 現在の再生時間を退避
	m_oldAnimTotalTime = m_animTotalTime;   // 現在の総時間も退避
	m_isOldLoop = m_isLoop;                 // ループ情報も退避

	// 新しいアニメーションをアタッチし、こちらを「現在(current)」とする
	m_currentAttachNo = MV1AttachAnim(modelHandle, animNo);

	// 新しいアニメーションの情報を設定
	m_animTotalTime = MV1GetAttachAnimTotalTime(modelHandle, m_currentAttachNo);
	m_currentPlayTime = 0.0f;
	m_isLoop = isLoop;
	m_timeIncrement = increment;
	m_isEnd = false;
	// ブレンドを開始する
	m_isBlending = true;
	m_blendRate = 0.0f;
	// 初回などで古いアニメーションがない場合はブレンドしない
	if (m_oldAttachNo == -1)
	{
		m_isBlending = false;
		MV1SetAttachAnimBlendRate(modelHandle, m_currentAttachNo, 1.0f);
		return;
	}
	// 各アニメーションのブレンド率を初期設定
	MV1SetAttachAnimBlendRate(modelHandle, m_oldAttachNo, 1.0f);
	MV1SetAttachAnimBlendRate(modelHandle, m_currentAttachNo, 0.0f);
}

bool Animation::GetIsAnimEnd()
{
	if (m_currentAttachNo == -1) return false;
	if (m_currentPlayTime >= m_animTotalTime) return true;
	//return m_currentPlayTime >= m_animTotalTime;
}