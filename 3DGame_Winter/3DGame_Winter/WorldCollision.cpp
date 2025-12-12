#include "WorldCollision.h"
#include "Player.h"
#include "Companion.h"
#include "Stage.h"
#include <cmath>
namespace
{
	constexpr float kGroundCheckRayOffsetY = 10.0f; // レイの開始Y座標オフセット
	constexpr float kGroundCheckRayLength = 50000.0f; // 地面に伸ばすレイの長さ
	constexpr float kWallCheckRayLength = 80.0f; // 正面に伸ばすレイの長さ
	constexpr float kGroundMargin = 0.01f; // 地面とのわずかな隙間(めり込み防止)
	constexpr float kGroundCorrectionOffsetY = -8.0f; // 地面抜け時に補正するY座標
	constexpr float kLerpSpeed = 0.3f;
	constexpr float kCharacterRadius = 40.0f;

	constexpr float kFrontLimit = -1000.0f; // ステージ奥
	constexpr float kBackLimit = 1000.0f;   // ステージ手前
	constexpr float kLeftLimit = -1000.0f;  // ステージ左
	constexpr float kRightLimit = 1000.0f;  // ステージ右
	constexpr float kWallOffset = 0.001f;
	constexpr float kCollisionThresholdSq = (kCharacterRadius + kWallOffset) * (kCharacterRadius + kWallOffset);
	constexpr int kMaxIterations = 4;
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
	CheckWallCollision(m_pPlayer.get());
	CheckWallCollision(m_pCompanion.get());
}

void WorldCollision::Draw()
{
}

void WorldCollision::CheckGroundCollision(CharacterBase* pTargetCharacter)
{
	// 必要な情報の取得
	VECTOR playerPos = pTargetCharacter->GetPos(); // プレイヤーの現在の座標
	const auto& tileHandles = m_pStage->GetCollisionObjectModelHandles(); // ステージの全タイル
	// レイを定義
	VECTOR rayStart = playerPos;
	rayStart.y += kGroundCheckRayOffsetY;
	VECTOR rayEnd = playerPos;
	rayEnd.y -= kGroundCheckRayLength;
	// 当たり判定の準備
	bool isGrounded = false;
	float highestGroundY = -99999.0f;
	VECTOR highestGroundNormal = VGet(0.0f, 1.0f, 0.0f);
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
				highestGroundNormal = result.Normal;
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
	// // プレイヤーの現在のY軸速度を取得
	VECTOR currentVec = pTargetCharacter->GetVec();
	playerVecY = currentVec.y;
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
			// 地面の法線と現在の移動ベクトルの内積を計算
			float dot = VDot(currentVec, highestGroundNormal);
			// 地面にめり込む方向の速度成分（法線方向の成分）を打ち消す
			// これにより、ベクトルが地面に平行な成分（坂に沿った方向）になる
			VECTOR velocityAlongNormal = VScale(highestGroundNormal, dot);
			VECTOR newVec = VSub(currentVec, velocityAlongNormal);
			// 調整されたXZ成分を速度に反映
			pTargetCharacter->SetVecX(newVec.x);
			pTargetCharacter->SetVecZ(newVec.z);
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
	VECTOR currentVec = pTargetCharacter->GetVec(); // 速度もループ内で更新するためここで取得
	const auto& wallHandles = m_pStage->GetCollisionObjectModelHandles(); // ステージの全ての壁
	for (int iter = 0; iter < kMaxIterations; iter++)
	{
		// 当たり判定の準備
		bool isWallHit = false;
		// 最も近い衝突結果を保持するための変数
		MV1_COLL_RESULT_POLY closestResult = { 0 };
		// キャラクター中心から衝突点までの距離の2乗 (めり込みが深いほど値が小さくなる)
		float minDistanceSq = 999999.0f;

		// ステージの壁と球の当たり判定を行う
		for (int handle : wallHandles)
		{
			if (handle == -1) continue;

			// 球とモデル(handle)の当たり判定 
			MV1_COLL_RESULT_POLY_DIM  result = MV1CollCheck_Sphere(handle, -1, characterPos, kCharacterRadius);

			if (result.HitNum > 0)
			{
				for (int i = 0; i < result.HitNum; i++)
				{
					// result.Dim[i] で i番目に当たったポリゴンの情報にアクセス
					if (result.Dim[i].HitFlag == 1)
					{
						VECTOR hitVec = VSub(result.Dim[i].HitPosition, characterPos);
						float distSq = VDot(hitVec, hitVec);

						if (distSq < minDistanceSq)
						{
							isWallHit = true;
							minDistanceSq = distSq;
							// 最も近いポリゴンの情報をコピーして保存しておく
							closestResult = result.Dim[i];
						}
					}
				}
				MV1CollResultPolyDimTerminate(result); // 当たり判定結果ポリゴン配列の後始末
			}
		}

		if (isWallHit)
		{
			// 押し戻し位置の計算
			// 衝突点 (closestResult.HitPosition) と壁の法線 (closestResult.Normal) を使用
			// キャラクターの中心を壁から (kCharacterRadius + kWallOffset) 分離れた位置に設定
			VECTOR pushbackVec = VScale(closestResult.Normal, kCharacterRadius + kWallOffset);
			VECTOR newPos = VAdd(closestResult.HitPosition, pushbackVec);

			// 現在のY座標は維持し、X,Z座標を更新する
			characterPos.x = newPos.x;
			characterPos.z = newPos.z;
			pTargetCharacter->SetPos(characterPos);

			// 移動量の調整 (壁にめり込む方向の速度を打ち消す)
			VECTOR currentVec = pTargetCharacter->GetVec();
			// 速度ベクトルと壁の法線との内積 (速度が法線と逆方向(めり込み方向)なら dot < 0)
			float dot = VDot(currentVec, closestResult.Normal);

			if (dot < 0.0f)
			{
				// 壁にめり込む方向の速度成分を打ち消す
				VECTOR velocityAlongNormal = VScale(closestResult.Normal, dot);
				currentVec = VSub(currentVec, velocityAlongNormal);
				pTargetCharacter->SetVec(currentVec);
			}
		}
	}
}

void WorldCollision::MoveCharacter(CharacterBase* pTargetCharacter)
{
	VECTOR pos = pTargetCharacter->GetPos();
	pos = VAdd(pos,pTargetCharacter->GetVec());
	pTargetCharacter->SetPos(pos);
	CheckWallCollision(pTargetCharacter);
	CheckGroundCollision(pTargetCharacter);
}
