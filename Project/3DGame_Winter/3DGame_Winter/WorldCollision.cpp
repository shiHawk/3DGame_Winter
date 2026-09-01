#include "WorldCollision.h"
#include "Player.h"
#include "Companion.h"
#include "Stage.h"
#include "NormalEnemy.h"
#include "StrongEnemy.h"
#include <cmath>
#include <cfloat>
namespace
{
	constexpr float kGroundCheckRayOffsetY = 50.0f; // レイの開始Y座標オフセット
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
	constexpr int kMaxIterations = 4; // // 複数の壁に挟まれた際の補正回数（1回だと角などでめり込むため）
	constexpr float kCameraWallOffset = 1.0f; // 壁からわずかに離す距離
	constexpr float kCameraCollisionMargin = 10.0f; // 壁の手前で止める余白
}
WorldCollision::WorldCollision():
	m_lastGroundY(0.0f)
{
}

WorldCollision::~WorldCollision()
{
}

void WorldCollision::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Stage> pStage, std::shared_ptr<Companion> pCompanion,
						  std::vector<std::shared_ptr<NormalEnemy>> pNormalEnemies, std::vector<std::shared_ptr<StrongEnemy>> pStrongEnemies)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	m_pStage = pStage;
	m_pNormalEnemies = pNormalEnemies;
	m_pStrongEnemies = pStrongEnemies;
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
	for (auto& enemy : m_pNormalEnemies)
	{
		if (enemy) // 有効なポインタかチェック
		{
			CheckGroundCollision(enemy.get());
			CheckWallCollision(enemy.get());
		}
	}

	for (auto& enemy : m_pStrongEnemies)
	{
		if (enemy)
		{
			CheckGroundCollision(enemy.get());
			CheckWallCollision(enemy.get());
		}
	}
}

VECTOR WorldCollision::CheckCameraCollision(const VECTOR& pivotPos, const VECTOR& idealCameraPos, float radius)
{
	VECTOR toCameraVec = VSub(idealCameraPos, pivotPos);
	float idealDistance = VSize(toCameraVec);
	if (idealDistance <= 0.0001f)
	{
		return idealCameraPos; // 支点と理想位置がほぼ同じなら何もしない
	}
	VECTOR dir = VScale(toCameraVec, 1.0f / idealDistance);

	// 遮るものが無ければ理想距離のまま
	float closestDistance = idealDistance;

	// 支点から理想カメラ位置までの線分と、指定したポリゴン群との当たり判定を行い
	// 一番手前でヒットした距離を採用する
	auto checkSegment = [&](const std::vector<int>& handles)
		{
			for (int handle : handles)
			{
				if (handle == -1) continue;

				MV1_COLL_RESULT_POLY result = MV1CollCheck_Line(handle, -1, pivotPos, idealCameraPos);
				if (result.HitFlag == 1)
				{
					VECTOR hitVec = VSub(result.HitPosition, pivotPos);
					// 進行方向(dir)への射影で「支点からの距離」に変換
					float hitDist = VDot(hitVec, dir);
					// 半径+余白の分だけ手前で止める(壁にレンズがめり込まないように)
					hitDist -= (radius + kCameraCollisionMargin);
					if (hitDist < 0.0f) hitDist = 0.0f;

					if (hitDist < closestDistance)
					{
						closestDistance = hitDist;
					}
				}
			}
		};

	checkSegment(m_pStage->GetWallCollisionModelHandles());
	checkSegment(m_pStage->GetGroundCollisionModelHandles()); // 階段・床の裏抜け防止

	return VAdd(pivotPos, VScale(dir, closestDistance));
}

void WorldCollision::CheckGroundCollision(CharacterBase* pTargetCharacter)
{
	// 必要な情報の取得
	VECTOR playerPos = pTargetCharacter->GetPos(); // プレイヤーの現在の座標
	VECTOR nextPos = VAdd(playerPos, pTargetCharacter->GetVec()); // 次のフレームの予想位置
	const auto& tileHandles = m_pStage->GetGroundCollisionModelHandles(); // ステージのタイル
	// レイを定義
	VECTOR rayStart = nextPos;
	rayStart.y += kGroundCheckRayOffsetY;
	VECTOR rayEnd = nextPos;
	rayEnd.y -= kGroundCheckRayLength;
	// 当たり判定の準備
	bool isGrounded = false;
	float highestGroundY = -99999.0f;
	VECTOR highestGroundNormal = VGet(0.0f, 1.0f, 0.0f);
	// ステージのタイルとレイの当たり判定を実行
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

	if (isGrounded && highestGroundY > -99999.0f)
	{
		m_lastGroundY = highestGroundY;
	}
	
	if (isGrounded)
	{
		// 1. もし足元が地面より下にあるなら、強制的に地面の上に押し戻す（突き抜け防止）
		if (nextPos.y < highestGroundY)
		{
			nextPos.y = highestGroundY;

			// 2. もし落下中（VecY <= 0）なら、着地処理
			if (pTargetCharacter->GetVec().y <= 0.0f)
			{
				pTargetCharacter->SetVecY(0.0f);
				pTargetCharacter->SetIsJump(false);
			}
		}
		pTargetCharacter->SetIsGrounded(true);
	}
	else
	{
		pTargetCharacter->SetIsGrounded(false);
	}
	pTargetCharacter->SetPos(nextPos);
}

void WorldCollision::CheckWallCollision(CharacterBase* pTargetCharacter)
{
	VECTOR characterPos = pTargetCharacter->GetPos();
	VECTOR currentVec = pTargetCharacter->GetVec(); // 速度もループ内で更新するためここで取得
	const auto& wallHandles = m_pStage->GetWallCollisionModelHandles(); // ステージの壁
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
							//printfDx(L"Hit\n\n");
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
