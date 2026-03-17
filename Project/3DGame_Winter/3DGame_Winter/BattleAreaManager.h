#pragma once
#include <DxLib.h>
#include <vector>
#include <memory>
#include "CharacterBase.h"
#include "Player.h"
#include "NormalEnemy.h"
#include "Companion.h"
#include "FlyingEnemy.h"
#include "StrongEnemy.h"
#include "BossEnemy.h"
class BattleAreaManager
{
public:
	BattleAreaManager();
	~BattleAreaManager();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion);
	void SetEnemy(std::vector<std::shared_ptr<NormalEnemy>>& normalEnemies, std::vector<std::shared_ptr<StrongEnemy>>& strongEnemies, std::shared_ptr<BossEnemy> pBossEnemy);
	void Update(std::shared_ptr<CharacterBase> activeCharacter, std::vector<std::shared_ptr<NormalEnemy>>& normalEnemies, std::vector<std::shared_ptr<StrongEnemy>>& strongEnemies);
	void DebugDraw();
	bool IsInBattle(); // 戦闘中か
	bool IsFinished(); // 戦闘が終了しているか
	void DrawBattleAreaBodary();
	bool IsBattleAreaActive() { return m_batlleAreaActive; }
	VECTOR GetCenterPos() { return m_battleAreaCenterPos; }
private:
	enum class State
	{
		None, // 戦闘中でない 
		InBattle, // 戦闘中
		Finish // 戦闘終了
	};

	bool CheckEncounter(std::shared_ptr<CharacterBase> activeCharacter,
		const std::vector<std::shared_ptr<NormalEnemy>>& normalEnemies,
		const std::vector<std::shared_ptr<StrongEnemy>>& strongEnemies);
	// 戦闘開始
	void EnterBattle(const VECTOR& centerPos,
		const std::vector<std::shared_ptr<NormalEnemy>>& normalEnemies,
		const std::vector<std::shared_ptr<StrongEnemy>>& strongEnemies,float radius);

	// キャラクターの移動制限

	void ConstraintCharacterMovement(std::shared_ptr<CharacterBase> character);

	//  バトルが終わっているかをチェック
	void CheckBattleEnd();
	State m_battleState;

	VECTOR m_battleAreaCenterPos;	// バトルエリアの中心座標
	float m_battleAreaRadius;		// バトルエリアの半径
	float m_battleTriggerDistance;	// 戦闘開始判定の距離

	// 計算用の一時変数（メンバとして保持）
	float m_distToNormal;
	float m_distToStrong;
	float m_battleAreaCenterPosToPlayerDistance;
	VECTOR m_afterCorrectionPos;
	bool m_batlleAreaActive;

	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Companion> m_pCompanion;
	std::shared_ptr<BossEnemy> m_pBossEnemy;
	std::vector<std::shared_ptr<NormalEnemy>> m_normalenemies;
	std::vector<std::shared_ptr<StrongEnemy>> m_strongEnemies;
	// 現在バトルエリア内にいる敵のリスト
	std::vector<std::shared_ptr<NormalEnemy>> m_activeNormalenemies;
	std::vector<std::shared_ptr<StrongEnemy>> m_activeStrongEnemies;
};

