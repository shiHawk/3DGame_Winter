#pragma once
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
		LORD,THIEF,ARCHER,ARMORKNIGHT,WIZARD,KNIGHT,SOLDIER
	};
public:
	Unit(UnitClass u_class, const Status& base_status);
	virtual ~Unit() {};
	int GetCurrentHp() { return m_currentStatus.currentHp; }
	int GetPower() { return m_baseStatus.attackPower; }
	void TakeDamage(int damage);
	void ResetForBattle();
	UnitClass m_unitClass;
};
