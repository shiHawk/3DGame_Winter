#pragma once
#include "DxLib.h"
#include <memory>
#include <vector>
class Player;
class EffectManager
{
public:
	EffectManager();
	~EffectManager();
	void Init(std::shared_ptr<Player> pPlayer);
	void End();
	void Update();
	void Draw();
	void SetSpecialSkilEffect();
private:
	int m_effectHandle;
	int m_playerEffectHandle;
	std::shared_ptr<Player> m_pPlayer;
	bool m_isPlayingSpecialEffect;
};

