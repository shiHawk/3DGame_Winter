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
	constexpr VECTOR kCollisionScale = { 2.0f,2.0, 0.5f };
	constexpr float kCollisionOffsetY = 60.0f;
}

Stage::Stage():
	m_tileTotal(0),
	m_stageStart(0),
	m_stageEnd(0),
	m_collisionTileModelBase(-1),
	m_tileModelBase(-1),
	m_wallModelBase(-1),
	m_collisionWallBase(-1),
	m_tileSize(0.0f),
	m_tilePos({0.0f,0.0f,0.0f}),
	m_tileStartPos({ 0.0f,0.0f,0.0f }),
	m_wallPos({ 0.0f,0.0f,0.0f }),
	m_collisionWallPos({ 0.0f,0.0f,0.0f }),
	m_stairsHandle(-1)
{
}

void Stage::Init()
{
	m_tilePos = kStartTilePos;
	m_tileSize = kTileSize;
	m_tileTotal = kMaxTileNum;
	m_collisionTileModelBase = MV1LoadModel(L"Data/model/ceiling_tile.mv1");
	m_tileModelBase = MV1LoadModel(L"Data/model/floor_tile_large.mv1");
	m_wallModelBase = MV1LoadModel(L"Data/model/wall.mv1");
	m_collisionWallBase = MV1LoadModel(L"Data/model/cube.mv1");
	m_stairsHandle = MV1LoadModel(L"Data/model/stairs.mv1");
	m_collisionTileModelHandles.resize(m_tileTotal,-1);
	m_tileModelHandles.resize(m_tileTotal, -1);
	// コピー元から複製
	for (int i = 0; i < m_tileTotal;i++)
	{
		m_collisionTileModelHandles[i] = MV1DuplicateModel(m_collisionTileModelBase);
		m_tileModelHandles[i] = MV1DuplicateModel(m_tileModelBase);
		MV1SetVisible(m_collisionTileModelHandles[i], false); // 当たり判定用の床は描画しない
	}
	for (int i = 0; i < m_tileTotal;i++)
	{
		MV1SetPosition(m_collisionTileModelHandles[i], m_tilePos);
		MV1SetupCollInfo(m_collisionTileModelHandles[i],-1);
		MV1SetPosition(m_tileModelHandles[i], m_tilePos);
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
	m_collisionWallModelHandles.resize(kTotalWallNum, -1);
	for (int i = 0; i < kTotalWallNum; i++)
	{
		m_wallModelHandles[i] = MV1DuplicateModel(m_wallModelBase);
		m_collisionWallModelHandles[i] = MV1DuplicateModel(m_collisionWallBase);
		MV1SetScale(m_collisionWallModelHandles[i], kCollisionScale);
		MV1SetVisible(m_collisionWallModelHandles[i],false); // 当たり判定用の壁は描画しない
	}
	m_wallPos.y = kStartTilePos.y;
	int wallIndex = 0; // m_wallModelHandles のインデックス
	m_wallPos.z = kStartTilePos.z + kWallOffset; // 800 + 200 = 1000
	m_collisionWallPos.z = kStartTilePos.z + kWallOffset;
	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.x = kStartTilePos.x + (kTileSize * i); // -800, -400, 0, 400, 800
		m_collisionWallPos.x = kStartTilePos.x + (kTileSize * i);

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		MV1SetPosition(m_collisionWallModelHandles[wallIndex], m_wallPos);
		MV1SetupCollInfo(m_collisionWallModelHandles[wallIndex], -1);
		wallIndex++;
	}

	// Z最小 (-800) からさらに手前へ
	m_wallPos.z = (kStartTilePos.z - kTileSize * (kWallNumPerSide - 1)) - kWallOffset; // (800 - 400*4) - 200 = -1000
	m_collisionWallPos.z = (kStartTilePos.z - kTileSize * (kWallNumPerSide - 1)) - kWallOffset;
	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.x = kStartTilePos.x + (kTileSize * i); // -800, -400, 0, 400, 800
		m_collisionWallPos.x = kStartTilePos.x + (kTileSize * i); 

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		MV1SetPosition(m_collisionWallModelHandles[wallIndex], m_wallPos);
		MV1SetupCollInfo(m_collisionWallModelHandles[wallIndex], -1);
		wallIndex++;
	}

	m_wallPos.x = kStartTilePos.x - kWallOffset; // -800 - 200 = -1000
	m_collisionWallPos.x = kStartTilePos.x - kWallOffset;
	// Y軸に90度回転 (モデルがZ軸方向に長い場合)

	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.z = kStartTilePos.z - (kTileSize * i); // 800, 400, 0, -400, -800

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		MV1SetPosition(m_collisionWallModelHandles[wallIndex], m_wallPos);
		MV1SetRotationXYZ(m_wallModelHandles[wallIndex], kWallRotY90); // 回転を適用
		MV1SetRotationXYZ(m_collisionWallModelHandles[wallIndex], kWallRotY90); 
		MV1SetupCollInfo(m_collisionWallModelHandles[wallIndex], -1);
		wallIndex++;
	}

	// X最大 (800) からさらに右へ
	m_wallPos.x = (kStartTilePos.x + kTileSize * (kWallNumPerSide - 1)) + kWallOffset; // (-800 + 400*4) + 200 = 1000
	m_collisionWallPos.x = (kStartTilePos.x + kTileSize * (kWallNumPerSide - 1)) + kWallOffset; 
	// 回転は左の壁と同じ (kWallRotY90)

	for (int i = 0; i < kWallNumPerSide; i++)
	{
		m_wallPos.z = kStartTilePos.z - (kTileSize * i); // 800, 400, 0, -400, -800

		MV1SetPosition(m_wallModelHandles[wallIndex], m_wallPos);
		MV1SetPosition(m_collisionWallModelHandles[wallIndex], m_wallPos);
		MV1SetRotationXYZ(m_wallModelHandles[wallIndex], kWallRotY90); // 回転を適用
		MV1SetRotationXYZ(m_collisionWallModelHandles[wallIndex], kWallRotY90);
		MV1SetupCollInfo(m_collisionWallModelHandles[wallIndex], -1);
		wallIndex++;
	}
	MV1TerminateCollInfo(m_collisionTileModelBase); // 複製元モデルの当たり判定情報は不要なので破棄
	MV1TerminateCollInfo(m_wallModelBase);
	MV1TerminateCollInfo(m_collisionWallBase);
}

void Stage::End()
{
	for (int collisionTileModelHandle : m_collisionTileModelHandles)
	{
		if (collisionTileModelHandle != -1) 
		{
			MV1TerminateCollInfo(collisionTileModelHandle);
			MV1DeleteModel(collisionTileModelHandle);
		}
	}
	m_collisionTileModelHandles.clear();
	MV1DeleteModel(m_collisionTileModelBase);

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

	for (int collisionwallModelHandle : m_collisionWallModelHandles)
	{
		if (collisionwallModelHandle != -1)
		{
			MV1TerminateCollInfo(collisionwallModelHandle);
			MV1DeleteModel(collisionwallModelHandle);
		}
	}
	m_collisionWallModelHandles.clear();
	MV1DeleteModel(m_collisionWallBase);
	MV1DeleteModel(m_stairsHandle);
}

void Stage::Update()
{
	
}

void Stage::Draw()
{
	for (int i = 0; i < m_tileTotal;i++)
	{
		MV1DrawModel(m_collisionTileModelHandles[i]);
	}
	for (int i = 0; i < m_tileTotal;i++)
	{
		MV1DrawModel(m_tileModelHandles[i]);
	}
	for (int wallModelHandle : m_wallModelHandles)
	{
		MV1DrawModel(wallModelHandle);
	}
	for (int collisionwallModelHandle : m_collisionWallModelHandles)
	{
		MV1DrawModel(collisionwallModelHandle);
	}
}
