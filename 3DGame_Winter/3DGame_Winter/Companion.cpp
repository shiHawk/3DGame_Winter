#include "Companion.h"
#include <cmath>
namespace
{
	constexpr VECTOR kDefaultPos = { 100.0f,0.0f,-60.0f };
	constexpr VECTOR kDefaultVec = { 0.0f,0.0f,0.0f };
	constexpr VECTOR kDefaultDir = { 0.0f,270.0f * DX_PI_F / 180.0f,0.0f };
	constexpr float kSphereRadius = 20.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x00f000;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 3.0f;
	constexpr float kPlayerMoveSpeed = 10.0f; // プレイヤー操作モードでの移動速度
	constexpr float kJumpPower = 15.0f;
	constexpr float kGravity = -0.7f;
	constexpr float kAirResistance = 0.99f;
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
	constexpr int kAvoidanceAnimNo = 15;
	constexpr float kWalkAnimIncrement = 0.6f; // 歩行アニメーションの再生速度
	constexpr float kIdleAnimIncrement = 0.4f; // 待機アニメーションの再生速度
	constexpr float kAttackAnimIncrement = 0.5f; // 攻撃アニメーションの再生速度
	constexpr float kStrongAttackAnimIncrement = 0.5f; // 強攻撃アニメーションの再生速度
	constexpr float kSpecialSkilAnimIncriment = 0.6f;
	constexpr float kAvoidanceAnimIncrement = 0.4f; // 回避アニメーションの再生速度

	constexpr int kAttackPower = 10;
	constexpr int kStrongAttackPower = 30;
	constexpr int kSpecialAttackPower = 150;
	constexpr float kAttackDuration = 30.0f;
	constexpr float kStrongAttackDuration = 100.0f;
	constexpr float kSpecialSkilDuration = 110.0f;

	constexpr float kAttackRadius = 30.0f;
	constexpr float kSpecialSkilRadius = 300.0f;
	constexpr float kAttackRange = 90.0f;
	constexpr float kAttackCoolTime = 40.0f;

	// 線分の長さ
	constexpr float kForwardLineLength = 100.0f;
	constexpr float kBackLineLength = 540.0f;
	constexpr float kAutoTurnDistance = 300.0f;

	constexpr float kStopDistance = 160.0f; // プレイヤーや敵への追跡を止める距離
	constexpr float kLongRangeAttackDistance = 500.0f;
	constexpr float kCloseRangeAttackDistance = kStopDistance * 0.7f; // 近距離攻撃のみを行う距離
	constexpr float kEnemyLeashDistance = 600.0f; // 敵への追跡を諦める距離
	constexpr float kWarpDistance = 900.0f;
	constexpr float kPostWarpPosZ = 100.0f;

	constexpr float kStrongAttackBulletSpeed = 8.0f; // 弾の速度
	constexpr float kOffSetStrongAttackPosY = 25.0f; // 攻撃の位置補正
	constexpr double kAnalogDeadZone = 0.25; // アナログスティックのデッドゾーン

	constexpr float kAvoidanceFrame = 15.0f;
	constexpr float kAvoidanceMoveSpeed = 0.2f;
	constexpr float kColRadius = 40.0f;
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
	m_distanceToEnemy(0.0f),
	m_distanceToPlayer(0.0f),
	m_angleY(0.0f),
	m_specialGauge(0.0f),
	m_attackCoolTimer(0.0f),
	m_avoidanceTimer(0.0f),
	m_isAvoidanceFlag(false)
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
		if (Pad::isTrigger(PAD_INPUT_1) && !m_isJump)
		{
			m_vec.y = kJumpPower;
			m_isJump = true;
		}
		if (Pad::isTrigger(PAD_INPUT_3) && !m_isAvoidanceFlag) // 回避
		{
			m_avoidanceTimer = kAvoidanceFrame;
			m_isAvoidanceFlag = true;
		}
		if (m_isAvoidanceFlag)
		{
			OnAvoidance();
			// 減速処理
			m_vec.x *= kMoveDecRate;
			m_vec.z *= kMoveDecRate;
			if (m_avoidanceTimer <= 0.0f)
			{
				m_vec.x = 0.0f;
				m_vec.z = 0.0f;
				m_isAvoidanceFlag = false;
			}
		}
		UpdatePlayerControlState();
		printfDx(L"m_isJump:%d\n", m_isJump);
	}
	else
	{
		UpdateAIState();
		if (m_distanceToPlayer > kWarpDistance) // プレイヤーと離れすぎたらプレイヤーの近くにワープする
		{
			m_pos = VGet(m_playerPos.x, m_playerPos.y, m_playerPos.z - kPostWarpPosZ);
			m_attack.active = false;
			m_companionState = CompanionState::FOLLOW_PLAYER;
		}
	}
	m_isInAttackSequence = m_companionState != CompanionState::NORMAL && m_companionState != CompanionState::FOLLOW_PLAYER
						 && m_companionState != CompanionState::TRACK_ENEMY;
	if (m_isInAttackSequence)
	{
		m_vec = { 0.0f,0.0f,0.0f };
	}
	if (m_controlMode == ControlMode::PLAYER)
	{
		m_vec.y += kGravity;
	}
	else
	{
		m_vec.y += kGravity*10.0f;
	}
	//if (m_pos.y + m_vec.y < 0.0f)
	//{
	//	m_pos.y = 0.0f; // 地面に固定
	//	m_vec.y = 0.0f; // 縦速度をゼロ
	//	m_isJump = false;
	//}
	//else
	//{
	//	m_pos.y += m_vec.y;
	//}
	m_companionToEnemy = VSub(m_enemyPos, m_pos);
	m_distanceToEnemy = VSize(m_companionToEnemy);
	// 敵への方向ベクトル（正規化）を毎フレーム計算
	if (m_distanceToEnemy > 0.0f)
	{
		m_dirToEnemy = VNorm(m_companionToEnemy);
	}
	else
	{
		// 敵との距離が0（または敵がいない）場合はゼロベクトル
		m_dirToEnemy = VGet(0.0f, 0.0f, 0.0f);
	}
	m_forwardDir.x = sinf(m_angleY);
	m_forwardDir.y = 0.0f;
	m_forwardDir.z = cosf(m_angleY);
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
	//if (nextPos.z >= kBackLimit - kWallOffset)
	//{
	//	nextPos.z = kBackLimit - kWallOffset;
	//	m_vec.z = 0.0f;
	//}
	//else if (nextPos.z <= kFrontLimit + kWallOffset)
	//{
	//	nextPos.z = kFrontLimit + kWallOffset;
	//	m_vec.z = 0.0f;
	//}

	//// X方向(左右)制限
	//if (nextPos.x <= kLeftLimit + kWallOffset)
	//{
	//	nextPos.x = kLeftLimit + kWallOffset;
	//	m_vec.x = 0.0f;
	//}
	//else if (nextPos.x >= kRightLimit - kWallOffset)
	//{
	//	nextPos.x = kRightLimit - kWallOffset;
	//	m_vec.x = 0.0f;
	//}

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
	//m_pos = nextPos;
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
#ifdef _DEBUG
	if (m_companionState == CompanionState::NORMAL_ATTACK)
	{
		DrawSphere3D(m_attack.pos, m_attack.radius, kDivNum, kSphereDifColor, kSphereSpcColor, false);
	}
	DrawLine3D(lineStart, lineEnd, kSphereDifColor);
#endif 
	if (m_companionState == CompanionState::STRONG_ATTACK || m_companionState == CompanionState::SPECIALSKIL)
	{
		DrawSphere3D(m_attack.pos, m_attack.radius, kDivNum, kSphereDifColor, kSphereSpcColor, true);
	}
	MV1DrawModel(m_modelHandle);
}

void Companion::OnAttack()
{
	m_attackPower = kAttackPower;
	m_attack.radius = kAttackRadius;
	m_attack.dir = VNorm(VGet(sinf(m_angleY), 0.0f, cosf(m_angleY)));
	m_attack.active = true;
	m_attack.pos = VAdd(m_pos, VScale(m_attack.dir, kAttackRange));
	m_attack.timer = kAttackDuration;
}

void Companion::OnStrongAttack()
{
	m_attackPower = kStrongAttackPower;
	m_attack.radius = kAttackRadius;
	m_attack.timer = kStrongAttackDuration;
	m_attack.active = true;
	if (m_controlMode == ControlMode::PLAYER)
	{
		// プレイヤー操作時: 弾の方向をキャラクターの現在の向き (m_angleY) に基づいて水平に設定
		// Y成分は 0.0f にして水平に飛ばす
		m_attack.dir = VNorm(VGet(sinf(m_angleY), 0.0f, cosf(m_angleY)));
	}
	else // ControlMode::COMPANION (AIモード)
	{
		// AI操作時: 弾の方向を敵の方向 (m_dirToEnemy) に設定 (Y成分を含む)
		m_attack.dir = VNorm(m_dirToEnemy);
	}
	VECTOR forwardVec = VNorm(VGet(sinf(m_angleY), 0.0f, cosf(m_angleY)));
	m_attack.pos = VAdd(m_pos,VScale(forwardVec,kSphereRadius*2.0f));
	m_attack.pos.y = kOffSetStrongAttackPosY;
}

void Companion::OnSpecialSkil()
{
	m_attackPower = kStrongAttackPower;
	m_attack.radius = kSpecialSkilRadius;
	m_attack.dir = VNorm(VGet(sinf(m_angleY), 0.0f, cosf(m_angleY)));
	m_attack.active = true;
	m_attack.pos = m_enemyPos;
	m_attack.timer = kSpecialSkilDuration;
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
		RotatingToAttack();

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
		if (m_attack.active)
		{
			m_attack.pos = VAdd(m_attack.pos, VScale(m_attack.dir, kStrongAttackBulletSpeed)); // 毎フレーム位置の更新
			m_attack.timer--;
			ChangeAnim(m_modelHandle,kStrongAttackAnimNo,false, kStrongAttackAnimIncrement);
			if (m_attack.timer <= 0.0f)
			{
				m_attack.active = false;
				m_companionState = CompanionState::NORMAL;
				m_attackCoolTimer = kAttackCoolTime;
			}
		}
		break;
	case Companion::CompanionState::SPECIALSKIL:
		if (m_attack.active)
		{
			m_attack.timer--;
			ChangeAnim(m_modelHandle, kSpecialSkilAnimNo, false, kSpecialSkilAnimIncriment); 
			if (m_attack.timer < 0.0f)
			{
				m_attack.active = false;
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
	if (m_isAvoidanceFlag) // 回避フラグが立っている間は、他のステート処理（特にUpdateMovement）を行わない
	{
		return;
	}
	// AI用のステートはプレイヤー操作時はNORMALとして扱う
	if (m_companionState == CompanionState::FOLLOW_PLAYER || m_companionState == CompanionState::TRACK_ENEMY)
	{
		m_companionState = CompanionState::NORMAL;
	}

	switch (m_companionState)
	{
	case Companion::CompanionState::NORMAL:
		UpdateMovement(m_moveInput); // 移動処理
		if (m_attackCoolTimer <= 0.0f)
		{
			if (Pad::isTrigger(PAD_INPUT_4))
			{
				OnAttack();
				m_companionState = CompanionState::NORMAL_ATTACK;
				m_attackCoolTimer = kAttackCoolTime;
			}
			else if (Pad::isTrigger(PAD_INPUT_2))
			{
				if (m_distanceToEnemy > 0.0f) // 敵がいる場合のみ回転
				{
					m_companionState = CompanionState::TURN_FOR_STRONG_ATTACK; // 回転ステートに遷移
				}
				else
				{
					OnStrongAttack();
					m_companionState = CompanionState::STRONG_ATTACK;
					m_attackCoolTimer = kAttackCoolTime;
				}
			}
			else if (Pad::isTrigger(PAD_INPUT_5))
			{
				if (m_distanceToEnemy > 0.0f) // 敵がいる場合のみ回転
				{
					m_companionState = CompanionState::TURN_FOR_SPECIALSKIL; // 回転ステートに遷移
				}
				else
				{
					OnSpecialSkil();
					m_companionState = CompanionState::SPECIALSKIL;
					m_attackCoolTimer = kAttackCoolTime;
				}
				// SpecialSkil 発動時に敵の方向を即座に向く
				if (m_distanceToEnemy > 0.0f) // 敵が有効な場合
				{
					// m_dirToEnemy を使って敵の方向の角度（ラジアン）を計算
					float targetAngle = atan2f(m_dirToEnemy.x, m_dirToEnemy.z);

					// キャラクターの向き(m_angleY)を即座に敵の方向に設定
					m_angleY = targetAngle;
				}
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
			m_companionState = CompanionState::NORMAL;// 予期せずactiveがfalseになったらNORMALに戻る
		}
		break;
	case Companion::CompanionState::STRONG_ATTACK:
		m_attack.pos = VAdd(m_attack.pos, VScale(m_attack.dir, kStrongAttackBulletSpeed)); // 毎フレーム位置の更新
		if (m_attack.active)
		{
			m_attack.timer--;
			ChangeAnim(m_modelHandle, kStrongAttackAnimNo, false, kStrongAttackAnimIncrement);
			if (m_attack.timer < 0.0f || VSize(VSub(m_attack.pos,m_enemyPos)) <= m_attack.radius*2.0f)
			{
				m_attack.active = false;
				m_companionState = CompanionState::NORMAL;
			}
		}
		else
		{
			m_companionState = CompanionState::NORMAL;// 予期せずactiveがfalseになったらNORMALに戻る
		}
		break;
	case Companion::CompanionState::SPECIALSKIL:
		if (m_attack.active)
		{
			m_attack.timer--;
			ChangeAnim(m_modelHandle, kSpecialSkilAnimNo, false, kSpecialSkilAnimIncriment);
			if (m_attack.timer < 0.0f)
			{
				m_attack.active = false;
				m_companionState = CompanionState::NORMAL;
			}
		}
		else
		{
			m_companionState = CompanionState::NORMAL;// 予期せずactiveがfalseになったらNORMALに戻る
		}
		break;
	case CompanionState::TURN_FOR_STRONG_ATTACK:
	{
		RotatingToAttack(); // 滑らかに回転

		// 敵の方向への目標角度を再計算
		float targetAngle = atan2f(m_dirToEnemy.x, m_dirToEnemy.z);
		float diff = targetAngle - m_angleY;
		if (diff > DX_PI_F) diff -= 2.0f * DX_PI_F;
		else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;

		// 角度の差が閾値以下になったら攻撃開始
		if (std::abs(diff) < kAngleThreshold) // kAngleThreshold は既に 0.1f で定義されています
		{
			OnStrongAttack();
			m_companionState = CompanionState::STRONG_ATTACK;
			m_attackCoolTimer = kAttackCoolTime;
		}
		break;
	}
	case Companion::CompanionState::TURN_FOR_SPECIALSKIL:
	{
		RotatingToAttack(); // 滑らかに回転

		float targetAngle = atan2f(m_dirToEnemy.x, m_dirToEnemy.z);
		float diff = targetAngle - m_angleY;
		if (diff > DX_PI_F) diff -= 2.0f * DX_PI_F;
		else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;

		if (std::abs(diff) < kAngleThreshold)
		{
			OnSpecialSkil();
			m_companionState = CompanionState::SPECIALSKIL;
			m_attackCoolTimer = kAttackCoolTime;
		}
		break;
	}
	}
}

float Companion::GetColRadius()
{
	return kColRadius;
}

VECTOR Companion::GetDir()
{
	VECTOR dir;
	dir.x = sinf(m_angleY + DX_PI_F);
	dir.y = 0.0f;
	dir.z = cosf(m_angleY + DX_PI_F);
	return dir;
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
		float targetAngle = atan2f(moveDir.x, moveDir.z);
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
		m_vec.x = moveDir.x * kPlayerMoveSpeed;
		m_vec.z = moveDir.z * kPlayerMoveSpeed;
	}
	else // 入力がない場合
	{
		if (m_isJump)
		{
			// 空中にいるときは、減速をほとんどさせない
			m_vec.x *= kAirResistance;
			m_vec.z *= kAirResistance;
		}
		else
		{
			m_vec.x *= kMoveDecRate;
			m_vec.z *= kMoveDecRate;
		}
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

void Companion::RotatingToAttack()
{
	// 敵がいない、または距離が0の場合は回転しない
	if (m_distanceToEnemy <= 0.0f)
	{
		return;
	}

	// 敵の方向への目標角度を計算
	// m_dirToEnemy は Update 関数で既に正規化されているはずです。
	float targetAngle = atan2f(m_dirToEnemy.x, m_dirToEnemy.z);

	// 現在の角度と目標角度の差分を計算
	float diff = targetAngle - m_angleY;

	// 角度の差分を -π から π の範囲に正規化 (最短回転)
	if (diff > DX_PI_F)      diff -= 2.0f * DX_PI_F;
	else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;

	// lerpによる滑らかな角度補間
	m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);

	// m_angleYを -π から π の範囲に正規化
	if (m_angleY > DX_PI_F)      m_angleY -= 2.0f * DX_PI_F;
	else if (m_angleY < -DX_PI_F) m_angleY += 2.0f * DX_PI_F;
}

void Companion::OnAvoidance()
{
	m_avoidanceTimer--;
	VECTOR avoidDir;
	// スティック入力がある場合、その方向(m_moveInput)を回避方向にする
	if (VSize(m_moveInput) > 0.0f)
	{
		avoidDir = VNorm(m_moveInput);
	}
	else
	{
		// スティック入力がない場合、キャラクターの向いている方向(m_forwardDir)を回避方向にする
		avoidDir = m_forwardDir;
	}
	m_vec.x = m_forwardDir.x * kAvoidanceMoveSpeed;
	m_vec.z = m_forwardDir.z * kAvoidanceMoveSpeed;
	ChangeAnim(m_modelHandle, kAvoidanceAnimNo, false, kAvoidanceAnimIncrement);
}
