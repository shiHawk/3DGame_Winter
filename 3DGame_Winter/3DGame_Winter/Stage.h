#pragma once
#include "DxLib.h"
#include <vector>
class Stage
{
public:
	Stage();
	~Stage() {};
	void Init();
	void End();
	void Update();
	void Draw();
	const std::vector<int>& GetTileModelHandles() const { return m_tileModelHandles; }
	const std::vector<int>& GetWallModelHandles() const { return m_wallModelHandles; }
private:
	int m_tileTotal; // タイルの合計
	int m_stageStart; // 配置するタイルの始点
	int m_stageEnd; // 配置するタイルの終点
	float m_tileSize; // タイルのサイズ
	VECTOR m_tilePos;
	VECTOR m_tileStartPos;
	VECTOR m_wallPos;
	int m_tileModelBase; // タイルのコピー元
	int m_wallModelBase; // 壁のコピー元
	std::vector<int> m_tileModelHandles;
	std::vector<int> m_wallModelHandles;
	int m_stairsHandle;
};

