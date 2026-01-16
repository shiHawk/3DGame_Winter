#include "Camera.h"
#include <cmath>
#include "Pad.h"
namespace
{
	constexpr float kLerpSpeed = 0.200f;
	constexpr float kOffSetPos = 200.0f;
	constexpr VECTOR kSecondLight = { -0.577f, -0.577f, -0.577f };
	constexpr float kRightLimitCamera = 4807.0f;
	constexpr float kLeftLimitCamera = -2355.0f;
	// カメラの位置と注視点
	constexpr VECTOR kDefaultCameraPos = { 0.0f,300.0f,-540.0f };
	constexpr VECTOR kCameraTarget = { 0.0f,250.0f,0.0f };
	constexpr float kMinTargetToPlayerDistance = 500.0f;
	// カメラの視野角
	constexpr float kDegreesPerCircle = 180.0f; // 一周当たりの度数
	constexpr float kViewAngle = 80.0f; // 視野角(度数) 
	// nearとfarの位置
	constexpr float kCameraNearClip = 10.0f;
	constexpr float kCameraFarClip = 12500.0f;
	// ライトのカラー
	constexpr float kRed = 1.0f;
	constexpr float kGreen = 1.0f;
	constexpr float kBlue = 1.0f;
	// カメラの旋回
	constexpr float kCameraAngleSpeed = 0.03f;
	//constexpr float kCameraToPlayerLength = 540.0f;
	constexpr float kCameraToPlayerLength = 450.0f;
	constexpr float kAngleLimitVertical = 0.6f;
	constexpr float kCameraPitchDownLimit = -0.3f;
	constexpr float kCameraPitchUpLimit = 0.97f;
	// カメラの移動範囲
	constexpr float kStageMinX = -1000.0f;
	constexpr float kStageMaxX = 1000.0f;
	constexpr float kStageMinZ = -1000.0f;
	constexpr float kStageMaxZ = 1000.0f;
	constexpr float kCameraRadius = 2000.0f; // カメラの当たり判定用半径

	constexpr float kMaxLockonRange = 800.0f;
	constexpr float kLockonRotateSpeed = 0.01f;
}
Camera::Camera():
	m_cameraPos({0.0f,0.0f,0.0f}),
	m_cameraMoveTargetPos({ 0.0f,0.0f,0.0f }),
	m_cameraTarget({ 0.0f,0.0f,0.0f }),
	m_lightHandle(-1),
	m_cameraAngleHorizontal(0.0f),
	m_cameraAngleVertical(0.0f),
	m_input({ 0,0,0,0,0,0,{0,0},{0xff, 0xff, 0xff, 0xff},0 }),
	m_viewRadianAngle(0.0f),
	m_targetAngleHorizontal(0.0f),
	m_targetAngleVertical(0.0f),
	m_playerPos({0.0f,0.0f,0.0f}),
	m_isLockOn(false),
	m_targetToPlayer({ 0.0f,0.0f,0.0f }),
	m_targetToPlayerDistance(0.0f),
	m_lockOnCameraPos({ 0.0f,0.0f,0.0f }),
	m_playerDir({ 0.0f,0.0f,0.0f }),
	m_lockonHandle(-1),
	m_lockonRotateAngle(0.0f)
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
	SetLightDifColor(GetColorF(kRed, kGreen, kBlue, 0.0f));
	m_lightHandle = CreateDirLightHandle(kSecondLight);

	// カメラの位置の初期化を行う
	// カメラ(始点)の位置
	m_cameraPos = kDefaultCameraPos;

	// カメラがどこを見ているか(注視点)
	m_cameraTarget = kCameraTarget;
	// カメラの位置と注視点を指定する
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, m_cameraTarget);
	RadianTranslation();
	// カメラの視野角を設定する
	SetupCamera_Perspective(m_viewRadianAngle);
	// カメラのnear,farを設定する
	// 画面に表示される距離の範囲を設定する
	// カメラからnear以上離れていてfarより近くにあるものが
	// ゲーム画面に表示される
	// farはあまり大きすぎる数字を設定しないように気を付ける(表示バグに繋がる)
	SetCameraNearFar(kCameraNearClip, kCameraFarClip);
	m_isLockOn = false;
	m_lockonHandle = LoadGraph(L"Data/UI/Lockon.png");
}

void Camera::End()
{
	// カメラの位置、注視点をリセットする
	m_cameraPos = kDefaultCameraPos;
	m_cameraTarget = kCameraTarget;
	DeleteLightHandle(m_lightHandle);
	DeleteGraph(m_lockonHandle);
}

void Camera::Update()
{
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_input);
	if (Pad::isTrigger(PAD_INPUT_10) && VSize(VSub(m_lockOnCameraPos, m_playerPos)) <= kMaxLockonRange)
	{
		if (!m_isLockOn)
		{
			m_isLockOn = true;
		}
		else
		{
			m_isLockOn = false;
		}
	}
	else if (VSize(VSub(m_lockOnCameraPos, m_playerPos)) >= kMaxLockonRange)
	{
		m_isLockOn = false;
	}

	if (!m_isLockOn)
	{
		m_cameraTarget = VAdd(m_playerPos, VGet(0.0f, kCameraTarget.y, 0.0f));
	}

	// カメラの角度の計算
	if (m_input.Rx > 0)
	{
		m_targetAngleHorizontal += kCameraAngleSpeed;
	}
	if(m_input.Rx < 0)
	{
		m_targetAngleHorizontal -= kCameraAngleSpeed;
	}
	if (m_input.Ry < 0)
	{
		m_targetAngleVertical += kCameraAngleSpeed;
		if (m_targetAngleVertical > kCameraPitchUpLimit) // カメラが上限を超えないように制限
		{
			m_targetAngleVertical = kCameraPitchUpLimit;
		}
	}
	if (m_input.Ry > 0)
	{
		m_targetAngleVertical -= kCameraAngleSpeed;
		if (m_targetAngleVertical < kCameraPitchDownLimit) // カメラが下限を下回らないように制限
		{
			m_targetAngleVertical = kCameraPitchDownLimit;
		}
	}
	//printfDx(L"m_targetAngleVertical:%f\nm_targetAngleHorizontal:%f\n", m_targetAngleVertical, m_targetAngleHorizontal);
	m_cameraAngleHorizontal = std::lerp(m_cameraAngleHorizontal,m_targetAngleHorizontal, kLerpSpeed);
	m_cameraAngleVertical = std::lerp(m_cameraAngleVertical,m_targetAngleVertical, kLerpSpeed);
	MATRIX rotX,rotY; // カメラの回転行列
	float cameraToPlayerLength;
	rotY = MGetRotY(m_cameraAngleHorizontal); // 水平方向の回転はY軸回転
	rotX = MGetRotX(m_cameraAngleVertical); // 垂直方向の回転はX軸回転
	cameraToPlayerLength = kCameraToPlayerLength; // カメラからプレイヤーまでの初期の距離をセット

	// カメラの座標を算出
	// Z軸にカメラとプレイヤーとの距離分だけ伸びたベクトルを
	// 垂直方向回転(X軸回転)させたあと水平方向回転(Y軸回転)して更に
	// 注視点の座標を足したものがカメラの座標
	m_cameraPos = VAdd(VTransform(VTransform(VGet(0.0f, 0.0f, -kCameraToPlayerLength), rotX), rotY), VGet(m_playerPos.x,m_playerPos.y + kDefaultCameraPos.y,m_playerPos.z));
	//ResolveCollisionWithStage();
	
	if (m_isLockOn)
	{
		// 本来注視したい位置（プレイヤーとロックオン対象の中間点）
		VECTOR desiredTarget = VAdd(m_playerPos, VScale(VSub(m_lockOnCameraPos, m_playerPos), 0.5f));
		m_cameraTarget = VAdd(VScale(m_cameraTarget, 1.0f - kLerpSpeed), VScale(desiredTarget, kLerpSpeed));
		m_lockonRotateAngle += kLockonRotateSpeed;
		if (m_lockonRotateAngle > DX_TWO_PI_F)
		{
			m_lockonRotateAngle -= DX_TWO_PI_F;
		}
	}
	
	//DrawFormatString(0,0,0xffffff,L"m_cameraTarget.x:%f,m_cameraTarget.y:%f,m_cameraTarget.z:%f", m_cameraTarget.x, m_cameraTarget.y, m_cameraTarget.z);
	SetCameraPositionAndTarget_UpVecY(m_cameraPos, m_cameraTarget); // カメラを計算した位置に設定する
	//printfDx(L"targetPos.x;%f, targetPos.y;%f, targetPos.z;%f\n",m_cameraTarget.x, m_cameraTarget.y, m_cameraTarget.z);
}

void Camera::Draw()
{
	//DrawSphere3D(m_cameraTarget,30.0f,8,0xff0000,0xffffff,true);
	if (m_isLockOn)
	{
		DrawBillboard3D(m_lockOnCameraPos, 0.5f, 0.5f, 384, m_lockonRotateAngle, m_lockonHandle, true);
	}
}

void Camera::SetLockOnPosition(VECTOR lockOnPos)
{
	m_lockOnCameraPos = lockOnPos;
	m_lockOnCameraPos.y = m_lockOnCameraPos.y + 100.0f;
}

void Camera::RadianTranslation()
{
	m_viewRadianAngle = kViewAngle * (DX_PI_F / kDegreesPerCircle);
}

void Camera::ResolveCollisionWithStage()
{
	// X方向の壁判定
	if (m_cameraPos.x < kStageMinX + kCameraRadius) 
	{
		m_cameraPos.x = kStageMinX + kCameraRadius;
	}
	else if (m_cameraPos.x > kStageMaxX - kCameraRadius) 
	{
		m_cameraPos.x = kStageMaxX - kCameraRadius;
	}

	// Z方向の壁判定
	if (m_cameraPos.z < kStageMinZ + kCameraRadius) 
	{
		m_cameraPos.z = kStageMinZ + kCameraRadius;
	}
	else if (m_cameraPos.z > kStageMaxZ - kCameraRadius) 
	{
		m_cameraPos.z = kStageMaxZ - kCameraRadius;
	}
}


