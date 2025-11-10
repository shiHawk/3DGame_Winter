#include "Player.h"
#include "Pad.h"
#include "Camera.h"
#include "DxLib.h"
#include <cmath>
namespace
{
    constexpr VECTOR kDefaultPos = { 0.0f,0.0f,-700.0f };
    constexpr VECTOR kDefaultVec = { 0.0f,0.0f,0.0f };
    constexpr VECTOR kRightDir = { 0.0f,270.0f * DX_PI_F / 180.0f,0.0f };
	constexpr float kSphereRadius = 20.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x000fff;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 10.0f;
	constexpr float kCompanionMoveSpeed = 3.0f;
    constexpr float kJumpPower = 10.0f;
    constexpr float kGravity = -0.5f;
    // 重力係数（攻撃中）
    const float kAttackGravityMultiplier = 0.4f;
	// 減速
	constexpr float kMoveDecRate = 0.60f;
    // 線分の長さ
    constexpr float kForwardLineLength = 100.0f;
    constexpr float kBackLineLength = 540.0f;
    // アナログスティックのデッドゾーン
    constexpr double kAnalogDeadZone = 0.25;
    constexpr float kMoveThreshold = 0.1f; // 移動とみなす入力の閾値
    constexpr float kRotateSpeed = 0.4f; // 方向転換の速度
    constexpr float kAngleThreshold = 0.1f; // 角度の差の閾値
    constexpr float kFrontLimit = -1000.0f; // ステージ奥
    constexpr float kBackLimit = 1000.0f;   // ステージ手前
    constexpr float kLeftLimit = -1000.0f;  // ステージ左
    constexpr float kRightLimit = 1000.0f;  // ステージ右
    constexpr float kWallOffset = 0.001f;

    // 各攻撃の攻撃力
    constexpr int kAttackPower = 15;
    constexpr int kStrongAttackPower = 45;
    constexpr int kComboFinishAttackPower = 60;
    constexpr int kSpecialSkilPower = 150;

    constexpr float kAttackRange = 60.0f;
    constexpr float kAutoTurnDistance = 250.0f;
    // 各攻撃の持続時間
    constexpr float kAttackDuration = 20.0f;
    constexpr float kStrongAttackDuration = 40.0f;
    constexpr float kComboFinishAttackDuration = 40.0f;
    constexpr float kSpecialSkilDuration = 70.0f;

    constexpr float kAvoidanceFrame = 15.0f;
    constexpr float kAvoidanceMoveSpeed = 0.3f;

    // 各攻撃の攻撃範囲
    constexpr float kAttackRadius = 30.0f;
    constexpr float kStrongAttackRadius = 40.0f;
    constexpr float kComboFinishAttackRadius = 50.0f;
    constexpr float kSpecialSkilRadius = 300.0f;

    constexpr int kGaugeIncreaseAmount = 5; // 必殺ゲージ増加量
    constexpr int kMaxGauge = 200; // ゲージの最大量
    constexpr int kGaugeConsumption = 100; // ゲージの消費量

    constexpr float kComboWindowTime = 20.0f;
    constexpr float kAIComboCoolTime = 10.0f; // AIのコンボ間クールタイム

    constexpr float kModelScale = 50.0f; // モデルのスケール
    constexpr int kIdleAnimNo = 1;
    constexpr int kWalkAnimNo = 3;
    constexpr int kAttackAnimNo = 31;
    constexpr int kStrongAttackAnimNo = 40;
    constexpr int kComboFinishAttackAnimNo = 41;
    constexpr int kSpecialSkilAnimNo = 38; // 必殺技アニメーション
    constexpr int kAvoidanceAnimNo = 15;
    constexpr float kAnimIncrement = 0.4f; // アニメーションの再生速度
    constexpr float kIdleAnimIncrement = 0.4f; // 待機アニメーションの再生速度
    constexpr float kWalkAnimIncrement = 0.6f; // 歩行アニメーションの再生速度
    constexpr float kAttackAnimIncrement = 0.7f; // 攻撃アニメーションの再生速度
    constexpr float kStrongAttackAnimIncrement = 0.9f; // 強攻撃アニメーションの再生速度
    constexpr float kComboFinishAttackAnimIncrement = 0.7f; 

    constexpr float kEnemyLeashDistance = 500.0f; // これ以上敵から離れたら、追跡をやめてプレイヤーの元に戻る距離
    constexpr float kNearAttackDistance = kAttackRange * 1.5f; // 近距離攻撃のために、敵にこの距離まで近づく (kAttackRange より少し広め)
    constexpr float kFollowTargetDistance = 160.0f; // 追従対象（プレイヤー）の、この距離まで近づいたら停止する
    constexpr float kWarpDistance = 800.0f;
    constexpr float kPostWarpPosZ = 100.0f;
}

Player::Player():
    m_controlMode(ControlMode::PLAYER),
    m_playerState(PlayerState::NORMAL),
    m_angleY(0.0f),
    m_forwardDir({0.0f,0.0f,0.0f}),
    m_enemyPos({0.0f,0.0f,0.0f}),
    m_followTargetPos(0.0f,0.0f,0.0f),
    m_attack(kAttackRadius,{0.0f,0.0f,0.0f},false,0.0f,{0.0f,0.0f,0.0f}),
    m_attack2(kStrongAttackRadius,{0.0f,0.0f,0.0f},false,0.0f,{0.0f,0.0f,0.0f}),
    m_comboFinish(kComboFinishAttackRadius,{0.0f,0.0f,0.0f},false,0.0f,{0.0f,0.0f,0.0f}),
    m_specialSkil(kSpecialSkilRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
    m_dirToEnemy({0.0f,0.0f,0.0f}),
    m_distanceToEnemy(0.0f),
    m_distanceToFollowTarget(0.0f),
    m_avoidanceTimer(0.0f),
    m_isAvoidanceFlag(false),
    m_comboStep(0),
    m_comboWindowTimer(0.0f),
    m_aiComboCoolTimer(0.0f),
    m_specialGauge(0),
    m_isSpecialSkilFlag(false),
    m_aiWillDo3HitCombo(false)
{
}

void Player::Init(std::shared_ptr<Camera> pCamera)
{
	m_pCamera = pCamera;
	m_pos = kDefaultPos;
	m_vec = kDefaultVec;
    m_angleY = 0.0f;
    m_isJump = false;
    m_attackPower = kAttackPower;
    m_playerState = PlayerState::NORMAL;
    m_attack.active = false;
    m_attack2.active = false;
    m_comboFinish.active = false;
    m_specialSkil.active = false;
    m_distanceToEnemy = 0.0f;
    m_modelHandle = MV1LoadModel(L"Data/model/Barbarian.mv1");
    MV1SetScale(m_modelHandle, VGet(kModelScale, kModelScale, kModelScale));
    MV1SetRotationXYZ(m_modelHandle, kRightDir);
    AttachAnim(m_modelHandle, kIdleAnimNo);
    SRand(GetTickCount64());
}

void Player::End()
{
    MV1DeleteModel(m_modelHandle);
}

void Player::Update()
{
    m_moveInput = HandleInput();
    UpdatePlayerState();
    //printfDx(L"m_distanceToFollowTarget:%f\n", m_distanceToFollowTarget);
    if (m_controlMode == ControlMode::PLAYER)
    {
        if (Pad::isTrigger(PAD_INPUT_1) && m_pos.y <= 0.0f)
        {
            m_vec.y = kJumpPower;
            m_isJump = true;
        }
        if (Pad::isTrigger(PAD_INPUT_3) && !m_isAvoidanceFlag)
        {
            m_avoidanceTimer = kAvoidanceFrame;
            m_isAvoidanceFlag = true;
        }
        if (/*m_specialGauge > kGaugeConsumption && */Pad::isTrigger(PAD_INPUT_5))
        {
            m_specialGauge -= kGaugeConsumption;
            OnSpecialSkil();
        }
    }
    else
    {
        m_distanceToFollowTarget = VSize(VSub(m_followTargetPos, m_pos));
        //printfDx(L"m_distanceToFollowTarget:%f\n", m_distanceToFollowTarget);
        if (m_distanceToFollowTarget > kWarpDistance)
        {
            m_pos = VGet(m_followTargetPos.x, m_followTargetPos.y, m_followTargetPos.z - kPostWarpPosZ);
        }
    }
    
    if (!m_isInAttackSequence)
    {
        m_vec.y += kGravity;
    }
    else
    {
        m_vec = { 0.0f,0.0f,0.0f };
        m_vec.y += kGravity * kAttackGravityMultiplier; // 攻撃時はかかる重力を小さくする
    }
    
    if (m_pos.y + m_vec.y < 0.0f)
    {
        m_pos.y = 0.0f;   // 地面に固定
        m_vec.y = 0.0f;   // 縦速度をゼロ
        m_isJump = false; // 着地
    }
    else
    {
        m_pos.y += m_vec.y;
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
    if (m_specialGauge >= kMaxGauge)
    {
        m_specialGauge = kMaxGauge;
    }
    if (m_specialGauge <= 0)
    {
        m_specialGauge = 0;
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
    if(nextPos.x <= kLeftLimit + kWallOffset)
    {
        nextPos.x = kLeftLimit + kWallOffset;
        m_vec.x = 0.0f;
    }
    else if(nextPos.x >= kRightLimit - kWallOffset) 
    {
        nextPos.x = kRightLimit - kWallOffset;
        m_vec.x = 0.0f;
    }
    m_pos = nextPos;
    MV1SetPosition(m_modelHandle, m_pos);
    MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_angleY, 0.0f));
    UpdateAnim();
    //DINPUT_JOYSTATE input;
    //int i;
    //int Color;
    ////入力状態を取得
    //GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
    ////画面に構造体の中身を描画
    //Color = GetColor(255, 255, 255);
    //DrawFormatString(0, 0, Color, L"X:%d Y:%d Z:%d",
    //    input.X, input.Y, input.Z);
    //DrawFormatString(0, 16, Color, L"Rx:%d Ry:%d Rz:%d",
    //    input.Rx, input.Ry, input.Rz);
    //DrawFormatString(0, 32, Color, L"Slider 0:%d 1:%d",
    //    input.Slider[0], input.Slider[1]);
    //DrawFormatString(0, 48, Color, L"POV 0:%d 1:%d 2:%d 3:%d",
    //    input.POV[0], input.POV[1],
    //    input.POV[2], input.POV[3]);
    //DrawString(0, 64, L"Button", Color);
    //for (i = 0; i < 32; i++)
    //{
    //    DrawFormatString(64 + i % 8 * 64, 64 + i / 8 * 16, Color,
    //        L"%2d:%d", i, input.Buttons[i]);
    //}
}

void Player::Draw()
{
    // 向きに合わせて線分を描画
    m_forwardDir.x = sinf(m_angleY+DX_PI_F) * kForwardLineLength;
    m_forwardDir.y = 0.0f;
    m_forwardDir.z = cosf(m_angleY+DX_PI_F) * kForwardLineLength;
    VECTOR lineStart = VGet(m_pos.x, m_pos.y + kSphereRadius / 2, m_pos.z);
    VECTOR lineEnd = VAdd(lineStart, m_forwardDir);

	//DrawSphere3D(m_pos,kSphereRadius,kDivNum,kSphereDifColor,kSphereSpcColor,true);
    MV1DrawModel(m_modelHandle);
#ifdef _DEBUG
    DrawLine3D(lineStart, lineEnd, kSphereDifColor);
    if (m_attack.active)
    {
        DrawSphere3D(m_attack.pos, m_attack.radius, kDivNum, kSphereDifColor, kSphereSpcColor, false);
    }
    if (m_attack2.active)
    {
        DrawSphere3D(m_attack2.pos, m_attack2.radius, kDivNum, kSphereDifColor, kSphereSpcColor, false);
    }
    if (m_comboFinish.active)
    {
        DrawSphere3D(m_comboFinish.pos, m_comboFinish.radius, kDivNum, kSphereDifColor, kSphereSpcColor, false);
    }
    if (m_specialSkil.active)
    {
        DrawSphere3D(m_specialSkil.pos, m_specialSkil.radius, kDivNum, kSphereDifColor, kSphereSpcColor, false);
    }
#endif
}

void Player::OnAttack()
{
    m_attackPower = kAttackPower;
    m_attack.dir = VNorm(VGet(sinf(m_angleY + DX_PI_F), 0.0f, cosf(m_angleY + DX_PI_F)));
    m_attack.active = true;
    m_attack.pos = VAdd(m_pos,VScale(m_attack.dir,kAttackRange));
    m_attack.timer = kAttackDuration;
    if (m_pos.y > 0.0f)
    {
        m_vec.y = 0.0f; // 垂直速度をゼロにし、上昇・下降を停止
    }
}

void Player::OnAttack2()
{
    m_attackPower = kStrongAttackPower;
    m_attack2.dir = VNorm(VGet(sinf(m_angleY + DX_PI_F), 0.0f, cosf(m_angleY + DX_PI_F)));
    m_attack2.active = true;
    m_attack2.pos = VAdd(m_pos, VScale(m_attack2.dir, kAttackRange));
    m_attack2.timer = kStrongAttackDuration;
    if (m_pos.y > 0.0f)
    {
        m_vec.y = 0.0f; // 垂直速度をゼロにし、上昇・下降を停止
    }
}

void Player::OnCombFinishAttack()
{
    m_attackPower = kComboFinishAttackPower;
    m_comboFinish.dir = VNorm(VGet(sinf(m_angleY + DX_PI_F), 0.0f, cosf(m_angleY + DX_PI_F)));
    m_comboFinish.active = true;
    m_comboFinish.pos = VAdd(m_pos, VScale(m_comboFinish.dir, kAttackRange));
    m_comboFinish.timer = kComboFinishAttackDuration;
}

void Player::OnSpecialSkil()
{
    m_attackPower = kSpecialSkilPower;
    m_specialSkil.active = true;
    m_specialSkil.pos = m_pos;
    m_specialSkil.timer = kSpecialSkilDuration;
    m_playerState = PlayerState::SPECIALSKIL;
}

void Player::OnAvoidance()
{
    m_avoidanceTimer--;
    m_vec.x = m_forwardDir.x * kAvoidanceMoveSpeed;
    m_vec.z = m_forwardDir.z * kAvoidanceMoveSpeed;
    ChangeAnim(m_modelHandle,kAvoidanceAnimNo,false,kAnimIncrement);
}

VECTOR Player::HandleInput()
{
    if (m_controlMode == ControlMode::COMPANION)
    {
        // 敵との距離を計算し、メンバー変数に保存
        m_dirToEnemy = VSub(m_enemyPos, m_pos);
        m_dirToEnemy.y = 0.0f; // 高低差は無視
        m_distanceToEnemy = VSize(m_dirToEnemy);

        // 追従対象（プレイヤー）との距離を計算
        VECTOR dirToTarget = VSub(m_followTargetPos, m_pos);
        dirToTarget.y = 0.0f;
        float distanceToTarget = VSize(dirToTarget);

        // 距離に基づいた移動判断
        // 敵が非常に遠い(kEnemyLeashDistance)場合、プレイヤーを追従する
        if (m_distanceToEnemy > kEnemyLeashDistance)
        {
            // プレイヤーを追従
            if (distanceToTarget > kFollowTargetDistance) // プレイヤーから離れていたら近づく
            {
                return VNorm(dirToTarget);
            }
            else
            {
                return VGet(0.0f, 0.0f, 0.0f); // プレイヤーの近くで停止
            }
        }
        // 敵が近距離攻撃の間合い (kNearAttackDistance) の外にいる場合
        else if (m_distanceToEnemy > kNearAttackDistance)
        {
            // 敵を追跡する
            return VNorm(m_dirToEnemy); // 正規化した敵への方向を返す
        }
        else
        {
            // 敵が近距離攻撃の間合いに入った場合、停止する
            // (攻撃の実行は UpdatePlayerState が担当)
            return VGet(0.0f, 0.0f, 0.0f);
        }
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

void Player::UpdateMovement(const VECTOR& moveDir)
{
    m_isInAttackSequence = m_playerState != PlayerState::NORMAL;
    // 攻撃中でなければ、移動状態に応じてアニメーションを切り替える
    if (!m_isInAttackSequence)
    {
        if (VSize(VGet(m_vec.x, 0.0f, m_vec.z)) > kMoveThreshold)
        {
            // 入力がある場合 → 移動アニメーションへ変更
            ChangeAnim(m_modelHandle, kWalkAnimNo, true, kWalkAnimIncrement);
        }
        else
        {
            // 入力がない場合 → 待機アニメーションへ変更
            ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
        }
    }
    
    // 回転処理
    if (m_isInAttackSequence)
    {
        // 敵との距離がkLockOnRange以下なら敵のほうを向く
        if (m_distanceToEnemy <= kAutoTurnDistance)
        {
            m_dirToEnemy = VSub(m_enemyPos, m_pos);
            if (VSize(VGet(m_dirToEnemy.x, 0.0f, m_dirToEnemy.z)) > 0.001f)
            {
                float targetAngle = atan2f(-m_dirToEnemy.x, -m_dirToEnemy.z); // 敵方向への角度(Y軸回転を求める)
                float diff = targetAngle - m_angleY; // 現在の角度と敵方向への角度の差分を求める
                // 差分をπ～-πの範囲に正規化する
                if (diff > DX_PI_F)       diff -= 2.0f * DX_PI_F;
                else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;

                m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed); // 角度を滑らかに変更
                // 最終的な角度をπ～-πの範囲に収める
                if (m_angleY > DX_PI_F)       m_angleY -= 2.0f * DX_PI_F;
                else if (m_angleY < -DX_PI_F) m_angleY += 2.0f * DX_PI_F;
            }
        }
    }
    else
    {
        // 通常時 スティックの入力方向を向く
        // 入力がある場合のみ回転処理を行う
        if (VSize(moveDir) > 0.0f)
        {
            float targetAngle = atan2f(-moveDir.x, -moveDir.z);
            float diff = targetAngle - m_angleY;
            if (diff > DX_PI_F)       diff -= 2.0f * DX_PI_F;
            else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;
            m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);

            if (m_angleY > DX_PI_F)       m_angleY -= 2.0f * DX_PI_F;
            else if (m_angleY < -DX_PI_F) m_angleY += 2.0f * DX_PI_F;
        }
    }
    bool isAerialAttack = m_isInAttackSequence && m_isJump;
    if (isAerialAttack)
    {
        m_vec.x = 0.0f;
        m_vec.z = 0.0f;
    }
    else
    {
        // 移動処理
        // isLockOn の状態に関わらずスティック入力に応じて移動・減速を制御する
        if (VSize(moveDir) > 0.0f)
        {
            // 5.移動ベクトルを更新
            if (m_controlMode == ControlMode::PLAYER)
            {
                m_vec.x = moveDir.x * kMoveSpeed;
                m_vec.z = moveDir.z * kMoveSpeed;
            }
            else
            {
                m_vec.x = moveDir.x * kCompanionMoveSpeed;
                m_vec.z = moveDir.z * kCompanionMoveSpeed;
            }
        }
        else // 入力がない場合
        {
            // 減速処理
            m_vec.x *= kMoveDecRate;
            m_vec.z *= kMoveDecRate;
        }
    }
}

void Player::UpdatePlayerState()
{
    // AI用の攻撃開始トリガー
    bool aiWantsToAttack = false;
    if (m_controlMode == ControlMode::COMPANION && m_playerState == PlayerState::NORMAL)
    {
        // HandleInput で計算済みの距離 m_distanceToEnemy を使う
        // 敵が攻撃範囲内 (kNearAttackDistance) にいたら攻撃開始
        if (m_distanceToEnemy <= kNearAttackDistance && !m_attack.active)
        {
            aiWantsToAttack = true;
        }
    }
    switch (m_playerState)
    {
    case Player::PlayerState::NORMAL:
        HandleStateNormal(aiWantsToAttack); // 引数でトリガーを渡す
        break;
    case Player::PlayerState::ROTATING_TO_ATTACK:
        RotatingToAttackAndAttack(&Player::OnAttack, PlayerState::ATTACKING);
        break;
    case Player::PlayerState::ROTATING_TO_ATTACK2:
        RotatingToAttackAndAttack(&Player::OnAttack2, PlayerState::ATTACKING2);
        break;
    case Player::PlayerState::ATTACKING:
        HandleStateAttacking();
        break;
    case Player::PlayerState::ATTACKING2:
        HandleStateAttacking2();
        break;
    case Player::PlayerState::ROTATING_TO_COMBOFINISH:
        RotatingToAttackAndAttack(&Player::OnCombFinishAttack, PlayerState::ATTACKING_COMBOFINISH);
        break;
    case Player::PlayerState::ATTACKING_COMBOFINISH:
        UpdateMovement(m_moveInput);
        UpdateAttackState(m_comboFinish, kComboFinishAttackAnimNo, kComboFinishAttackAnimIncrement, PlayerState::NORMAL);
        break;
    case Player::PlayerState::COMBO_WINDOW:
        HandleStateComboWindow();
        break;
    case Player::PlayerState::SPECIALSKIL:
        HandleStateSpecialSkil();
        break;
    }
}

void Player::HandleStateNormal(bool aiWantsToAttack)
{
    UpdateMovement(m_moveInput);
    m_comboStep = 0; // 通常時はコンボ数をリセット

    // プレイヤーの弱攻撃入力
    if (m_controlMode == ControlMode::PLAYER && Pad::isTrigger(PAD_INPUT_4) && !m_attack.active)
    {
        m_comboStep = 1;
        TryStartAttack(&Player::OnAttack, PlayerState::ROTATING_TO_ATTACK, PlayerState::ATTACKING);
    }
    // AIの攻撃トリガー
    else if (m_controlMode == ControlMode::COMPANION && aiWantsToAttack)
    {
        m_comboStep = 1;
        m_aiWillDo3HitCombo = (GetRand(1) == 1);
        TryStartAttack(&Player::OnAttack, PlayerState::ROTATING_TO_ATTACK, PlayerState::ATTACKING);
    }

    // プレイヤーの単発強攻撃入力 (弱攻撃とは独立して判定)
    if (m_controlMode == ControlMode::PLAYER && Pad::isTrigger(PAD_INPUT_2) && !m_attack2.active)
    {
        m_comboStep = 0; // 単発
        TryStartAttack(&Player::OnAttack2, PlayerState::ROTATING_TO_ATTACK2, PlayerState::ATTACKING2);
    }
}

void Player::HandleStateAttacking()
{
    UpdateMovement(m_moveInput);
    // プレイヤー操作時のコンボ連携（キャンセル）
    if (m_controlMode == ControlMode::PLAYER && Pad::isTrigger(PAD_INPUT_2))
    {
        m_attack.active = false; // 現在の攻撃をキャンセル
        m_comboStep = 2;         // 2段階目へ
        TryStartAttack(&Player::OnAttack2, PlayerState::ROTATING_TO_ATTACK2, PlayerState::ATTACKING2);
        return; // ステートが変更されたので、このフレームの処理は終了
    }

    // 攻撃タイマーを進める (コンボ連携しなかった場合)
    UpdateAttackState(m_attack, kAttackAnimNo, kAttackAnimIncrement, PlayerState::COMBO_WINDOW);
}

void Player::HandleStateAttacking2()
{
    UpdateMovement(m_moveInput);
    // プレイヤー操作時のコンボ連携（キャンセル）
    if (m_controlMode == ControlMode::PLAYER && m_comboStep == 2 && Pad::isTrigger(PAD_INPUT_2))
    {
        m_attack2.active = false; // 現在の攻撃をキャンセル
        m_comboStep = 3;          // 3段階目へ
        TryStartAttack(&Player::OnCombFinishAttack, PlayerState::ROTATING_TO_COMBOFINISH, PlayerState::ATTACKING_COMBOFINISH);
        return; // ステートが変更されたので、このフレームの処理は終了
    }

    // 攻撃タイマーを進める (コンボ連携しなかった場合)
    PlayerState nextState = (m_comboStep == 2) ? PlayerState::COMBO_WINDOW : PlayerState::NORMAL;
    UpdateAttackState(m_attack2, kStrongAttackAnimNo, kStrongAttackAnimIncrement, nextState);
}

void Player::HandleStateComboWindow()
{
    if (m_controlMode == ControlMode::COMPANION)
    {
        HandleAIComboWindow(); // AIの自動コンボ処理
    }
    else
    {
        HandlePlayerComboWindow(); // プレイヤーの入力待ち処理
    }
}

void Player::HandleStateSpecialSkil()
{
    if (m_specialSkil.active)
    {
        m_specialSkil.timer--;
        ChangeAnim(m_modelHandle, kSpecialSkilAnimNo, false, kComboFinishAttackAnimIncrement);
        if (m_specialSkil.timer < 0.0f)
        {
            m_specialSkil.active = false;
            m_playerState = PlayerState::NORMAL;
        }
    }
}

void Player::HandleAIComboWindow()
{
    if (m_aiComboCoolTimer > 0.0f)
    {
        m_aiComboCoolTimer--;
        return;
    }
    // 1段階目 (弱) が終わった後
    if (m_comboStep == 1)
    {
        m_comboStep = 2; // AIは必ず2段階目 (強A) に移行する
        TryStartAttack(&Player::OnAttack2, PlayerState::ROTATING_TO_ATTACK2, PlayerState::ATTACKING2);
        m_aiComboCoolTimer = kAIComboCoolTime;
    }
    // 2段階目 (強A) が終わった後
    else if (m_comboStep == 2)
    {
        if (m_aiWillDo3HitCombo) // 3段階目をやると決めていたか？
        {
            m_comboStep = 3;
            TryStartAttack(&Player::OnCombFinishAttack, PlayerState::ROTATING_TO_COMBOFINISH, PlayerState::ATTACKING_COMBOFINISH);
        }
        else
        {
            m_playerState = PlayerState::NORMAL; // 2ヒットで終了
            m_comboStep = 0;
            m_aiComboCoolTimer = 0.0f;
        }
    }
    else
    {
        // 安全装置
        m_playerState = PlayerState::NORMAL;
        m_comboStep = 0;
    }
}

void Player::HandlePlayerComboWindow()
{
    UpdateMovement(m_moveInput);
    m_comboWindowTimer--;

    // 1段階目 (弱) の後→強 (PAD_INPUT_2)
    if (m_comboStep == 1 && Pad::isTrigger(PAD_INPUT_2))
    {
        m_comboStep = 2; // 2段階目へ
        TryStartAttack(&Player::OnAttack2, PlayerState::ROTATING_TO_ATTACK2, PlayerState::ATTACKING2);
    }
    // 2段階目 (強A) の後→強 (PAD_INPUT_2)
    else if (m_comboStep == 2 && Pad::isTrigger(PAD_INPUT_2))
    {
        m_comboStep = 3; // 3段階目へ
        TryStartAttack(&Player::OnCombFinishAttack, PlayerState::ROTATING_TO_COMBOFINISH, PlayerState::ATTACKING_COMBOFINISH);
    }
    // 時間切れ、または他の行動（ジャンプや回避など）でコンボ中断
    else if (m_comboWindowTimer <= 0.0f || Pad::isTrigger(PAD_INPUT_1) || Pad::isTrigger(PAD_INPUT_3))
    {
        m_playerState = PlayerState::NORMAL;
        m_comboStep = 0;
    }
}

void Player::RotatingToAttackAndAttack(void(Player::* attackFunc)(), PlayerState nextState)
{
    UpdateMovement(m_moveInput);
    // 敵の方向を計算
    m_dirToEnemy = VSub(m_enemyPos, m_pos);
    float targetAngle = atan2f(-m_dirToEnemy.x, -m_dirToEnemy.z);
    float diff = targetAngle - m_angleY;

    // 角度の差を正規化 
    if (diff > DX_PI_F)      diff -= 2.0f * DX_PI_F;
    else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;

    // 角度の差がごくわずかになったら、攻撃を出す
    if (std::abs(diff) < kAngleThreshold)
    {
        m_angleY = targetAngle; // 最後に角度をぴったり合わせる
        (this->*attackFunc)(); // ← メンバ関数ポインタ呼び出し
        m_playerState = nextState; // 攻撃状態へ移行
    }
}

void Player::TryStartAttack(void(Player::* onAttackFunc)(), PlayerState rotationState, PlayerState attackState)
{
    // 攻撃実行の直前に、必ず最新の敵情報を参照する
    m_dirToEnemy = VSub(m_enemyPos, m_pos);
    m_distanceToEnemy = VSize(m_dirToEnemy);

    if (m_distanceToEnemy <= kAutoTurnDistance)
    {
        m_playerState = rotationState; // 近い→回転ステートへ
    }
    else
    {
        (this->*onAttackFunc)();     // 遠い→攻撃を即実行
        m_playerState = attackState; // 攻撃ステートへ
    }
}

void Player::UpdateAttackState(AttackSphere& attackData, int animNo, float animInc, PlayerState nextState)
{
    // 攻撃タイマーを進める
    if (attackData.active)
    {
        attackData.timer--;
        ChangeAnim(m_modelHandle, animNo, false, animInc);
        if (attackData.timer < 0.0f)
        {
            attackData.active = false;
            m_playerState = nextState;

            if (nextState == PlayerState::COMBO_WINDOW)
            {
                m_comboWindowTimer = kComboWindowTime; // コンボ受付時間を設定
                if (m_controlMode == ControlMode::COMPANION)
                {
                    m_aiComboCoolTimer = kAIComboCoolTime; // コンボの繋ぎ目にクールタイムを設定
                }
            }
            else if (nextState == PlayerState::NORMAL)
            {
                m_comboStep = 0; // コンボ終了
            }
        }
    }
}