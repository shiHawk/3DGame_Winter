#include "Camera.h"
#include <cmath>
#include "Pad.h"
namespace
{
	constexpr float kLerpSpeed = 0.015f;
	constexpr float kOffSetPos = 200.0f;
	constexpr VECTOR kSecondLight = { -0.577f, -0.577f, 0.577f };
	constexpr float kRightLimitCamera = 4807.0f;
	constexpr float kLeftLimitCamera = -2355.0f;
	// カメラの位置と注視点
	constexpr VECTOR kDefaultCameraPos = { 0.0f,300.0f,-840.0f };
	constexpr VECTOR kCameraTarget = { 0.0f,50.0f,0.0f };
	// カメラの視野角
	constexpr float kViewAngle = 0.447f;
	// nearとfarの位置
	constexpr float kCameraNearClip = 10.0f;
	constexpr float kCameraFarClip = 3000.0f;
	// ライトのカラー
	constexpr float kRed = 1.0f;
	constexpr float kGreen = 0.647f;
	constexpr float kBlue = 0.0f;
	// カメラの旋回
	constexpr float kCameraAngleSpeed = 0.02f;
	constexpr float kCameraSphereLength = 840.0f;

}
Camera::Camera():
	m_cameraPos({0.0f,0.0f,0.0f}),
	m_cameraMoveTargetPos({ 0.0f,0.0f,0.0f }),
	m_cameraTarget({ 0.0f,0.0f,0.0f }),
	m_lightHandle(-1),
	m_cameraAngle(0.0f)
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
	DINPUT_JOYSTATE input;
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
	// カメラの角度の計算
	if (input.Rx > 0)
	{
		m_cameraAngle += kCameraAngleSpeed;
		if (m_cameraAngle > DX_PI_F) // 角度値を大きくしすぎないように-180°を超えたら360°引く
		{
			m_cameraAngle -= DX_TWO_PI_F;
		}
	}
	else if(input.Rx < 0)
	{
		m_cameraAngle -= kCameraAngleSpeed;
		if (m_cameraAngle < -DX_PI_F)
		{
			m_cameraAngle += DX_TWO_PI_F;
		}
	}

	MATRIX rotY; // カメラの回転行列
	float cameraToPlayerLength;
	rotY = MGetRotY(m_cameraAngle); // 水平方向の回転はY軸回転
	cameraToPlayerLength = kCameraSphereLength; // カメラからプレイヤーまでの初期の距離をセット
	// カメラの座標を算出
	// Z軸にカメラとプレイヤーとの距離分だけ伸びたベクトルを
	// 垂直方向回転( X軸回転 )させたあと水平方向回転( Ｙ軸回転 )して更に
	// 注視点の座標を足したものがカメラの座標
	m_cameraPos = VAdd(VTransform(VGet(0.0f,0.0f, -cameraToPlayerLength),rotY), m_cameraTarget);
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, m_cameraTarget); // カメラを計算した位置に設定する
}
