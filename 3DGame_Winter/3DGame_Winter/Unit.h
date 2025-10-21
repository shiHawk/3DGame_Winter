#pragma once
#include "vector"
class Unit
{
protected:
	struct Status
	{
		int maxHp; //最大HP
		int currentHp; //現在のHP
		int attackPower; //攻撃力
		int actionSpeed; //行動速度
		int maxActivePoint; //アクティブポイント
		int maxPassivePont; //パッシブポイント
		int currentActivePoint; //現在のアクティブポイント
		int currentPassivePont; //現在のパッシブポイント
		int hit; //命中力
		int evasion; //回避値
	};
	Status m_baseStatus;
	Status m_currentStatus;
private:
	enum UnitClass
	{
		LORD,THIEF,ARCHER,ARMORKNIGHT,WIZARD,KNIGHT,SOLDIER,NONE
	};
public:
	Unit();
	virtual ~Unit() {};
	int GetCurrentHp() { return m_currentStatus.currentHp; }
	int GetPower() { return m_baseStatus.attackPower; }
	void TakeDamage(int damage);
	void ResetForBattle();
	UnitClass m_unitClass;
	enum class TargetCondition
	{
		PRIORITIZE_TARGETING_THE_THIEF, // THIEFを優先して狙う
		PRIORITIZE_TARGETING_THE_ARMORKNIGHT, // ARMORKNIGHTを優先して狙う
		PRIORITIZE_TARGETING_THE_KNIGHT, // KNIGHTを優先して狙う
		ENEMIES_LINED_UP_IN_FRONT_AND_BEHIND, // 前後列に並んだ敵
		ENEMY_IN_THE_BACK_ROW, // 後列の敵
		HP_LOWEST_ALLY // 最もHPが少ない味方ユニット
	};
	struct Operation
	{
		int m_skilIndex; // 実行するスキルID
		TargetCondition m_condisiton; // ターゲット選択条件
	};
	std::vector<Operation> operations; // 作戦リスト
};
