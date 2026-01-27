#pragma once
#include "DxLib.h"
#include <vector>
#include <memory>
class Player;
class Companion;
class EffectManager;
class Chest
{
public:
	Chest();
	~Chest() {};
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<EffectManager> pEffectManager);
	void End();
	void Update();
	void Draw();
	void SetBattleActiveFlag(bool flag) { m_isBattleActive = flag; };
private:
	enum class ChestType {HP,SG,BUFF};
	VECTOR m_pos;
	bool m_isBattleActive;
	bool m_isOpened;
	int m_chestHandle;
	struct ChestData
	{
		VECTOR pos;
		ChestType type;
		bool isOpened;
		int modelHandle;
		float rotation;
		float alpha;
	};
	std::vector<ChestData> m_chests;
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Companion> m_pCompanion;
	std::shared_ptr<EffectManager> m_pEffectManager;
};

