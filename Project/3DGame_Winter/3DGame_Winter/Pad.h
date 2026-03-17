#pragma once
// パッドの他の割り当ててある値とかぶらない値
#define PAD_INPUT_RT (1 << 30)
#define PAD_INPUT_LT (1 << 29)
// コントローラーの入力状態を取得する
namespace Pad
{
	// パッドの入力状態取得
	void Update();

	// 押し下げ判定
	bool isPress(int button);
	// トリガー判定
	bool isTrigger(int button);
	// 離した判定
	bool isRelase(int button);
}
