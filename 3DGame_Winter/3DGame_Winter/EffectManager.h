#pragma once
#include "DxLib.h"
#include <memory>
#include <vector>
#include "CharacterBase.h"
class Player;
class Companion;
class EffectManager
{
public:
	EffectManager();
	~EffectManager();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion);
	void End();
	void Update();
	void Draw();
	void SetSpecialSkilEffect(float x,float y, float z);
private:
	int m_meleeSpecialEffectHandle; // 近接キャラの必殺技エフェクト
	int m_rangedSpecialEffectHandle; // 遠距離キャラの必殺技エフェクト
	int m_playerEffectHandle;
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Companion> m_pCompanion;
	bool m_isMeleeSpecialEffect;
	bool m_isRangedSpecialEffect;
};

