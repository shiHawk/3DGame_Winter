#pragma once
#include "DxLib.h"
enum class SceneID
{
	TitleScene,
	GameScene,
	ResultScene
};
class SceneBase
{
public:
	SceneBase();
	virtual ~SceneBase() {}
	virtual void Init() {}
	virtual void End() {}

	virtual SceneBase* Update() { return this; }
	virtual void Draw() {}
	// フェード関連
	void UpdateFade();
	void DrawFade() const;

	bool IsFadingIn() const;	// フェードイン中
	bool IsFadingOut() const;	// フェードアウト中
	bool IsFading() const { return IsFadingIn() || IsFadingOut(); }	// フェードインorアウト中

	void StartFadeOut();	// フェードアウト開始
	bool IsFadeComplete()const;
	
	virtual SceneID GetSceneID() const = 0;
protected:
	// フェード関係
	int m_fadeColor;
	int m_fadeBright;
	int m_fadeSpeed;
	// シャドウマップ
	int m_shadowMapHandle;
};

