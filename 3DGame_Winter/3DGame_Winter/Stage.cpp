#include "Stage.h"
#include <fstream>
#include <sstream>
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
	constexpr float kDegreesPerCircle = 180.0f; // 一周当たりの度数
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

bool Stage::LoadData(const char* fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false; // ファイルが開けなかった
	}
	std::string line;
	// 1行目のヘッダー行をスキップ
	std::getline(file, line);

	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string cell;
		StageObjectData data;

		// 1. TypeID (int)
		std::getline(ss, cell, ',');
		data.typeId = std::stoi(cell);

		// 2. ObjectName (string)
		std::getline(ss, cell, ',');
		data.objectName = cell;

		// 3. HasCollision (int)
		std::getline(ss, cell, ',');
		data.hasCollision = std::stoi(cell);

		// 4. PositionX (float)
		std::getline(ss, cell, ',');
		data.position.x = std::stof(cell);

		// 5. PositionY (float)
		std::getline(ss, cell, ',');
		data.position.y = std::stof(cell);

		// 6. PositionZ (float)
		std::getline(ss, cell, ',');
		data.position.z = std::stof(cell);

		// 7. RotationX (float)
		std::getline(ss, cell, ',');
		data.rotation.x = std::stof(cell);

		// 8. RotationY (float)
		std::getline(ss, cell, ',');
		data.rotation.y = std::stof(cell);

		// 9. RotationZ (float)
		std::getline(ss, cell, ',');
		data.rotation.z = std::stof(cell);

		// 10. ScaleX (float)
		std::getline(ss, cell, ',');
		data.scale.x = std::stof(cell);

		// 11. ScaleY (float)
		std::getline(ss, cell, ',');
		data.scale.y = std::stof(cell);

		// 12. ScaleZ (float) - 最後の要素なので区切り文字は不要
		std::getline(ss, cell, ',');
		data.scale.z = std::stof(cell);

		// データをコンテナに格納
		m_stageData.push_back(data);
	}
	return true;
}

void Stage::Init()
{
	m_baseModelHandles[0] = MV1LoadModel(L"Data/model/stairs_wall_left.mv1");
	m_baseModelHandles[1] = MV1LoadModel(L"Data/model/stairs_walled.mv1");
	m_baseModelHandles[2] = MV1LoadModel(L"Data/model/stairs_wide.mv1");
	m_baseModelHandles[3] = MV1LoadModel(L"Data/model/wall_corner_small.mv1");
	m_baseModelHandles[4] = MV1LoadModel(L"Data/model/wall_open_scaffold.mv1");
	m_baseModelHandles[5] = MV1LoadModel(L"Data/model/wall_cracked.mv1");
	m_baseModelHandles[6] = MV1LoadModel(L"Data/model/wall_corner.mv1");
	m_baseModelHandles[7] = MV1LoadModel(L"Data/model/wall.mv1");
	m_baseModelHandles[8] = MV1LoadModel(L"Data/model/floor_foundation_allsides.mv1");
	m_baseModelHandles[9] = MV1LoadModel(L"Data/model/barrier_colum_half.mv1");
	m_baseModelHandles[10] = MV1LoadModel(L"Data/model/barrier_corner.mv1");
	m_baseModelHandles[11] = MV1LoadModel(L"Data/model/barrier.mv1");
	m_baseModelHandles[12] = MV1LoadModel(L"Data/model/ramp.mv1");
	m_baseModelHandles[13] = MV1LoadModel(L"Data/model/cube6.mv1");
	m_baseModelHandles[14] = MV1LoadModel(L"Data/model/cube7.mv1");
	m_baseModelHandles[15] = MV1LoadModel(L"Data/model/floor_tile_grate_open.mv1");
	m_baseModelHandles[16] = MV1LoadModel(L"Data/model/floor_tile_large.mv1");


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
