#include "Animation.h"
#include "DxLib.h"

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
	m_attachAnimNo(-1)
{
}

Animation::~Animation()
{
}

void Animation::Init()
{
}

void Animation::AttachAnim(int modelHandle, int animNo)
{
	m_modelHandle = modelHandle;
	m_currentAttachNo = MV1AttachAnim(modelHandle, animNo);
	m_animTotalTime = MV1GetAttachAnimTotalTime(modelHandle, m_currentAttachNo);
}

void Animation::UpdateAnim()
{
	// �Đ����Ԃ�i�߂�
	m_currentPlayTime += m_timeIncrement;
	// �Đ����Ԃ��Z�b�g����
	MV1SetAttachAnimTime(m_modelHandle, m_currentAttachNo, m_currentPlayTime);
	
	if (m_currentPlayTime >= m_animTotalTime)
	{
		if (m_isLoop)
		{
			m_currentPlayTime = 0.0f;
			m_isEnd = false;
		}
		else
		{
			m_currentPlayTime = m_animTotalTime; // �Ō�̃t���[���Ŏ~�߂�
			m_isEnd = true;
		}
	}
}

void Animation::ChangeAnim(int modelHandle, int animNo, bool isLoop, float increment)
{
	if (m_currentAttachNo == animNo) return;
	// ���̃A�j���[�V�������A�^�b�`
	m_nextAttachNo = MV1AttachAnim(modelHandle, animNo);
	// ���A�^�b�`����Ă���A�j���[�V�������f�^�b�`
	MV1DetachAnim(modelHandle, m_currentAttachNo);
	m_currentAttachNo = m_nextAttachNo;
	m_isLoop = isLoop;
	// �Đ����Ԃ����Z�b�g
	m_currentPlayTime = 0.0f;
	m_timeIncrement = increment;
	// ���Đ����Ԃ��Đݒ�
	m_animTotalTime = MV1GetAttachAnimTotalTime(modelHandle, m_nextAttachNo);
	m_isEnd = false;
	m_attachAnimNo = animNo;
}

bool Animation::GetIsAnimEnd()
{
	if (m_currentAttachNo == -1) return false;
	return m_currentPlayTime >= m_animTotalTime;
}
