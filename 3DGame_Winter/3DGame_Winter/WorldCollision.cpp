#include "WorldCollision.h"
#include "Player.h"
#include "Companion.h"
#include "Stage.h"
#include <cmath>
namespace
{
	constexpr float kGroundCheckRayOffsetY = 50.0f; // レイの開始Y座標オフセット
	constexpr float kGroundCheckRayLength = 200.0f; // 地面に伸ばすレイの長さ
	constexpr float kWallCheckRayLength = 50.0f; // 正面に伸ばすレイの長さ
	constexpr float kGroundMargin = 0.01f; // 地面とのわずかな隙間(めり込み防止)
	constexpr float kGroundCorrectionOffsetY = -8.0f; // 地面抜け時に補正するY座標
	constexpr float kLerpSpeed = 0.3f;
	constexpr float kCharacterRadius = 20.0f;

	constexpr float kFrontLimit = -1000.0f; // ステージ奥
	constexpr float kBackLimit = 1000.0f;   // ステージ手前
	constexpr float kLeftLimit = -1000.0f;  // ステージ左
	constexpr float kRightLimit = 1000.0f;  // ステージ右
	constexpr float kWallOffset = 0.001f;
	constexpr float kCollisionThresholdSq = (kCharacterRadius + kWallOffset) * (kCharacterRadius + kWallOffset);
}
WorldCollision::WorldCollision():
	m_lastGroundY(0.0f)
{
}

WorldCollision::~WorldCollision()
{
}

void WorldCollision::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Stage> pStage, std::shared_ptr<Companion> pCompanion)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	m_pStage = pStage;
}

void WorldCollision::End()
{
}

void WorldCollision::Update()
{
	// PlayerやStageが正しく設定されていなければ処理しない
	if (!m_pPlayer || !m_pStage)
	{
		return;
	}
	CheckGroundCollision(m_pPlayer.get());
	CheckGroundCollision(m_pCompanion.get());
	//CheckWallCollision(m_pPlayer.get());
}

void WorldCollision::Draw()
{
}

void WorldCollision::CheckGroundCollision(CharacterBase* pTargetCharacter)
{
	// 必要な情報の取得
	VECTOR playerPos = pTargetCharacter->GetPos(); // プレイヤーの現在の座標
	const auto& tileHandles = m_pStage->GetTileModelHandles(); // ステージの全タイル
	// レイを定義
	VECTOR rayStart = playerPos;
	rayStart.y += kGroundCheckRayOffsetY;
	VECTOR rayEnd = playerPos;
	rayEnd.y -= kGroundCheckRayLength;
	// 当たり判定の準備
	bool isGrounded = false;
	float highestGroundY = -99999.0f;
	// ステージの全タイルとレイの当たり判定を実行
	for (int handle : tileHandles)
	{
		if (handle == -1) continue;
		// 線分(rayStart, rayEnd)とモデル(handle)の当たり判定
		MV1_COLL_RESULT_POLY result = MV1CollCheck_Line(handle,-1,rayStart,rayEnd);
		if (result.HitFlag == 1)
		{
			isGrounded = true;
			// 衝突位置のY座標を、見つかった地面のY座標と比較
			if (result.HitPosition.y > highestGroundY)
			{
				highestGroundY = result.HitPosition.y;
			}
		}
	}
	//printfDx(L"highestGroundY:%f\n", highestGroundY);
	// プレイヤーの現在のY軸速度を取得
	float playerVecY = pTargetCharacter->GetVec().y;
	if (isGrounded && highestGroundY > -99999.0f)
	{
		m_lastGroundY = highestGroundY;
	}
	//printfDx(L"m_lastGroundY:%f\n", m_lastGroundY);
	//printfDx(L"playerPosY:%f\n", m_pPlayer->GetPos().y);
	// 判定結果からプレイヤーに反映
	// 地面が見つかり、かつ プレイヤーのY座標が地面より下(またはめり込んでいる)場合
	if (isGrounded && playerPos.y + pTargetCharacter->GetVec().y <= highestGroundY)
	{
		// 上昇中でない場合のみ着地判定
		if (playerVecY <= 0.0f)
		{
			float targetY = highestGroundY;
			playerPos.y = targetY;
			pTargetCharacter->SetVecY(0.0f);
			pTargetCharacter->SetIsJump(false);
		}
	}
	playerPos = VAdd(playerPos, pTargetCharacter->GetVec());
	pTargetCharacter->SetPos(playerPos);
	//else
	//{
	//	// 地面判定失敗時の保険
	//	if (playerPos.y < m_lastGroundY)
	//	{
	//		float targetY = m_lastGroundY;
	//		playerPos.y = targetY;
	//		m_pPlayer->SetPos(playerPos);
	//		m_pPlayer->SetVecY(0.0f);
	//		m_pPlayer->SetIsJump(false);
	//	}
	//	else
	//	{
	//		m_pPlayer->SetIsJump(true);
	//	}
	//}

	/*if (isGrounded)
	{
		DrawLine3D(rayStart,rayEnd,0xff0000);
	}
	else
	{
		DrawLine3D(rayStart, rayEnd, 0x00ff00);
	}*/
}

void WorldCollision::CheckWallCollision(CharacterBase* pTargetCharacter)
{
	VECTOR characterPos = pTargetCharacter->GetPos();
	const auto& wallHandles = m_pStage->GetWallModelHandles(); // ステージの全ての壁
	// レイを定義
	VECTOR rayStart = characterPos;
	VECTOR direction = pTargetCharacter->GetDir(); // キャラクターの向き
	rayStart.y += kGroundCheckRayOffsetY;
	VECTOR rayVec = VScale(direction, kWallCheckRayLength);
	VECTOR rayEnd = VAdd(rayStart,rayVec);
	// 当たり判定の準備
	bool isWallHit = false;
	VECTOR closestHitPos = VGet(0, 0, 0);
	float minHitDistSq = (kWallCheckRayLength * kWallCheckRayLength) + 1.0f; // レイの長さの2乗より大きく初期化
	// ステージの壁とレイの当たり判定を行う
	for (int handle : wallHandles)
	{
		if (handle == -1) continue;
		// 線分(rayStart, rayEnd)とモデル(handle)の当たり判定
		MV1_COLL_RESULT_POLY result = MV1CollCheck_Line(handle, -1, rayStart, rayEnd);
		if (result.HitFlag == 1)
		{
			isWallHit = true;
			// 最も近い衝突位置を探す
			VECTOR hitVec = VSub(result.HitPosition, rayStart);
			float distSq = VDot(hitVec, hitVec); // 距離の2乗
			if (distSq < minHitDistSq)
			{
				minHitDistSq = distSq;
				closestHitPos = result.HitPosition;
			}
		}
	}

	if (isWallHit)
	{
		if (minHitDistSq < kCollisionThresholdSq)
		{
			// 押し戻し位置の計算
		    // ヒット位置(closestHitPos)から、キャラクターの向きと逆方向に kWallOffset だけ戻す
			VECTOR reverseDir = VScale(direction, -1.0f); // 逆向きベクトル
			VECTOR offset = VScale(reverseDir, kCharacterRadius + kWallOffset);
			// ヒット位置+オフセット位置を新しい座標とする
			VECTOR newPos = VAdd(closestHitPos, offset);
			// 現在のY座標は維持し、X,Z座標を更新する
			characterPos.x = newPos.x;
			characterPos.z = newPos.z;
			pTargetCharacter->SetPos(characterPos);

			// 壁に当たったら移動量をリセット
			VECTOR currentVec = pTargetCharacter->GetVec();
			float dot = VDot(currentVec, direction);
			if (dot > 0.0f) // 前に進もうとしている場合のみ
			{
				// 進行方向成分の速度を打ち消す
				VECTOR velocityAlongDir = VScale(direction, dot);
				currentVec = VSub(currentVec, velocityAlongDir);
				pTargetCharacter->SetVec(currentVec);
			}
		}
	}

	if (isWallHit)
	{
		DrawLine3D(rayStart, closestHitPos, 0xff0000); // ヒットした箇所まで赤
	}
	else
	{
		DrawLine3D(rayStart, rayEnd, 0x00ff00); // ヒットしなかったら緑
	}
	printfDx(L"closestHitPos.x:%f\nclosestHitPos.y:%f\nclosestHitPos.z:%f\n\n", closestHitPos.x, closestHitPos.y, closestHitPos.z);
}
