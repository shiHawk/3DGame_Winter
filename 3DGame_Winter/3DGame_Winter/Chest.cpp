#include "Chest.h"
#include "Player.h"
#include "Companion.h"
#include "EffectManager.h"
#include "SoundManager.h"
#include "ScoreManager.h"
namespace
{
	constexpr float kModelScale = 0.75f;
	constexpr float kColRadius = 120.0f;
	constexpr VECTOR kChest1Pos = {644.3f,-59.0f,1516.6f};
	constexpr VECTOR kChest2Pos = {1087.9f,339.9f,9297.2f};
	constexpr VECTOR kChest3Pos = {2694.0f,1135.9f,5639.0f};
	constexpr float kChest1Rotation = -DX_PI_F / 2;
	constexpr float kChest2Rotation = 0.0f;
	constexpr float kChest3Rotation = DX_PI_F;
	constexpr int kChestAddSgAmount = 50;
	constexpr int kRecoverHP = 400;
	constexpr int kChestAttackUpValue = 30;
}
Chest::Chest():
	m_pos({0.0f,0.0f,0.0f}),
	m_isBattleActive(false),
	m_isOpened(false),
	m_chestHandle(-1)
{
	m_chests.push_back({ kChest1Pos,ChestType::SG,false,-1,kChest1Rotation,1.0f,0 });
	m_chests.push_back({ kChest2Pos,ChestType::HP,false,-1,kChest2Rotation,1.0f,1 });
	m_chests.push_back({ kChest3Pos,ChestType::BUFF,false,-1,kChest3Rotation,1.0f,2 });
}

void Chest::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, 
				 std::shared_ptr<EffectManager> pEffectManager,std::shared_ptr<ScoreManager> pScoreManager)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	m_pEffectManager = pEffectManager;
	m_pScoreManager = pScoreManager;
	m_chestHandle = MV1LoadModel(L"Data/model/chest.mv1");
	for (auto& chest : m_chests)
	{
		chest.modelHandle = MV1DuplicateModel(m_chestHandle);
		MV1SetScale(chest.modelHandle, VGet(kModelScale, kModelScale, kModelScale));
		MV1SetPosition(chest.modelHandle, chest.pos);
		MV1SetRotationXYZ(chest.modelHandle,VGet(0.0f,chest.rotation,0.0f));

		COLOR_F colorScale;
		switch (chest.type)
		{
		case ChestType::HP:
			colorScale = GetColorF(1.0f, 1.5f, 1.0f, 1.0f);
			break;
		case ChestType::SG:
			colorScale = GetColorF(2.0f, 2.0f, 1.0f, 1.0f);
			break;
		case ChestType::BUFF:
			colorScale = GetColorF(2.0f, 1.0f, 1.0f, 1.0f);
			break;
		default:
			colorScale = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
		MV1SetDifColorScale(chest.modelHandle, colorScale);
	}
}

void Chest::End()
{
	for (auto& chest : m_chests)
	{
		MV1DeleteModel(chest.modelHandle);
	}
	// 大元のハンドルを削除
	MV1DeleteModel(m_chestHandle);
}

void Chest::Update()
{
	for (auto& chest : m_chests) 
	{
		if (chest.isOpened)
		{
			if (chest.alpha > 0.0f) 
			{
				chest.alpha -= 0.02f; 
				if (chest.alpha < 0.0f) chest.alpha = 0.0f;

				// 不透明度を更新
				MV1SetOpacityRate(chest.modelHandle, chest.alpha);
			}
			continue; // すでに開いていたら無視
		}

		std::shared_ptr<CharacterBase> characters[] = { m_pPlayer, m_pCompanion };

		for (auto& ch : characters)
		{
			VECTOR chPos = ch->GetPos();
			VECTOR vec = VSub(chPos, chest.pos); // 宝箱からキャラへのベクトル
			float dist = VSize(vec);

			// 判定半径以内なら押し戻す
			if (dist < kColRadius)
			{
				// 0除算防止（完全に重なっている場合）
				if (dist < 0.001f) vec = VGet(0, 0, 1);

				// 押し出す方向ベクトル（長さ1）
				VECTOR normal = VNorm(vec);
				// 押し戻す量（半径 - 現在の距離）
				float pushLen = kColRadius - dist;
				// 新しい座標 = 現在の座標 + (方向 * 押し戻す量)
				VECTOR newPos = VAdd(chPos, VScale(normal, pushLen));

				// キャラクターの座標を更新
				ch->SetPos(newPos);
			}
		}

		// 当たり判定（距離の計算）
		float distPlayer = VSize(VSub(chest.pos, m_pPlayer->GetPos()));
		float distCompanion = VSize(VSub(chest.pos, m_pCompanion->GetPos()));
		if (distPlayer < kColRadius || distCompanion < kColRadius)
		{
			if (m_isBattleActive)
			{
				continue;
			}
			else
			{
				chest.isOpened = true;
				m_pScoreManager->AddTreasureCount();
				SoundManager::GetInstance()->PlayChestSE(chest.chestNo);
			}
			if (chest.type == ChestType::SG && chest.isOpened)
			{
				m_pPlayer->AddSg(kChestAddSgAmount);
				m_pCompanion->AddSg(kChestAddSgAmount);
				m_pEffectManager->PlayChestEffect(0);
			}
			if (chest.type == ChestType::HP)
			{
				m_pPlayer->AddHp(kRecoverHP);
				m_pCompanion->AddHp(kRecoverHP);
				m_pEffectManager->PlayChestEffect(1);
			}
			if (chest.type == ChestType::BUFF)
			{
				m_pPlayer->PowerUp(kChestAttackUpValue);
				m_pCompanion->PowerUp(kChestAttackUpValue);
				m_pEffectManager->PlayChestEffect(2);
			}
		}
	}
}

void Chest::Draw()
{
	// 個別の宝箱を描画する
	for (const auto& chest : m_chests)
	{
		MV1DrawModel(chest.modelHandle);
	}
}
