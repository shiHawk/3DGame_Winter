#include "Pad.h"
#include "DxLib.h"

namespace
{
	constexpr int kLogNum = 16;
	// 入力ログ	0が最新の状態
	int padLog[kLogNum];
}


void Pad::Update()
{
	// 現在のパッドの状態を取得
	int padState = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	DINPUT_JOYSTATE dinput;
	if (GetJoypadDirectInputState(DX_INPUT_PAD1, &dinput) == 0)
	{
		if (dinput.Z < -50)
		{
			padState |= PAD_INPUT_RT; // RTボタンの状態を追加する
		}
		if (dinput.Z > 50)
		{
			padState |= PAD_INPUT_LT; // LTボタンの状態を追加する
		}
	}
	// ログの更新
	for (int i = kLogNum - 1; i >= 1; i--)
	{
		padLog[i] = padLog[i - 1];
	}
	// 最新の状態
	padLog[0] = padState;
}

// 押し下げ判定
bool Pad::isPress(int button)
{
	return (padLog[0] & button);
}

// トリガー判定
bool Pad::isTrigger(int button)
{
	bool isNow = (padLog[0] & button);	// 現在の状態
	bool isLast = (padLog[1] & button);	// １フレーム前の状態
	return (isNow && !isLast);
}

// 離した判定
bool Pad::isRelase(int button)
{
	bool isNow = (padLog[0] & button);	// 現在の状態
	bool isLast = (padLog[1] & button);	// １フレーム前の状態
	return (!isNow && isLast);
}
