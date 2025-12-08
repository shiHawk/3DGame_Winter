#pragma once
#include "DxLib.h"
#include <vector>
#include <string>
#include<map>
class Stage
{
public:
	Stage();
	~Stage() {};
	bool LoadData(const char* fileName);
	void Init();
	void End();
	void Update();
	void Draw();
	const std::vector<int>& GetTileModelHandles() const { return m_collisionTileModelHandles; }
	const std::vector<int>& GetWallModelHandles() const { return m_collisionWallModelHandles; }
private:
	int m_tileTotal; // タイルの合計
	int m_stageStart; // 配置するタイルの始点
	int m_stageEnd; // 配置するタイルの終点
	float m_tileSize; // タイルのサイズ
	VECTOR m_tilePos;
	VECTOR m_tileStartPos;
	VECTOR m_wallPos;
	VECTOR m_collisionWallPos;
	int m_collisionTileModelBase; // 当たり判定用タイルのコピー元
	int m_tileModelBase;
	int m_wallModelBase; // 壁のコピー元
	int m_collisionWallBase; // 当たり判定用の壁
	std::vector<int> m_collisionTileModelHandles;
	std::vector<int> m_tileModelHandles;
	std::vector<int> m_wallModelHandles;
	std::vector<int> m_collisionWallModelHandles;
	int m_stairsHandle;
	struct StageObjectData
	{
		int typeId;
		std::string objectName;
		int hasCollision;
		VECTOR position;
		VECTOR rotation;
		VECTOR scale;
	};
	std::vector<StageObjectData> m_stageData;
	std::map<int, int> m_baseModelHandles; // typeIdとベースモデルハンドル
	std::vector<int> m_objectModelHandles; // 生成した全オブジェクトのモデルハンドル
};

