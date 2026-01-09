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
private:
	int m_startTime;
	int m_elapsedTimeSeconds; // Œo‰ßŽžŠÔ(•b)
	int m_timeBonus;
};

