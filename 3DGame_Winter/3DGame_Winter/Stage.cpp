#include "Stage.h"
namespace
{
	constexpr VECTOR kStartTilePos = {-800.0f,-10.0f,800.0f};
	constexpr float kTileSize = 400.0f; // タイルサイズ
	constexpr int kMaxTileNum = 26;
	constexpr VECTOR kTestTilePos = { 600.0f,80.0f,-600.0f };
}
Stage::Stage():
	m_tileTotal(0),
	m_stageStart(0),
	m_stageEnd(0),
	m_tileModelBase(-1),
	m_tileSize(0.0f),
	m_tilePos({0.0f,0.0f,0.0f}),
	m_tileStartPos({ 0.0f,0.0f,0.0f })
{
}

void Stage::Init()
{
	m_tilePos = kStartTilePos;
	m_tileSize = kTileSize;
	m_tileTotal = kMaxTileNum;
	m_tileModelBase = MV1LoadModel(L"Data/model/floor_dirt_large.mv1");
	m_tileModelHandles.resize(m_tileTotal,-1);
	// コピー元から複製
	for (int i = 0; i < m_tileTotal;i++)
	{
		m_tileModelHandles[i] = MV1DuplicateModel(m_tileModelBase);
	}
	for (int i = 0; i < m_tileTotal;i++)
	{
		MV1SetPosition(m_tileModelHandles[i], m_tilePos);
		MV1SetupCollInfo(m_tileModelHandles[i],-1);
		m_tilePos.x += kTileSize;
		if ((i + 1) % 5 == 0)
		{
			// x座標を開始位置に戻す
			m_tilePos.x = kStartTilePos.x;
			// z座標をkTileSize分引いて、手前へ移動
			m_tilePos.z -= kTileSize;
			if (i == 24)
			{
				m_tilePos = kTestTilePos;
			}
		}
	}
	MV1TerminateCollInfo(m_tileModelBase); // 複製元モデルの当たり判定情報は不要なので破棄
}

void Stage::End()
{
	for (int tileModelHandle : m_tileModelHandles)
	{
		if (tileModelHandle != -1) 
		{
			MV1TerminateCollInfo(tileModelHandle);
			MV1DeleteModel(tileModelHandle);
		}
	}
	m_tileModelHandles.clear();
	MV1DeleteModel(m_tileModelBase);
}

void Stage::Update()
{
	
}

void Stage::Draw()
{
	for (int i = 0; i < m_tileTotal;i++)
	{
		MV1DrawModel(m_tileModelHandles[i]);
	}
}
