#pragma once
#include "CharacterBase.h"
#include "Player.h"
#include "Companion.h"
#include "NormalEnemy.h"
#include "StrongEnemy.h"
#include "DxLib.h"
#include <memory>
#include <vector>
class GameplayCollision
{
public:
	GameplayCollision();
	~GameplayCollision();
	void Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<NormalEnemy> pNormalEnemy, std::shared_ptr<StrongEnemy> pStrongEnemy);
	void End();
	void Update();
	void Draw();
	void CheckPlayerAttack();
	void CheckCompanionAttack();
	void CheckNormalEnemyAttack();
	void CheckStrongEnemyRangeAttack(VECTOR attackCenter, float attackRadius,int enemyPower);
	void PushBackCharacter(VECTOR pos1,float pos1Radius, VECTOR pos2, float pos2Radius,CharacterBase* pTargetCharacter);
private:
	std::shared_ptr<Player> m_pPlayer;
	std::shared_ptr<Companion> m_pCompanion;
	std::shared_ptr<NormalEnemy> m_pNormalEnemy;
	std::shared_ptr<StrongEnemy> m_pStrongEnemy;

	struct CollisionOverlapData // 衝突によるめり込み(オーバーラップ)と押し戻しに関するデータ
	{
		VECTOR m_penetrationVector; // キャラクター同士の重なっている部分を示すベクトル量
		float m_penetrationVectorSize; // m_penetrationVectorの大きさ
		VECTOR m_pushDir; // 押し戻す方向
		VECTOR m_pushBack; // 実際に押し戻される距離
		float m_overLapSize; // 押し戻しをするかを判断するための値
	};
	struct HitDetectionInfo // キャラクター間の攻撃判定・距離に関する情報
	{
		VECTOR m_deltaVector; // / 攻撃元から被攻撃対象へ向かうベクトル
		float m_distance; // 2点間の距離の大きさ
		VECTOR m_hitPos; // 攻撃が当たった位置のワールド座標
	};
	CollisionOverlapData m_overLapData;
};

