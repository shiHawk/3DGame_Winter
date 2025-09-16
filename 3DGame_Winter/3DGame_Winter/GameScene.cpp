#include "GameScene.h"
#include "DxLib.h"
namespace
{
	constexpr VECTOR kTrianglePos1 = { 200.0f,200.0f,0.0f };
	constexpr VECTOR kTrianglePos2 = { 400.0f,0.0f,0.0f };
	constexpr VECTOR kTrianglePos3 = { 0.0f,0.0f,0.0f };
	constexpr int kTriangleColor = 0xfff0ff;
	constexpr VECTOR kSpherePos = { 0.0f,0.0f,0.0f };
	constexpr VECTOR kSpherePos2 = { 600.0f,0.0f,500.0f };
	constexpr float kSphereRadius = 20.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0xff0fff;
	constexpr int kSphereSpcColor = 0xffffff;
}
GameScene::GameScene():
	m_temp(0)
{
}

void GameScene::Init()
{
	m_pCamera = std::make_shared<Camera>();
	m_pCamera->Init();
}

void GameScene::End()
{
	m_pCamera->End();
}

SceneBase* GameScene::Update()
{
	m_pCamera->Update();
	return this;
}

void GameScene::Draw()
{
	//DrawTriangle3D(kTrianglePos1, kTrianglePos2, kTrianglePos3, kTriangleColor,false);
	DrawSphere3D(kSpherePos, kSphereRadius, kDivNum, kSphereDifColor, kSphereSpcColor,true);
	DrawSphere3D(kSpherePos2, kSphereRadius, kDivNum, kSphereDifColor, kSphereSpcColor,true);
}
