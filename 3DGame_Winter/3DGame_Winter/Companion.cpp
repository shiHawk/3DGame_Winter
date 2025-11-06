#include "Companion.h"
#include <cmath>
namespace
{
	constexpr VECTOR kDefaultPos = { 100.0f,0.0f,-60.0f };
	constexpr VECTOR kDefaultVec = { 0.0f,0.0f,0.0f };
	constexpr VECTOR kDefaultDir = { 0.0,270.0f * DX_PI_F / 180.0f,0.0f };
	constexpr float kSphereRadius = 20.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x00f000;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 2.0f;
	constexpr float kJumpPower = 10.0f;
	constexpr float kGravity = -5.5f;
	constexpr float kMoveThreshold = 0.1f; // 移動とみなす閾値
	// 減速
	constexpr float kMoveDecRate = 0.80f;
	constexpr float kRotateSpeed = 0.4f; // 方向転換の速度
	constexpr float kAngleThreshold = 0.1f; // 角度の差の閾値
	constexpr float kFrontLimit = -1000.0f; // ステージ奥
	constexpr float kBackLimit = 1000.0f;   // ステージ手前
	constexpr float kLeftLimit = -1000.0f;  // ステージ左
	constexpr float kRightLimit = 1000.0f;  // ステージ右
	constexpr float kWallOffset = 0.001f;

	constexpr float kModelScale = 50.0f; // モデルのスケール
	constexpr int kIdleAnimNo = 1;
	constexpr int kWalkAnimNo = 3;
	constexpr int kAttackAnimNo = 31;
	constexpr int kStrongAttackAnimNo = 60;
	constexpr int kSpecialSkilAnimNo = 62;
	constexpr float kComboFinishAttackAnimNo = 41;
	constexpr float kWalkAnimIncrement = 0.6f; // 歩行アニメーションの再生速度
	constexpr float kIdleAnimIncrement = 0.4f; // 待機アニメーションの再生速度
	constexpr float kAttackAnimIncrement = 0.5f; // 攻撃アニメーションの再生速度
	constexpr float kStrongAttackAnimIncrement = 0.5f; // 強攻撃アニメーションの再生速度
	constexpr float kSpecialSkilAnimIncriment = 0.6f;

	constexpr int kAttackPower = 10;
	constexpr int kStrongAttackPower = 30;
	constexpr int kSpecialAttackPower = 150;
	constexpr float kAttackDuration = 30.0f;
	constexpr float kStrongAttackDuration = 60.0f;
	constexpr float kSpecialSkilDuration = 90.0f;

	constexpr float kAttackRadius = 30.0f;
	constexpr float kSpecialSkilRadius = 300.0f;
	constexpr float kAttackRange = 60.0f;
	constexpr float kAttackCoolTime = 40.0f;

	// 線分の長さ
	constexpr float kForwardLineLength = 100.0f;
	constexpr float kBackLineLength = 540.0f;
	constexpr float kAutoTurnDistance = 300.0f;

	constexpr float kStopDistance = 160.0f; // プレイヤーや敵への追跡を止める距離
	constexpr float kLongRangeAttackDistance = 400.0f;
	constexpr float kCloseRangeAttackDistance = kStopDistance * 0.7f; // 近距離攻撃のみを行う距離
	constexpr float kEnemyLeashDistance = 600.0f; // 敵への追跡を諦める距離
	constexpr float kWarpDistance = 800.0f;
	constexpr float kPostWarpPosZ = 100.0f;

	constexpr float kStrongAttackBulletSpeed = 6.0f; // 弾の速度
	constexpr double kAnalogDeadZone = 0.25; // アナログスティックのデッドゾーン
}

Companion::Companion():
	m_controlMode(ControlMode::COMPANION),
	m_companionState(CompanionState::NORMAL),
	m_forwardDir({0.0f,0.0f,0.0f}),
	m_enemyPos({ 0.0f,0.0f,0.0f }),
	m_playerPos({ 0.0f,0.0f,0.0f }),
	m_dirToEnemy({ 0.0f,0.0f,0.0f }),
	m_dirToPlayer({ 0.0f,0.0f,0.0f }),
	m_companionToEnemy({ 0.0f,0.0f,0.0f }),
	m_companionToPlayer({ 0.0f,0.0f,0.0f }),
	m_attack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_strongAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_specialSkil(kSpecialSkilRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
	m_distanceToEnemy(0.0f),
	m_distanceToPlayer(0.0f),
	m_angleY(0.0f),
	m_specialGauge(0.0f),
	m_attackCoolTimer(0.0f)
{
}

Companion::~Companion()
{
}

void Companion::Init(std::shared_ptr<Camera> pCamera)
{
	m_pCamera = pCamera;
	m_pos = kDefaultPos;
	m_vec = kDefaultVec;
	m_distanceToEnemy = 0.0f;
	m_attack.active = false;
	m_strongAttack.active = false;
	m_specialSkil.active = false;
	m_modelHandle = MV1LoadModel(L"Data/model/Mage.mv1");
	MV1SetScale(m_modelHandle, VGet(kModelScale, kModelScale, kModelScale));
	MV1SetRotationXYZ(m_modelHandle,kDefaultDir);
	AttachAnim(m_modelHandle,kIdleAnimNo);
}

void Companion::End()
{
	MV1DeleteModel(m_modelHandle);
}

void Companion::Update()
{
	// 敵の位置が未設定（初期値）なら何もしない
	if (m_enemyPos.x == 0.0f && m_enemyPos.y == 0.0f && m_enemyPos.z == 0.0f)
	{
		return;
	}
	//printfDx(L"m_distanceToEnemy:%f\n", m_distanceToEnemy);
	if (m_controlMode == ControlMode::PLAYER)
	{
		m_moveInput = HandleInput();
		UpdatePlayerControlState();
	}
	else
	{
		UpdateAIState();
	}

	m_isInAttackSequence = m_companionState != CompanionState::NORMAL && m_companionState != CompanionState::FOLLOW_PLAYER
						 && m_companionState != CompanionState::TRACK_ENEMY;

	if (m_distanceToPlayer > kWarpDistance) // プレイヤーと離れすぎたらプレイヤーの近くにワープする
	{
		m_pos = VGet(m_playerPos.x, m_playerPos.y, m_playerPos.z - kPostWarpPosZ);
		m_companionState = CompanionState::FOLLOW_PLAYER;
	}

	m_vec.y += kGravity;
	if (m_pos.y + m_vec.y < 0.0f)
	{
		m_pos.y = 0.0f;   // 地面に固定
		m_vec.y = 0.0f;   // 縦速度をゼロ
	}
	m_companionToEnemy = VSub(m_enemyPos, m_pos);
	m_distanceToEnemy = VSize(m_companionToEnemy);
	m_companionToPlayer = VSub(m_playerPos, m_pos);
	m_distanceToPlayer = VSize(m_companionToPlayer);
	if (m_companionState == CompanionState::NORMAL)
	{
		if (m_distanceToEnemy <= kEnemyLeashDistance) // 敵に向かって移動する
		{
			m_companionState = CompanionState::TRACK_ENEMY;
		}
		else // プレイヤーに追従する
		{
			m_companionState = CompanionState::FOLLOW_PLAYER;
		}
	}
	
	VECTOR nextPos = VAdd(m_pos, m_vec); // 仮の次の位置
	// Z方向(前後)制限
	if (nextPos.z >= kBackLimit - kWallOffset)
	{
		nextPos.z = kBackLimit - kWallOffset;
		m_vec.z = 0.0f;
	}
	else if (nextPos.z <= kFrontLimit + kWallOffset)
	{
		nextPos.z = kFrontLimit + kWallOffset;
		m_vec.z = 0.0f;
	}

	// X方向(左右)制限
	if (nextPos.x <= kLeftLimit + kWallOffset)
	{
		nextPos.x = kLeftLimit + kWallOffset;
		m_vec.x = 0.0f;
	}
	else if (nextPos.x >= kRightLimit - kWallOffset)
	{
		nextPos.x = kRightLimit - kWallOffset;
		m_vec.x = 0.0f;
	}

	if (!m_isInAttackSequence)
	{
		if (VSize(VGet(m_vec.x, 0.0f, m_vec.z)) > kMoveThreshold)
		{
			// 移動中→移動アニメーションへ変更
			ChangeAnim(m_modelHandle, kWalkAnimNo, true, kWalkAnimIncrement);
		}
		else
		{
			// 停止後→待機アニメーションへ変更
			ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
		}
	}
	m_pos = nextPos;
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_angleY+DX_PI_F, 0.0f));
	MV1SetPosition(m_modelHandle,m_pos);
	UpdateAnim();
}

void Companion::Draw()
{
	// 向きに合わせて線分を描画
	m_forwardDir.x = sinf(m_angleY) * kForwardLineLength;
	m_forwardDir.y = 0.0f;
	m_forwardDir.z = cosf(m_angleY) * kForwardLineLength;
	VECTOR lineStart = VGet(m_pos.x, m_pos.y + kSphereRadius *0.5f, m_pos.z);
	VECTOR lineEnd = VAdd(lineStart, m_forwardDir);

	if (m_attack.active)
	{
		DrawSphere3D(m_attack.pos, m_attack.radius, kDivNum, kSphereDifColor, kSphereSpcColor, false);
	}
	if (m_strongAttack.active)
	{
		DrawSphere3D(m_strongAttack.pos, m_strongAttack.radius, kDivNum, kSphereDifColor, kSphereSpcColor, false);
	}
	if (m_specialSkil.active)
	{
		DrawSphere3D(m_specialSkil.pos, m_specialSkil.radius, kDivNum, kSphereDifColor, kSphereSpcColor, false);
	}
	DrawLine3D(lineStart, lineEnd, kSphereDifColor);
	MV1DrawModel(m_modelHandle);
}

void Companion::OnAttack()
{
	m_attackPower = kAttackPower;
	m_attack.dir = VNorm(VGet(sinf(m_angleY ), 0.0f, cosf(m_angleY )));
	m_attack.active = true;
	m_attack.pos = VAdd(m_pos, VScale(m_attack.dir, kAttackRange));
	m_attack.timer = kAttackDuration;
}

void Companion::OnStrongAttack()
{
	m_attackPower = kStrongAttackPower;
	m_strongAttack.timer = kStrongAttackDuration;
	m_strongAttack.active = true;
	m_strongAttack.dir = VNorm(m_dirToEnemy);
	VECTOR forwardVec = VNorm(VGet(sinf(m_angleY), 0.0f, cosf(m_angleY)));
	m_strongAttack.pos = VAdd(m_pos,VScale(forwardVec,kSphereRadius*2.0f));
	m_strongAttack.pos.y = m_pos.y;
}

void Companion::OnSpecialSkil()
{
	m_attackPower = kStrongAttackPower;
	m_specialSkil.dir = VNorm(VGet(sinf(m_angleY), 0.0f, cosf(m_angleY)));
	m_specialSkil.active = true;
	m_specialSkil.pos = m_enemyPos;
	m_specialSkil.timer = kSpecialSkilDuration;
}

void Companion::UpdateAIState()
{
	if (m_attackCoolTimer > 0.0f)
	{
		m_attackCoolTimer--;
	}
	switch (m_companionState)
	{
	case Companion::CompanionState::NORMAL:
	{
		// 敵が遠い場合は減速して停止
		m_vec = VScale(m_vec, kMoveDecRate);
		break;
	}
	case Companion::CompanionState::FOLLOW_PLAYER:
	{
		if (m_distanceToEnemy <= kEnemyLeashDistance) // プレイヤーの追従中に射程内に敵が入ったら敵の追跡に移行する
		{
			m_companionState = CompanionState::TRACK_ENEMY;
			break; // このフレームでのFOLLOW_PLAYERの処理は中断
		}
		m_dirToPlayer = VNorm(m_companionToPlayer);
		float targetAngle = atan2f(m_dirToPlayer.x, m_dirToPlayer.z);
		float diff = targetAngle - m_angleY;
		if (diff > DX_PI_F)       diff -= 2.0f * DX_PI_F;
		else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;
		m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);

		if (m_angleY > DX_PI_F)       m_angleY -= 2.0f * DX_PI_F;
		else if (m_angleY < -DX_PI_F) m_angleY += 2.0f * DX_PI_F;

		if (m_distanceToPlayer > kStopDistance)
		{
			m_forwardDir.x = sinf(m_angleY);
			m_forwardDir.z = cosf(m_angleY);
			m_vec = VScale(m_forwardDir, kMoveSpeed);
		}
		else
		{
			m_vec = { 0.0f,0.0f,0.0f };
			m_companionState = CompanionState::NORMAL;
		}
		break;
	}
	case Companion::CompanionState::TRACK_ENEMY:
	{
		m_dirToEnemy = VNorm(m_companionToEnemy);
		float targetAngle = atan2f(m_dirToEnemy.x, m_dirToEnemy.z);
		float diff = targetAngle - m_angleY;
		if (diff > DX_PI_F)       diff -= 2.0f * DX_PI_F;
		else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;
		m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);

		if (m_angleY > DX_PI_F)       m_angleY -= 2.0f * DX_PI_F;
		else if (m_angleY < -DX_PI_F) m_angleY += 2.0f * DX_PI_F;

		if (m_distanceToEnemy > kEnemyLeashDistance) // kEnemyLeashDistanceより敵が離れたら追跡をやめる
		{
			m_vec = { 0.0f,0.0f,0.0f };
			m_companionState = CompanionState::FOLLOW_PLAYER;
		}
		else if (m_distanceToEnemy > kLongRangeAttackDistance) // 遠距離攻撃の射程に入るまで追跡する
		{
			m_forwardDir.x = sinf(m_angleY);
			m_forwardDir.z = cosf(m_angleY);
			m_vec = VScale(m_forwardDir, kMoveSpeed);
		}
		else if (m_distanceToEnemy > kCloseRangeAttackDistance) // 近距離攻撃の範囲に近づくまで遠距離攻撃
		{
			m_vec = { 0.0f,0.0f,0.0f }; // 移動を停止
			if (m_attackCoolTimer <= 0.0f) // クールタイムが終わっているかチェック
			{
				OnStrongAttack();
				m_companionState = CompanionState::STRONG_ATTACK;
			}
			else
			{
				m_companionState = CompanionState::NORMAL;
			}
		}
		else // 近距離攻撃の範囲に入ったら近距離攻撃
		{
			m_vec = { 0.0f,0.0f,0.0f }; // 移動を停止
			if (m_attackCoolTimer <= 0.0f) // クールタイムが終わっているかチェック
			{
				OnAttack();
				m_companionState = CompanionState::NORMAL_ATTACK;
			}
			else
			{
				m_companionState = CompanionState::NORMAL;
			}
		}
		break;
	}
	case Companion::CompanionState::NORMAL_ATTACK:
		if (m_attack.active)
		{
			m_attack.timer--;
			ChangeAnim(m_modelHandle, kAttackAnimNo, false, kAttackAnimIncrement);
			if (m_attack.timer <= 0.0f)
			{
				m_attack.active = false;
				m_companionState = CompanionState::NORMAL;
				m_attackCoolTimer = kAttackCoolTime;
			}
		}
		break;
	case Companion::CompanionState::STRONG_ATTACK:
		if (m_strongAttack.active)
		{
			m_strongAttack.pos = VAdd(m_strongAttack.pos, VScale(m_strongAttack.dir, kStrongAttackBulletSpeed)); // 毎フレーム位置の更新
			m_strongAttack.timer--;
			ChangeAnim(m_modelHandle,kStrongAttackAnimNo,false, kStrongAttackAnimIncrement);
			if (m_strongAttack.timer <= 0.0f)
			{
				m_strongAttack.active = false;
				m_companionState = CompanionState::NORMAL;
				m_attackCoolTimer = kAttackCoolTime;
			}
		}
		break;
	case Companion::CompanionState::SPECIALSKIL:
		if (m_specialSkil.active)
		{
			m_specialSkil.timer--;
			ChangeAnim(m_modelHandle, kSpecialSkilAnimNo, false, kSpecialSkilAnimIncriment); 
			if (m_specialSkil.timer < 0.0f)
			{
				m_specialSkil.active = false;
				m_companionState = CompanionState::NORMAL;
			}
		}
		break;
	}
}

void Companion::UpdatePlayerControlState()
{
	if (m_attackCoolTimer > 0.0f)
	{
		m_attackCoolTimer--;
	}
	// AI用のステートはプレイヤー操作時はNORMALとして扱う
	if (m_companionState == CompanionState::FOLLOW_PLAYER || m_companionState == CompanionState::TRACK_ENEMY)
	{
		m_companionState == CompanionState::NORMAL;
	}

	switch (m_companionState)
	{
	case Companion::CompanionState::NORMAL:
		UpdateMovement(m_moveInput); // 移動処理
		if (m_attackCoolTimer > 0.0f)
		{
			if (Pad::isTrigger(PAD_INPUT_4))
			{
				OnAttack();
				m_companionState = CompanionState::NORMAL_ATTACK;
				m_attackCoolTimer = kAttackCoolTime;
			}
			else if (Pad::isTrigger(PAD_INPUT_2))
			{
				OnStrongAttack();
				m_companionState == CompanionState::STRONG_ATTACK;
				m_attackCoolTimer = kAttackCoolTime;
			}
			else if (Pad::isTrigger(PAD_INPUT_5))
			{
				OnSpecialSkil();
				m_companionState == CompanionState::SPECIALSKIL;
				m_attackCoolTimer = kAttackCoolTime;
			}
		}
		break;
	case Companion::CompanionState::NORMAL_ATTACK:
		if (m_attack.active)
		{
			m_attack.timer--;
			ChangeAnim(m_modelHandle, kAttackAnimNo, false, kAttackAnimIncrement);
			if (m_attack.timer < 0.0f)
			{
				m_attack.active = false;
				m_companionState = CompanionState::NORMAL;
			}
		}
		else
		{
			m_companionState = CompanionState::NORMAL;// 予期せず active が false になったら NORMAL に戻る
		}
		break;
	case Companion::CompanionState::STRONG_ATTACK:
		m_strongAttack.pos = VAdd(m_strongAttack.pos, VScale(m_strongAttack.dir, kStrongAttackBulletSpeed)); // 毎フレーム位置の更新
		if (m_strongAttack.active)
		{
			m_strongAttack.timer--;
			ChangeAnim(m_modelHandle, kStrongAttackAnimNo, false, kStrongAttackAnimIncrement);
			if (m_strongAttack.timer < 0.0f)
			{
				m_strongAttack.active = false;
				m_companionState = CompanionState::NORMAL;
			}
		}
		else
		{
			m_companionState = CompanionState::NORMAL;// 予期せず active が false になったら NORMAL に戻る
		}
		break;
	case Companion::CompanionState::SPECIALSKIL:
		if (m_specialSkil.active)
		{
			m_specialSkil.timer--;
			ChangeAnim(m_modelHandle, kSpecialSkilAnimNo, false, kSpecialSkilAnimIncriment);
			if (m_specialSkil.timer < 0.0f)
			{
				m_specialSkil.active = false;
				m_companionState = CompanionState::NORMAL;
			}
		}
		else
		{
			m_companionState = CompanionState::NORMAL;// 予期せず active が false になったら NORMAL に戻る
		}
		break;
	}
}

void Companion::UpdateMovement(const VECTOR& moveDir)
{
	if (!m_isInAttackSequence)
	{
		if (VSize(VGet(m_vec.x, 0.0f, m_vec.z)) > kMoveThreshold)
		{
			// 移動中→移動アニメーションへ変更
			ChangeAnim(m_modelHandle, kWalkAnimNo, true, kWalkAnimIncrement);
		}
		else
		{
			// 停止後→待機アニメーションへ変更
			ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
		}
	}
	if (VSize(moveDir) > 0.0f)
	{
		float targetAngle = atan2f(-moveDir.x, -moveDir.z); // Player.cpp と同じ
		float diff = targetAngle - m_angleY;
		if (diff > DX_PI_F)       diff -= 2.0f * DX_PI_F;
		else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;

		m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);

		if (m_angleY > DX_PI_F)       m_angleY -= 2.0f * DX_PI_F;
		else if (m_angleY < -DX_PI_F) m_angleY += 2.0f * DX_PI_F;
	}

	// 移動処理
	if (VSize(moveDir) > 0.0f)
	{
		m_vec.x = moveDir.x * kMoveSpeed;
		m_vec.z = moveDir.z * kMoveSpeed;
	}
	else // 入力がない場合
	{
		m_vec.x *= kMoveDecRate;
		m_vec.z *= kMoveDecRate;
	}
}

VECTOR Companion::HandleInput()
{
	if (m_controlMode == ControlMode::COMPANION)
	{
		return VGet(0.0f, 0.0f, 0.0f);
	}
	// アナログスティックの入力を取得
	int stickX = 0;
	int stickY = 0;
	GetJoypadAnalogInput(&stickX, &stickY, DX_INPUT_PAD1);
	SetJoypadDeadZone(DX_INPUT_PAD1, kAnalogDeadZone);

	// 入力値を-1.0fから1.0fの範囲に正規化
	float inputX = stickX / 1000.0f;
	float inputZ = -stickY / 1000.0f; // Y軸をZ軸に(奥方向)
	// 入力がない場合はゼロベクトルを返す
	if (inputX == 0.0f && inputZ == 0.0f)
	{
		return VGet(0.0f, 0.0f, 0.0f);
	}

	// 入力ベクトルを正規化
	VECTOR inputVec = VNorm(VGet(inputX, 0.0f, inputZ));
	// カメラの向きに合わせて入力ベクトルを回転
	float cameraYaw = -m_pCamera->GetHorizonrtalAngle();
	float cosY = cosf(cameraYaw);
	float sinY = sinf(cameraYaw);

	VECTOR moveDir;
	moveDir.x = inputVec.x * cosY - inputVec.z * sinY;
	moveDir.z = inputVec.x * sinY + inputVec.z * cosY;
	moveDir.y = 0.0f;
	return moveDir;
}
