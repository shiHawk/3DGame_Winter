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
	Unit m_targetUnit;
private:

};

