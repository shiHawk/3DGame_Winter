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
private:
	int m_tileTotal; // タイルの合計
	int m_stageStart; // 配置するタイルの始点
	int m_stageEnd; // 配置するタイルの終点
	float m_tileSize; // タイルのサイズ
	VECTOR m_tilePos;
	VECTOR m_tileStartPos;
	int m_tileModelBase; // タイルのコピー元
	std::vector<int> m_tileModelHandles;
};

