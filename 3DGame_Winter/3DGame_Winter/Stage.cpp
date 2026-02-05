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
	constexpr float kPositionScale = 100.0f;
	constexpr float kRadConvert = DX_PI_F / kDegreesPerCircle; // 度数法をラジアンに変換するための定数 (180.0f)
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

		// 12. ScaleZ (float)
		std::getline(ss, cell, ',');
		data.scale.z = std::stof(cell);

		// 13. RotationQW (float)
		std::getline(ss, cell, ',');
		data.quaternion.w = std::stof(cell);

		// 14. RotationQX (float)
		std::getline(ss, cell, ',');
		data.quaternion.x = std::stof(cell);

		// 15. RotationQY (float)
		std::getline(ss, cell, ',');
		data.quaternion.y = std::stof(cell);

		// 16. RotationQZ (float) - 最後の要素なので区切り文字は不要
		std::getline(ss, cell, ',');
		data.quaternion.z = std::stof(cell); 

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
	m_baseModelHandles[7] = MV1LoadModel(L"Data/model/wall_arched.mv1");
	m_baseModelHandles[8] = MV1LoadModel(L"Data/model/wall.mv1");
	m_baseModelHandles[9] = MV1LoadModel(L"Data/model/floor_foundation_allsides.mv1");
	m_baseModelHandles[10] = MV1LoadModel(L"Data/model/barrier_colum_half.mv1");
	m_baseModelHandles[11] = MV1LoadModel(L"Data/model/barrier_corner.mv1");
	m_baseModelHandles[12] = MV1LoadModel(L"Data/model/barrier.mv1");
	m_baseModelHandles[13] = MV1LoadModel(L"Data/model/Primitive_Slope.mv1");
	m_baseModelHandles[14] = MV1LoadModel(L"Data/model/Primitive_Floor.mv1");
	m_baseModelHandles[15] = MV1LoadModel(L"Data/model/Primitive_Cube.mv1");
	m_baseModelHandles[16] = MV1LoadModel(L"Data/model/floor_tile_grate_open.mv1");
	m_baseModelHandles[17] = MV1LoadModel(L"Data/model/floor_tile_large.mv1");
	m_baseModelHandles[18] = MV1LoadModel(L"Data/model/pillar_decorated.mv1");

	if (!LoadData("Data/stage/stage_data.csv"))
	{
		return;
	}

	std::map<int, float> modelScaleFix;
	modelScaleFix[0] = 100.0f; // stairs_wall_left
	modelScaleFix[1] = 1.0f; // stairs_walled
	modelScaleFix[2] = 100.0f; // stairs_wide
	modelScaleFix[3] = 100.0f; // wall_corner_small
	modelScaleFix[4] = 100.0f; // wall_open_scaffold
	modelScaleFix[5] = 100.0f; // wall_cracked
	modelScaleFix[6] = 100.0f; // wall_corner
	modelScaleFix[7] = 100.0f; // wall_arched
	modelScaleFix[8] = 1.0f; // wall
	modelScaleFix[9] = 100.0f; // floor_foundation_allsides
	modelScaleFix[10] = 100.0f; // barrier_colum_half
	modelScaleFix[11] = 100.0f; // barrier_corner
	modelScaleFix[12] = 100.0f; // barrier
	modelScaleFix[13] = 100.0f; // Primitive_Slope
	modelScaleFix[14] = 100.0f; // Primitive_Floor
	modelScaleFix[15] = 100.0f; // Primitive_Cube
	modelScaleFix[16] = 1.0f; // floor_tile_grate_open
	modelScaleFix[17] = 1.0f; // floor_tile_large
	modelScaleFix[18] = 100.0f; // pillar_decorated

	for (auto& data : m_stageData)
	{
		auto temp = m_baseModelHandles.find(data.typeId);
		if (temp == m_baseModelHandles.end())
		{
			continue;
		}

		int baseHandle = temp->second;

		// ベースモデルから新しいモデルを複製
		int handle = MV1DuplicateModel(baseHandle);
		// 当たり判定用のモデルは描画しない
		if (data.typeId == 13 || data.typeId == 14 || data.typeId == 15)
		{
			MV1SetVisible(handle,false);
		}
		VECTOR scaledPos;
		scaledPos.x = data.position.x * kPositionScale;
		scaledPos.y = data.position.y * kPositionScale;
		scaledPos.z = data.position.z * kPositionScale;
		// 座標を設定
		//MV1SetPosition(handle, scaledPos);

		VECTOR finalScale = data.scale;
		// もし補正マップに登録されているIDなら、その倍率を掛ける
		if (modelScaleFix.count(data.typeId) > 0)
		{
			float fixRate = modelScaleFix[data.typeId];
			finalScale.x *= fixRate;
			finalScale.y *= fixRate;
			finalScale.z *= fixRate;
		}

		// スケールを設定
		//MV1SetScale(handle, finalScale);

		// 1. クォータニオンから回転行列を取得
		MATRIX rotationMatrix = data.quaternion.GetMatrix();
		MATRIX modelFixMatrix = MGetRotY(DX_PI_F); // 180度回転(これをしないとモデルの回転が配置通りにならない)
		rotationMatrix = MMult(modelFixMatrix, rotationMatrix);

		// 2-1. 拡大行列を生成
		VECTOR scaleVec = VGet(finalScale.x, finalScale.y, finalScale.z);
		MATRIX scaleMatrix = MGetScale(scaleVec);

		// 2-2. 拡大と回転を合成
		MATRIX finalMatrix = MMult(scaleMatrix, rotationMatrix);

		// 2-3. 平行移動成分を最終行列に設定
		finalMatrix.m[3][0] = scaledPos.x;
		finalMatrix.m[3][1] = scaledPos.y;
		finalMatrix.m[3][2] = scaledPos.z;
		MV1SetMatrix(handle, finalMatrix);

		if (data.typeId == 13 || data.typeId == 14 || data.typeId == 15)
		{
			MV1SetupCollInfo(handle, -1);
			if (data.typeId == 13 || data.typeId == 14)
			{
				m_groundCollisionModelHandles.push_back(handle);
			}
			else
			{
				m_wallCollisionModelHandles.push_back(handle);
			}
			m_CollisionObjectModelHandles.push_back(handle); // 当たり判定用のモデルハンドルを格納
		}
		m_objectModelHandles.push_back(handle); 
	}
}

void Stage::End()
{
	for (int handle : m_objectModelHandles)
	{
		if (handle != -1)
		{
			MV1TerminateCollInfo(handle);
			MV1DeleteModel(handle);
		}
	}
	m_objectModelHandles.clear();

	for (int handle : m_CollisionObjectModelHandles)
	{
		if (handle != -1)
		{
			MV1TerminateCollInfo(handle);
			MV1DeleteModel(handle);
		}
	}
	m_CollisionObjectModelHandles.clear();

	for (int handle : m_groundCollisionModelHandles)
	{
		if (handle != -1)
		{
			MV1TerminateCollInfo(handle);
			MV1DeleteModel(handle);
		}
	}
	m_groundCollisionModelHandles.clear();

	for (int handle : m_wallCollisionModelHandles)
	{
		if (handle != -1)
		{
			MV1TerminateCollInfo(handle);
			MV1DeleteModel(handle);
		}
	}
	m_wallCollisionModelHandles.clear();


	// ベースモデルを削除
	for (auto const& [typeId, handle] : m_baseModelHandles)
	{
		MV1DeleteModel(handle);
	}
	m_baseModelHandles.clear();

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
	for (int handle : m_objectModelHandles)
	{
		MV1DrawModel(handle);
	}

	/*for (int i = 0; i < m_tileTotal;i++)
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
	}*/
}
