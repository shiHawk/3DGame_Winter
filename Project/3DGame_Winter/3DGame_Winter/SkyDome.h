#pragma once
#include <memory>
#include "DxLib.h"
class Player;
class SkyDome
{
public:
	SkyDome();
	~SkyDome();
	void Init();
	void End();
	void Update();
	void Draw();
	void SetPos(VECTOR pos) { m_pos = pos; }
	void SetScale(float scale) { MV1SetScale(m_handle, VGet(scale, scale, scale)); }
	void RotationSkyDome();
private:
	//スカイドームの回転率
	float m_rot;
	//スカイドームのモデル
	int m_handle = -1;

	VECTOR m_pos = VGet(0.0f, 0.0f, 0.0f);
};

