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
	constexpr float kGridSize = 1000.0f;   // �O���b�h�S�̂̔��a
	constexpr float kGridInterval = 100.0f; // ���Ɛ��̊Ԋu
	constexpr int kGridColor = 0xAAAAAA;    // �O���b�h�̐F
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
	DrawGrid();
}

void GameScene::DrawGrid()
{
	// Y=0�̕��ʂɃO���b�h��`��
	for (float i = -kGridSize; i <= kGridSize; i += kGridInterval)
	{
		// Z�����ɕ��s�Ȑ��iX�Œ�j
		DrawLine3D(
			VGet(i, 0.0f, -kGridSize),  // �n�_
			VGet(i, 0.0f, kGridSize),   // �I�_
			kGridColor
		);

		// X�����ɕ��s�Ȑ��iZ�Œ�j
		DrawLine3D(
			VGet(-kGridSize, 0.0f, i),  // �n�_
			VGet(kGridSize, 0.0f, i),   // �I�_
			kGridColor
		);
	}
}
