#pragma once
#include "DxLib.h"
#include "Unit.h"
class Unit;
class StrategyMenu
{
public:
	StrategyMenu(Unit unit);
	~StrategyMenu() {};
	void Init();
	void Update();
	void Draw();
	Unit* m_pTargetUnit;
	
private:
	int m_SelectedOperationIndex;
};

