#include "Player.h"
#include "Pad.h"
#include "Camera.h"
#include <cmath>
namespace
{
    constexpr VECTOR kDefaultPos = { 0.0f,0.0f,0.0f };
    constexpr VECTOR kDefaultVec = { 0.0f,0.0f,0.0f };
    constexpr VECTOR kRightDir = { 0.0,270.0f * DX_PI_F / 180.0f,0.0f };
	constexpr float kSphereRadius = 20.0f;
	constexpr int kDivNum = 8;
	constexpr int kSphereDifColor = 0x000fff;
	constexpr int kSphereSpcColor = 0xffffff;
	constexpr float kMoveSpeed = 10.0f;
    constexpr float kJumpPower = 10.0f;
    constexpr float kGravity = -0.5f;
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
    constexpr int kStrongAttackPower = 30;
    constexpr int kComboFinishAttackPower = 45;
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

    constexpr float kModelScale = 50.0f; // モデルのスケール
    constexpr int kIdleAnimNo = 1;
    constexpr int kWalkAnimNo = 3;
    constexpr int kAttackAnimNo = 31;
    constexpr int kStrongAttackAnimNo = 40;
    constexpr float kComboFinishAttackAnimNo = 41;
    constexpr int kSpecialSkilAnimNo = 38; // 必殺技アニメーション
    constexpr int kAvoidanceAnimNo = 15;
    constexpr float kAnimIncrement = 0.4f; // アニメーションの再生速度
    constexpr float kIdleAnimIncrement = 0.4f; // 待機アニメーションの再生速度
    constexpr float kWalkAnimIncrement = 0.6f; // 歩行アニメーションの再生速度
    constexpr float kAttackAnimIncrement = 0.7f; // 攻撃アニメーションの再生速度
    constexpr float kStrongAttackAnimIncrement = 0.9f; // 強攻撃アニメーションの再生速度
    constexpr float kComboFinishAttackAnimIncrement = 0.7f; 
}

Player::Player():
    m_playerState(PlayerState::NORMAL),
    m_angleY(0.0f),
    m_isJump(false),
    m_forwardDir({0.0f,0.0f,0.0f}),
    m_enemyPos({0.0f,0.0f,0.0f}),
    m_attack(kAttackRadius,{0.0f,0.0f,0.0f},false,0.0f,{0.0f,0.0f,0.0f}),
    m_attack2(kStrongAttackRadius,{0.0f,0.0f,0.0f},false,0.0f,{0.0f,0.0f,0.0f}),
    m_comboFinish(kComboFinishAttackRadius,{0.0f,0.0f,0.0f},false,0.0f,{0.0f,0.0f,0.0f}),
    m_specialSkil(kSpecialSkilRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
    m_dirToEnemy({0.0f,0.0f,0.0f}),
    m_moveInput({ 0.0f,0.0f,0.0f }),
    m_distanceToEnemy(0.0f),
    m_isInAttackSequence(false),
    m_avoidanceTimer(0.0f),
    m_isAvoidanceFlag(false),
    m_comboStep(0),
    m_comboWindowTimer(0.0f),
    m_specialGauge(0),
    m_isSpecialSkilFlag(false)
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
}

void Player::End()
{
    MV1DeleteModel(m_modelHandle);
}

void Player::Update()
{
    m_moveInput = HandleInput();
    UpdatePlayerState();
    //printfDx(L"m_playerState:%d\n", m_playerState);
    //printfDx(L"m_comboWindowTimer:%f\n", m_comboWindowTimer);
    if (Pad::isTrigger(PAD_INPUT_1) && m_pos.y <= 0.0f)
    {
        m_vec.y = kJumpPower;
        m_isJump = true;
    }
    if (!m_isInAttackSequence)
    {
        m_vec.y += kGravity;
    }
    /*else
    {
        m_vec.y += kGravity * 0.5f;
    }*/
    
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
    
    if (Pad::isTrigger(PAD_INPUT_3)&&!m_isAvoidanceFlag)
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
    if (m_specialGauge >= kMaxGauge)
    {
        m_specialGauge = kMaxGauge;
    }
    if (/*m_specialGauge > kGaugeConsumption && */Pad::isTrigger(PAD_INPUT_5))
    {
        m_specialGauge -= kGaugeConsumption;
        OnSpecialSkil();
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
    /*DINPUT_JOYSTATE input;
    int i;
    int Color;
    //入力状態を取得
    GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

    //画面に構造体の中身を描画
    Color = GetColor(255, 255, 255);
    DrawFormatString(0, 0, Color, L"X:%d Y:%d Z:%d",
        input.X, input.Y, input.Z);
    DrawFormatString(0, 16, Color, L"Rx:%d Ry:%d Rz:%d",
        input.Rx, input.Ry, input.Rz);
    DrawFormatString(0, 32, Color, L"Slider 0:%d 1:%d",
        input.Slider[0], input.Slider[1]);
    DrawFormatString(0, 48, Color, L"POV 0:%d 1:%d 2:%d 3:%d",
        input.POV[0], input.POV[1],
        input.POV[2], input.POV[3]);
    DrawString(0, 64, L"Button", Color);
    for (i = 0; i < 32; i++)
    {
        DrawFormatString(64 + i % 8 * 64, 64 + i / 8 * 16, Color,
            L"%2d:%d", i, input.Buttons[i]);
    }*/
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
    // isLockOn の状態に応じて、向きの決め方を変える
    if (m_isInAttackSequence)
    {
        // 敵との距離がkLockOnRange以下なら敵のほうを向く
        if (m_distanceToEnemy <= kAutoTurnDistance)
        {
            // ロックオン時 敵の方向を向く
            m_dirToEnemy = VSub(m_enemyPos, m_pos);
            if (VSize(VGet(m_dirToEnemy.x, 0.0f, m_dirToEnemy.z)) > 0.001f)
            {
                float targetAngle = atan2f(-m_dirToEnemy.x, -m_dirToEnemy.z);
                float diff = targetAngle - m_angleY;
                if (diff > DX_PI_F)       diff -= 2.0f * DX_PI_F;
                else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;
                m_angleY = std::lerp(m_angleY, m_angleY + diff, kRotateSpeed);

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
    // 移動処理
    // isLockOn の状態に関わらずスティック入力に応じて移動・減速を制御する
    if (VSize(moveDir) > 0.0f)
    {
        // 5.移動ベクトルを更新
        m_vec.x = moveDir.x * kMoveSpeed;
        m_vec.z = moveDir.z * kMoveSpeed;
    }
    else // 入力がない場合
    {
        // 減速処理
        m_vec.x *= kMoveDecRate;
        m_vec.z *= kMoveDecRate;
    }
}

void Player::UpdatePlayerState()
{
    switch (m_playerState)
    {
    case Player::PlayerState::NORMAL:
        UpdateMovement(m_moveInput);
        m_comboStep = 0; // 通常時はコンボ数をリセット
        if (Pad::isTrigger(PAD_INPUT_4) && !m_attack.active)
        {
            m_comboStep = 1; // コンボ1段階目
            m_dirToEnemy = VSub(m_enemyPos, m_pos); // 攻撃ボタンを押したら敵との距離を測る
            m_distanceToEnemy = VSize(m_dirToEnemy);
            if (m_distanceToEnemy <= kAutoTurnDistance) // 距離がkLockOnRange以下なら敵の方向を向く
            {
                m_playerState = PlayerState::ROTATING_TO_ATTACK;
            }
            else // 距離が遠いなら方向転換を行わず即攻撃
            {
                OnAttack(); // 攻撃実行
                m_playerState = PlayerState::ATTACKING;
            }
        }
        if (Pad::isTrigger(PAD_INPUT_2) && !m_attack2.active) // 単発強攻撃
        {
            m_comboStep = 0; // 単発攻撃なのでコンボはリセット
            m_dirToEnemy = VSub(m_enemyPos, m_pos); // 攻撃ボタンを押したら敵との距離を測る
            m_distanceToEnemy = VSize(m_dirToEnemy);
            if (m_distanceToEnemy <= kAutoTurnDistance) // 距離がkLockOnRange以下なら敵の方向を向く
            {
                m_playerState = PlayerState::ROTATING_TO_ATTACK2;
            }
            else // 距離が遠いなら方向転換を行わず即攻撃
            {
                OnAttack2(); // 攻撃実行
                m_playerState = PlayerState::ATTACKING2;
            }
        }
        break;
    case Player::PlayerState::ROTATING_TO_ATTACK:
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
            OnAttack(); // 攻撃実行
            m_playerState = PlayerState::ATTACKING; // 攻撃状態へ移行
        }
        break;
    }
    case Player::PlayerState::ROTATING_TO_ATTACK2:
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
            OnAttack2(); // 攻撃実行
            m_playerState = PlayerState::ATTACKING2; // 攻撃状態へ移行
        }
        break;
    }
    case Player::PlayerState::ATTACKING:
        UpdateMovement(m_moveInput);
        if (Pad::isTrigger(PAD_INPUT_2)) // 攻撃中に強ボタン
        {
            m_attack.active = false; // 現在の攻撃をキャンセル
            m_comboStep = 2;         // コンボ2段階目へ

            // 2段目攻撃 (強A) を開始
            m_dirToEnemy = VSub(m_enemyPos, m_pos);
            m_distanceToEnemy = VSize(m_dirToEnemy);
            if (m_distanceToEnemy <= kAutoTurnDistance)
            {
                m_playerState = PlayerState::ROTATING_TO_ATTACK2;
            }
            else
            {
                OnAttack2();
                m_playerState = PlayerState::ATTACKING2;
            }
            break; // ATTACKING の処理を抜ける
        }
        // 攻撃タイマーを進める
        if (m_attack.active)
        {
            m_attack.timer--;
            ChangeAnim(m_modelHandle, kAttackAnimNo, false, kAttackAnimIncrement);
            if (m_attack.timer < 0.0f)
            {
                m_attack.active = false;
                // NORMALに戻さず、コンボ受付状態 (WINDOW) へ
                m_playerState = PlayerState::COMBO_WINDOW;
                m_comboWindowTimer = kComboWindowTime;
            }
        }
        break;
    case Player::PlayerState::ATTACKING2:
        UpdateMovement(m_moveInput);
        // m_comboStep == 2 は「弱→強」の連携中である証
        if (m_comboStep == 2 && Pad::isTrigger(PAD_INPUT_2))
        {
            m_attack2.active = false; // 現在の攻撃をキャンセル
            m_comboStep = 3;          // コンボ3段階目へ

            // 3段目攻撃 (強B) を開始
            m_dirToEnemy = VSub(m_enemyPos, m_pos);
            m_distanceToEnemy = VSize(m_dirToEnemy);
            if (m_distanceToEnemy <= kAutoTurnDistance)
            {
                m_playerState = PlayerState::ROTATING_TO_COMBOFINISH;
            }
            else
            {
                OnCombFinishAttack();
                m_playerState = PlayerState::ATTACKING_COMBOFINISH;
            }
            break; // ATTACKING2 の処理を抜ける
        }
        // 攻撃タイマーを進める
        if (m_attack2.active)
        {
            m_attack2.timer--;
            ChangeAnim(m_modelHandle, kStrongAttackAnimNo, false, kStrongAttackAnimIncrement);
            if (m_attack2.timer < 0.0f)
            {
                m_attack2.active = false;
                // m_comboStep が 2 ならコンボ受付へ、それ以外 (単発) なら NORMAL へ
                if (m_comboStep == 2)
                {
                    m_playerState = PlayerState::COMBO_WINDOW;
                    m_comboWindowTimer = kComboWindowTime;
                }
                else
                {
                    m_playerState = PlayerState::NORMAL;
                    m_comboStep = 0; // コンボ終了
                }
            }
        }
        break;
    case Player::PlayerState::ROTATING_TO_COMBOFINISH:
    {
        UpdateMovement(m_moveInput);
        m_dirToEnemy = VSub(m_enemyPos, m_pos);
        float targetAngle = atan2f(-m_dirToEnemy.x, -m_dirToEnemy.z);
        float diff = targetAngle - m_angleY;

        if (diff > DX_PI_F)     diff -= 2.0f * DX_PI_F;
        else if (diff < -DX_PI_F) diff += 2.0f * DX_PI_F;

        if (std::abs(diff) < kAngleThreshold)
        {
            m_angleY = targetAngle;
            OnCombFinishAttack(); // 3段目攻撃を実行
            m_playerState = PlayerState::ATTACKING_COMBOFINISH;
        }
        break;
    }
    case Player::PlayerState::ATTACKING_COMBOFINISH:
        UpdateMovement(m_moveInput);
        // この攻撃からは連携しない
        if (m_comboFinish.active)
        {
            m_comboFinish.timer--;
            ChangeAnim(m_modelHandle, kComboFinishAttackAnimNo, false, kComboFinishAttackAnimIncrement); // 3段目アニメ
            if (m_comboFinish.timer < 0.0f)
            {
                m_comboFinish.active = false;
                m_playerState = PlayerState::NORMAL; // 攻撃が終わったら通常状態へ
                m_comboStep = 0; // コンボ終了
            }
        }
        break;
    case Player::PlayerState::COMBO_WINDOW:
        UpdateMovement(m_moveInput);
        m_comboWindowTimer--;
        // コンボ受付時間内に次の入力があったか
        // 1段階目 (弱) の後 -> 強 (PAD_INPUT_2)
        if (m_comboStep == 1 && Pad::isTrigger(PAD_INPUT_2))
        {
            m_comboStep = 2; // 2段階目へ
            m_dirToEnemy = VSub(m_enemyPos, m_pos);
            m_distanceToEnemy = VSize(m_dirToEnemy);
            if (m_distanceToEnemy <= kAutoTurnDistance)
            {
                m_playerState = PlayerState::ROTATING_TO_ATTACK2;
            }
            else
            {
                OnAttack2();
                m_playerState = PlayerState::ATTACKING2;
            }
        }
        // 2段階目 (強A) の後 -> 強 (PAD_INPUT_2)
        else if (m_comboStep == 2 && Pad::isTrigger(PAD_INPUT_2))
        {
            m_comboStep = 3; // 3段階目へ
            m_dirToEnemy = VSub(m_enemyPos, m_pos);
            m_distanceToEnemy = VSize(m_dirToEnemy);
            if (m_distanceToEnemy <= kAutoTurnDistance)
            {
                m_playerState = PlayerState::ROTATING_TO_COMBOFINISH;
            }
            else
            {
                OnCombFinishAttack();
                m_playerState = PlayerState::ATTACKING_COMBOFINISH;
            }
        }
        // 時間切れ、または他の行動（ジャンプや回避など）でコンボ中断
        else if (m_comboWindowTimer <= 0.0f || Pad::isTrigger(PAD_INPUT_1) || Pad::isTrigger(PAD_INPUT_3))
        {
            m_playerState = PlayerState::NORMAL;
            m_comboStep = 0;
        }
        break;
    case Player::PlayerState::SPECIALSKIL:
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
        break;
    }
}
