#include "ScoreManager.h"

namespace
{
	constexpr int kMaxTimeBonus = 1000;                // タイムボーナス計算の上限基準
	constexpr float kMillisecondsPerSecond = 1000.0f;  // ミリ秒から秒への変換
	constexpr int kTimeBonusMultiplier = 20;           // 経過時間をボーナスに変換する倍率
}

ScoreManager::ScoreManager():
	m_startTime(0),
	m_elapsedTimeSeconds(0),
	m_timeBonus(0)
{
}

void ScoreManager::Init()
{
	m_startTime = GetNowCount();
	m_elapsedTimeSeconds = 0;
}

void ScoreManager::End()
{
	m_startTime = 0;
	m_elapsedTimeSeconds = 0;
}

void ScoreManager::Updata()
{
	m_elapsedTimeSeconds = static_cast<int>((GetNowCount() - m_startTime) / kMillisecondsPerSecond);
	m_timeBonus = (kMaxTimeBonus - m_elapsedTimeSeconds) * kTimeBonusMultiplier;
	//printfDx(L"m_elapsedTimeSeconds:%d\n",m_elapsedTimeSeconds);
}

int ScoreManager::GetTimeBonus()
{
	if (m_timeBonus <= 0)
	{
		m_timeBonus = 0;
	}
	return m_timeBonus;
}
