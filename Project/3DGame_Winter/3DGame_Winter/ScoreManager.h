#pragma once
#include "DxLib.h"
class ScoreManager
{
public:
	ScoreManager();
	~ScoreManager() {};
	void Init();
	void End();
	void Updata();
	int GetTime() { return m_elapsedTimeSeconds; }
	int GetTimeBonus();
	void SetGameoverFlag(bool gameover) { m_gameoverFlag = gameover; }
	void AddTreasureCount() { m_treasureCount++; }
	int GetTreasureCount() const { return m_treasureCount; }
private:
	int m_startTime;
	int m_elapsedTimeSeconds; // Œo‰ßŽžŠÔ(•b)
	int m_timeBonus;
	bool m_gameoverFlag;
	int m_treasureCount;
};

