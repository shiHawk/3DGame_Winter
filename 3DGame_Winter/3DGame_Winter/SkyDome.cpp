#include "SkyDome.h"
namespace
{
	//回転速度
	constexpr float kRotSpeed = 0.0002f;

	//スカイドームのスケール
	constexpr VECTOR kSkyDomeScale = { 40.0f,40.0f,40.0f };
	constexpr VECTOR kSkyDomePos = { 0.0f,200.0f,-840.0f };
}

SkyDome::SkyDome():
	m_pos(kSkyDomePos),
	m_rot(0.0f)
{
}

SkyDome::~SkyDome()
{
}

void SkyDome::Init()
{
	m_handle = MV1LoadModel(L"Data/skyDome/Dome.mv1");
	MV1SetScale(m_handle,kSkyDomeScale);
	// スカイドームがZバッファを更新しないようにする
	MV1SetWriteZBuffer(m_handle, FALSE);
}

void SkyDome::End()
{
	MV1DeleteModel(m_handle);
}

void SkyDome::Update()
{
	RotationSkyDome();
	MV1SetPosition(m_handle, m_pos);
	MV1SetRotationXYZ(m_handle,VGet(0.0f,m_rot,0.0f));
}

void SkyDome::Draw()
{
	MV1DrawModel(m_handle);
}

void SkyDome::RotationSkyDome()
{
	//回転
	m_rot += kRotSpeed;
}
