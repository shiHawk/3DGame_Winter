#include "Camera.h"
#include <cmath>
#include "Pad.h"
namespace
{
	constexpr float kLerpSpeed = 0.055f;
	constexpr float kOffSetPos = 200.0f;
	constexpr VECTOR kSecondLight = { -0.577f, -0.577f, 0.577f };
	constexpr float kRightLimitCamera = 4807.0f;
	constexpr float kLeftLimitCamera = -2355.0f;
	// カメラの位置と注視点
	constexpr VECTOR kDefaultCameraPos = { 0.0f,300.0f,-540.0f };
	constexpr VECTOR kCameraTarget = { 0.0f,200.0f,0.0f };
	// カメラの視野角
	constexpr float kViewAngle = 1.396f; // 視野角(度数) DX_PI_F / 180.0f * 80
	// nearとfarの位置
	constexpr float kCameraNearClip = 10.0f;
	constexpr float kCameraFarClip = 3000.0f;
	// ライトのカラー
	constexpr float kRed = 1.0f;
	constexpr float kGreen = 0.647f;
	constexpr float kBlue = 0.0f;
	// カメラの旋回
	constexpr float kCameraAngleSpeed = 0.01f;
	constexpr float kCameraSphereLength = 540.0f;
	constexpr float kAngleLimitVertical = 0.6f;
}
Camera::Camera():
	m_cameraPos({0.0f,0.0f,0.0f}),
	m_cameraMoveTargetPos({ 0.0f,0.0f,0.0f }),
	m_cameraTarget({ 0.0f,0.0f,0.0f }),
	m_lightHandle(-1),
	m_cameraAngleHorizontal(0.0f),
	m_cameraAngleVertical(0.0f),
	m_input({ 0,0,0,0,0,0,{0,0},{0xFF, 0xFF, 0xFF, 0xFF},0 }),
	m_viewRadianAngle(0.0f),
	m_targetAngleHorizontal(0.0f),
	m_targetAngleVertical(0.0f)
{
}

void Camera::Init()
{
	// 3D表示の設定
	SetUseZBuffer3D(true);	  // Zバッファを指定する
	SetWriteZBuffer3D(true);  // Zバッファへの書き込みを行う

	SetUseBackCulling(true);  // ポリゴンの裏面を表示しない

	// ライトの色を変更する
	SetLightSpcColor(GetColorF(kRed, kGreen, kBlue, 0.0f));

	// カメラの位置の初期化を行う

	// カメラ(始点)の位置
	m_cameraPos = kDefaultCameraPos;

	// カメラがどこを見ているか(注視点)
	m_cameraTarget = kCameraTarget;
	// カメラの位置と注視点を指定する
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, m_cameraTarget);
	RadianTranslation();
	// カメラの視野角を設定する
	SetupCamera_Perspective(kViewAngle);
	// カメラのnear,farを設定する
	// 画面に表示される距離の範囲を設定する
	// カメラからnear以上離れていてfarより近くにあるものが
	// ゲーム画面に表示される
	// farはあまり大きすぎる数字を設定しないように気を付ける(表示バグに繋がる)
	SetCameraNearFar(kCameraNearClip, kCameraFarClip);
}

void Camera::End()
{
	m_cameraPos = kDefaultCameraPos;
	m_cameraTarget = kCameraTarget;
}

void Camera::Update()
{
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_input);

	// カメラの角度の計算
	if (m_input.Rx > 0)
	{
		m_targetAngleHorizontal += kCameraAngleSpeed;
	}
	else if(m_input.Rx < 0)
	{
		m_targetAngleHorizontal -= kCameraAngleSpeed;
	}
	else if (m_input.Ry < 0)
	{
		m_targetAngleVertical += kCameraAngleSpeed;
		if (m_targetAngleVertical > DX_PI_F * 0.5f - kAngleLimitVertical) // ある一定の角度以上にはならないようにする
		{
			m_targetAngleVertical = DX_PI_F * 0.5f - kAngleLimitVertical;
		}
	}
	else if (m_input.Ry > 0)
	{
		m_targetAngleVertical -= kCameraAngleSpeed;
		if (m_targetAngleVertical < -DX_PI_F * 0.5f + kAngleLimitVertical) // ある一定の角度以下にはならないようにする
		{
			m_targetAngleVertical = -DX_PI_F * 0.5f + kAngleLimitVertical;
		}
	}
	m_cameraAngleHorizontal = std::lerp(m_cameraAngleHorizontal,m_targetAngleHorizontal, kLerpSpeed);
	m_cameraAngleVertical = std::lerp(m_cameraAngleVertical,m_targetAngleVertical, kLerpSpeed);
	MATRIX rotX,rotY; // カメラの回転行列
	float cameraToPlayerLength;
	rotY = MGetRotY(m_cameraAngleHorizontal); // 水平方向の回転はY軸回転
	rotX = MGetRotX(m_cameraAngleVertical); // 垂直方向の回転はX軸回転
	cameraToPlayerLength = kCameraSphereLength; // カメラからプレイヤーまでの初期の距離をセット
	// カメラの座標を算出
	// Z軸にカメラとプレイヤーとの距離分だけ伸びたベクトルを
	// 垂直方向回転( X軸回転 )させたあと水平方向回転( Ｙ軸回転 )して更に
	// 注視点の座標を足したものがカメラの座標
	m_cameraPos = VAdd(VTransform(VTransform(VGet(0.0f, 0.0f, -cameraToPlayerLength), rotX), rotY), m_cameraTarget);
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, m_cameraTarget); // カメラを計算した位置に設定する
}

void Camera::RadianTranslation()
{
	m_viewRadianAngle = (DX_PI_F / 180.0f) * kViewAngle;
}
