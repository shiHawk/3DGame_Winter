#include "Stage.h"
namespace
{
	constexpr VECTOR kStartTilePos = {-800.0f,-10.0f,800.0f};
	constexpr float kTileSize = 400.0f; // タイルサイズ
	constexpr int kMaxTileNum = 26;
	constexpr int kWallNumPerSide = 5; // 1辺あたりの壁の数
	constexpr int kTotalWallNum = kWallNumPerSide * 4;
	constexpr float kWallOffset = kTileSize * 0.5f;
	constexpr VECTOR kTestTilePos = { 600.0f,130.0f,-600.0f };
	constexpr VECTOR kWallRotY90 = { 0.0f, DX_PI_F / 2.0f, 0.0f };
}

Stage::Stage():
	m_tileTotal(0),
	m_stageStart(0),
	m_stageEnd(0),
	m_tileModelBase(-1),
	m_wallModelBase(-1),
	m_tileSize(0.0f),
	m_tilePos({0.0f,0.0f,0.0f}),
	m_tileStartPos({ 0.0f,0.0f,0.0f }),
	m_wallPos({ 0.0f,0.0f,0.0f }),
	m_stairsHandle(-1)
{
}

void Stage::Init()
{
	m_tilePos = kStartTilePos;
	m_tileSize = kTileSize;
	m_tileTotal = kMaxTileNum;
	m_tileModelBase = MV1LoadModel(L"Data/model/floor_dirt_large.mv1");
	m_wallModelBase = MV1LoadModel(L"Data/model/wall.mv1");
	m_stairsHandle = MV1LoadModel(L"Data/model/stairs.mv1");
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

	m_wallModelHandles.resize(kTotalWallNum, -1);
	for (int i = 0; i < kTotalWallNum; i++)
	{
		m_wallModelHandles[i] = MV1DuplicateModel(m_wallModelBase);
	}
	m_wallPos.y = kStartTilePos.y;
	int wallIndex = 0; // m_wallModelHandles のインデックス
	m_wallPos.z = kStartTilePos.z + kWallOffset; // 800 + 200 = 1000
	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.x = kStartTilePos.x + (kTileSize * i); // -800, -400, 0, 400, 800

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		MV1SetupCollInfo(m_wallModelHandles[wallIndex], -1);
		wallIndex++;
	}
	// Z最小 (-800) からさらに手前へ
	m_wallPos.z = (kStartTilePos.z - kTileSize * (kWallNumPerSide - 1)) - kWallOffset; // (800 - 400*4) - 200 = -1000
	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.x = kStartTilePos.x + (kTileSize * i); // -800, -400, 0, 400, 800

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		MV1SetupCollInfo(m_wallModelHandles[wallIndex], -1);
		wallIndex++;
	}

	m_wallPos.x = kStartTilePos.x - kWallOffset; // -800 - 200 = -1000
	// Y軸に90度回転 (モデルがZ軸方向に長い場合)

	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.z = kStartTilePos.z - (kTileSize * i); // 800, 400, 0, -400, -800

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		MV1SetRotationXYZ(m_wallModelHandles[wallIndex], kWallRotY90); // 回転を適用
		MV1SetupCollInfo(m_wallModelHandles[wallIndex], -1);
		wallIndex++;
	}

	// X最大 (800) からさらに右へ
	m_wallPos.x = (kStartTilePos.x + kTileSize * (kWallNumPerSide - 1)) + kWallOffset; // (-800 + 400*4) + 200 = 1000
	// 回転は左の壁と同じ (kWallRotY90)

	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.z = kStartTilePos.z - (kTileSize * i); // 800, 400, 0, -400, -800

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		MV1SetRotationXYZ(m_wallModelHandles[wallIndex], kWallRotY90); // 回転を適用
		MV1SetupCollInfo(m_wallModelHandles[wallIndex], -1);
		wallIndex++;
	}
	MV1TerminateCollInfo(m_tileModelBase); // 複製元モデルの当たり判定情報は不要なので破棄
	MV1TerminateCollInfo(m_wallModelBase); // 複製元モデルの当たり判定情報は不要なので破棄
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


	for (int wallModelHandle : m_wallModelHandles)
	{
		if (wallModelHandle != -1)
		{
			MV1TerminateCollInfo(wallModelHandle);
			MV1DeleteModel(wallModelHandle);
		}
	}
	m_wallModelHandles.clear();
	MV1DeleteModel(m_wallModelBase);
	MV1DeleteModel(m_stairsHandle);
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
	for (int wallModelHandle : m_wallModelHandles)
	{
		MV1DrawModel(wallModelHandle);
	}
}
