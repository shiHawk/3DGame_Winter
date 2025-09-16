#include "SceneManager.h"
#include "DxLib.h"
#include "GameScene.h"

SceneManager::SceneManager()
{
	m_pScene = nullptr;
}

void SceneManager::Init()
{
	m_pScene = new GameScene;
	m_pScene->Init();
}

void SceneManager::End()
{
	if (!m_pScene)	return;

	m_pScene->End();
	delete m_pScene;
}

void SceneManager::Updata()
{
	if (!m_pScene)	return;
	SceneBase* pScene = m_pScene->Update();
	if (pScene != m_pScene)
	{
		// 前のシーンの終了処理
		m_pScene->End();
		delete m_pScene;

		m_pScene = pScene;
		m_pScene->Init();
	}
	m_pScene->Update();
}

void SceneManager::Draw()
{
	if (!m_pScene)	return;

	m_pScene->Draw();
}
