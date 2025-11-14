#include "WorldCollision.h"
#include "Player.h"
#include "Stage.h"
#include <cmath>
namespace
{
	constexpr float kGroundCheckRayOffsetY = 50.0f; // レイの開始Y座標オフセット
	constexpr float kGroundCheckRayLength = 200.0f; // レイの長さ
	constexpr float kGroundMargin = 0.01f; // 地面とのわずかな隙間(めり込み防止)
	constexpr float kGroundCorrectionOffsetY = -8.0f; // 地面抜け時に補正するY座標
	constexpr float kLerpSpeed = 0.3f;
}
WorldCollision::WorldCollision():
	m_lastGroundY(0.0f)
{
}

WorldCollision::~WorldCollision()
{
}

void WorldCollision::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Stage> pStage)
{
	m_pPlayer = pPlayer;
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
	CheckGroundCollision();
}

void WorldCollision::Draw()
{
}

void WorldCollision::CheckGroundCollision()
{
	// 必要な情報の取得
	VECTOR playerPos = m_pPlayer->GetPos(); // プレイヤーの現在の座標
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
	float playerVecY = m_pPlayer->GetVec().y;
	if (isGrounded && highestGroundY > -99999.0f)
	{
		m_lastGroundY = highestGroundY;
	}
	//printfDx(L"m_lastGroundY:%f\n", m_lastGroundY);
	//printfDx(L"playerPosY:%f\n", m_pPlayer->GetPos().y);
	// 判定結果からプレイヤーに反映
	// 地面が見つかり、かつ プレイヤーのY座標が地面より下(またはめり込んでいる)場合
	if (isGrounded && playerPos.y + m_pPlayer->GetVec().y <= highestGroundY)
	{
		// 上昇中でない場合のみ着地判定
		if (playerVecY <= 0.0f)
		{
			float targetY = highestGroundY;
			playerPos.y = targetY;
			m_pPlayer->SetVecY(0.0f);
			m_pPlayer->SetIsJump(false);
		}
	}
	playerPos = VAdd(playerPos, m_pPlayer->GetVec());
	m_pPlayer->SetPos(playerPos);
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

	if (isGrounded)
	{
		DrawLine3D(rayStart,rayEnd,0xff0000);
	}
	else
	{
		DrawLine3D(rayStart, rayEnd, 0x00ff00);
	}
}
