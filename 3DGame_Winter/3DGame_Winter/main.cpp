#include "DxLib.h"
#include "game.h"
#include "SceneManager.h"

//Dxlibのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// windowモード設定
	ChangeWindowMode(Game::kWindowMode);
	// ウインドウ名設定
	//SetMainWindowText("WarriorAdventure");
	// 画面サイズの設定
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorDepth);

	SetUseDirect3DVersion(DX_DIRECT3D_11);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}
	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);
	// 最初のシーンの初期化
	SceneManager scene;
	scene.Init();
	// ゲームループ部
	while (ProcessMessage() == 0)
	{
		LONGLONG  time = GetNowHiPerformanceCount();
		// 画面のクリア
		ClearDrawScreen();
		scene.Update();
		scene.Draw();
		// 裏画面を表画面を入れ替える
		ScreenFlip();

		// escキーを押したら終了する
		if (CheckHitKey(KEY_INPUT_ESCAPE))	break;

		// fpsを60に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}
	}
	scene.End();
	DxLib_End();//Dxlib終了処理
	return 0;//終了 
}