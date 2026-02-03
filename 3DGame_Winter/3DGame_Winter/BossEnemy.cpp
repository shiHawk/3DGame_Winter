#include "BossEnemy.h"
#include "EffectManager.h"
namespace
{
    constexpr float kAttackRadius = 50.0f;
    constexpr float kColRadius = 50.0f;
    constexpr VECTOR kDefaultPos = { -5405.0f,741.0f,9199.0f };
    constexpr VECTOR kDefaultDir = { 0.0,270.0f,0.0f };
    constexpr VECTOR kDefaultTarfetPos = { 0.0,0.0f,0.0f };
    constexpr float kModelScale = 180.0f; // モデルのスケール
    constexpr float kChageTime = 1.5f;
    constexpr float kCoolDownTime = 1.5f;
    constexpr float kRangeAttackRadius = 250.0f;
    constexpr float kStrongAttackRadius = 150.0f;
    constexpr float kRangeAttackDuration = 60.0f;
    constexpr float kStrongAttackDuration = 100.0f;
    constexpr float kTrackingRange = 1000.0f;
    constexpr float kActionCheckInterval = 0.5f; // 抽選頻度
    constexpr float kNormalAttackDuration = 40.0f;
    constexpr float kNormalAttackRange = 90.0f;
    constexpr float kMoveSpeed = 5.0f;
    // 各アニメーション番号
    constexpr int kIdleAnimNo = 42;
    constexpr int kWalkAnimNo = 54;
    constexpr int kChargeAnimNo = 83;
    constexpr int kNormalAnimNo = 84;
    constexpr int kStrongAttackAnimNo = 78;
    constexpr int kRangeAttackAnimNo = 2;
    constexpr int kDamageAnimNo = 40;
    constexpr int kDeathAnimNo = 27;
    // アニメーション速度
    constexpr float kWalkAnimIncrement = 0.4f; // 歩行アニメーションの再生速度
    constexpr float kIdleAnimIncrement = 0.4f; // 待機アニメーションの再生速度
    constexpr float kAttackAnimIncrement = 0.5f; // 攻撃アニメーションの再生速度
    constexpr float kDamageAnimIncrement = 0.6f; // 被弾アニメーションの再生速度

    constexpr int kRandMax = 100;
    constexpr int kRangeAttackProbability = 30;
    constexpr float kFramesPerSecond = 60.0f; // 秒数変換
    constexpr unsigned int kAreaColor = 0xff4500;
    constexpr unsigned int kOutLineColor = 0xff0000;
    constexpr int kDivNum = 32;
    constexpr float kInvincibilityTime = 30.0f;
    constexpr int kMaxHp = 800;
    constexpr int kAttackPower = 75;
    constexpr int kStrongAttackPower = 80;
    constexpr int kRangeAttackPower = 95;
    constexpr float kAttenuationRate = 0.75f; // 被ダメージの減衰率
    constexpr float kCumulativeRate = 1.5f; // 被ダメージの累加率
}

BossEnemy::BossEnemy() :
    m_state(BossEnemyState::DEFAULT),
    m_enemyAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
    m_alpha(0.0f),
    m_targetAngle(0.0f),
    m_attackTimer(kCoolDownTime),
    m_actionCheckTimer(0.0f),
    m_storngAttackTargetPos({ 0.0f,0.0f,0.0f })
{
}

BossEnemy::~BossEnemy()
{
}

void BossEnemy::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<EffectManager> pEffectManager)
{
    Enemy::Init(pPlayer, pCompanion);
    m_pEffectManager = pEffectManager;
    m_pos = kDefaultPos;
    m_hp = kMaxHp;
    m_attackPower = kAttackPower;
    m_colRadius = kColRadius;
    m_modelHandle = MV1LoadModel(L"Data/model/Skeleton_Rogue.mv1");
    MV1SetScale(m_modelHandle, VGet(kModelScale, kModelScale, kModelScale));
    MV1SetRotationXYZ(m_modelHandle, kDefaultDir);
    MV1SetPosition(m_modelHandle,kDefaultPos);
    AttachAnim(m_modelHandle, kIdleAnimNo);
}

void BossEnemy::End()
{
    MV1DeleteModel(m_modelHandle);
    //m_pEffectManager->StopBossDeathEffect();
}

void BossEnemy::Update()
{
    if (m_isDead) return;
    if (m_hp <= 0 && m_state != BossEnemyState::DEAD)
    {
        m_state = BossEnemyState::DEAD;
        m_enemyAttack.active = false;
        ChangeAnim(m_modelHandle, kDeathAnimNo, false, 0.4f);
    }
    if (m_state == BossEnemyState::DEAD)
    {
        m_vec = { 0.0f, 0.0f, 0.0f }; // 移動停止
        UpdateAnim(m_modelHandle);
        MV1SetPosition(m_modelHandle, m_pos);
        
        if (GetIsAnimEnd())
        {
            m_isDead = true;
            End(); // モデルを削除
        }
        return; // 死んでいる時は、これ以降の移動や回転ロジックを通さない
    }
    SearchTarget();
    // 1 プレイヤーとの距離と方向を計算
    m_toPlayerDistance = VSize(VSub(m_targetPos, m_pos));
    m_toPlayerDir = VNorm(VSub(m_targetPos, m_pos));
    m_targetAngle = atan2f(m_toPlayerDir.x, m_toPlayerDir.z);

    // 2 無敵時間とダメージ処理
    if (m_invincibilityTimer > 0.0f)
    {
        m_invincibilityTimer--;
        
        if (m_invincibilityTimer <= 0.0f)
        {
            m_isHitFlag = false;
        }
    }

    // 3 状態遷移
    switch (m_state)
    {
    case BossEnemyState::DEFAULT:
        UpdateDefault(); // 次の行動を決める
        break;

    case BossEnemyState::MOVE:
        UpdateMove();    // プレイヤーに近づく
        break;

    case BossEnemyState::NORMAL_ATTACK:
        // 通常攻撃（素早い攻撃）
        m_enemyAttack.timer--;
        ChangeAnim(m_modelHandle, kNormalAnimNo, false, kAttackAnimIncrement);
        if (m_enemyAttack.timer <= 0)
        {
            m_state = BossEnemyState::COOLDOWN;
            m_enemyAttack.active = false;
            m_attackTimer = kCoolDownTime;
        }
        break;

    case BossEnemyState::STRONG_ATTACK_CHARGE: // 強攻撃の溜め（予兆）
        m_attackTimer -= 1.0f / kFramesPerSecond;
        if (m_attackTimer > 0.2f)
        {
            m_storngAttackTargetPos = m_targetPos;
        }
        else
        {
            VECTOR dir = VNorm(VSub(m_storngAttackTargetPos, m_pos)); // 攻撃位置を固定したらその方向を向き続ける
            m_targetAngle = atan2f(dir.x, dir.z);
        }
        ChangeAnim(m_modelHandle, kChargeAnimNo, false, 0.2f); // ゆっくり溜めるアニメ
        if (m_attackTimer <= 0)
        {
            OnStrongAttack(); // 強攻撃実行
            m_state = BossEnemyState::STRONG_ATTACK;
        }
        break;

    case BossEnemyState::RANGE_ATTACK_CHARGE: // 範囲攻撃の溜め（予兆）
        m_attackTimer -= 1.0f / kFramesPerSecond;
        ChangeAnim(m_modelHandle, kChargeAnimNo, false, 0.2f);
        if (m_attackTimer <= 0)
        {
            OnRangeAttack(); // 範囲攻撃実行
            m_state = BossEnemyState::RANGE_ATTACK;
        }
        break;

    case BossEnemyState::STRONG_ATTACK:
    {
        // 攻撃演出中
        m_enemyAttack.timer--;
        VECTOR dir = VNorm(VSub(m_storngAttackTargetPos, m_pos)); // 攻撃中も攻撃位置の方向を向き続ける
        m_targetAngle = atan2f(dir.x, dir.z);
        ChangeAnim(m_modelHandle, kStrongAttackAnimNo, false, 1.0f);
        if (m_enemyAttack.timer <= 0)
        {
            m_enemyAttack.active = false;
            m_storngAttackTargetPos = kDefaultTarfetPos;
            m_state = BossEnemyState::COOLDOWN;
            m_attackTimer = kCoolDownTime; // HP半分以下で隙短縮
        }
        break;
    }
    case BossEnemyState::RANGE_ATTACK:
        // 攻撃演出中
        m_enemyAttack.timer--;
        ChangeAnim(m_modelHandle, kRangeAttackAnimNo, false, kAttackAnimIncrement);
        if (m_enemyAttack.timer <= 0)
        {
            m_enemyAttack.active = false;
            m_state = BossEnemyState::COOLDOWN;
            m_attackTimer = kCoolDownTime; // HP半分以下で隙短縮
        }
        break;

    case BossEnemyState::COOLDOWN:
        // 攻撃後の隙
        m_attackTimer -= 1.0f / kFramesPerSecond;
        ChangeAnim(m_modelHandle, kIdleAnimNo, true, kIdleAnimIncrement);
        if (m_attackTimer <= 0)
        {
            m_state = BossEnemyState::DEFAULT;
        }
        break;
    }

    UpdateAnim(m_modelHandle);
    MV1SetPosition(m_modelHandle, m_pos);
    MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_targetAngle + DX_PI_F, 0.0f));
}

void BossEnemy::Draw()
{
    MV1DrawModel(m_modelHandle);
    if (m_state == BossEnemyState::RANGE_ATTACK_CHARGE)
    {
        // progressを 0.0f ～ 1.0f の間にする
        float progress = 1.0f - (m_attackTimer / kChageTime);
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        float currentRadius = kRangeAttackRadius * progress;

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
        VECTOR drawPos = VAdd(m_pos, VGet(0.0f, 1.0f, 0.0f));
        // 予兆の描画
        DrawCone3D(VAdd(drawPos, VGet(0.0f, 0.1f, 0.0f)), drawPos, currentRadius, kDivNum, kAreaColor, kAreaColor, true);

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
        DrawCone3D(VAdd(drawPos, VGet(0.0f, 0.2f, 0.0f)), drawPos, kRangeAttackRadius, kDivNum, kOutLineColor, kOutLineColor, true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    if (m_state == BossEnemyState::STRONG_ATTACK_CHARGE)
    {
        // progressを 0.0f ～ 1.0f の間にする
        float progress = 1.0f - (m_attackTimer / kChageTime);
        if (progress < 0.0f) progress = 0.0f;
        if (progress > 1.0f) progress = 1.0f;

        float currentRadius = kStrongAttackRadius * progress;

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
        VECTOR drawPos = VAdd(m_storngAttackTargetPos, VGet(0.0f, 1.0f, 0.0f));
        // 予兆の描画
        DrawCone3D(VAdd(drawPos, VGet(0.0f, 0.1f, 0.0f)), drawPos, currentRadius, kDivNum, kAreaColor, kAreaColor, true);

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
        DrawCone3D(VAdd(drawPos, VGet(0.0f, 0.2f, 0.0f)), drawPos, kStrongAttackRadius, kDivNum, kOutLineColor, kOutLineColor, true);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
    if (m_enemyAttack.active)
    {
        //DrawSphere3D(m_enemyAttack.pos, m_enemyAttack.radius, kDivNum, 0xffff00, 0xffffff, false);
    }
}

void BossEnemy::OnAttack()
{
    m_enemyAttack.timer = kNormalAttackDuration;
    m_enemyAttack.active = true;
    m_attackPower = kAttackPower;
    m_enemyAttack.dir = VNorm(VGet(sinf(m_targetAngle), 0.0f, cosf(m_targetAngle)));
    m_enemyAttack.pos = VAdd(m_pos, VScale(m_enemyAttack.dir, kNormalAttackRange));
    m_enemyAttack.radius = kAttackRadius;
}

void BossEnemy::OnStrongAttack()
{
    m_enemyAttack.pos = m_storngAttackTargetPos;
    m_enemyAttack.active = true;
    m_attackPower = kStrongAttackPower;
    m_enemyAttack.dir = VNorm(VSub(m_pPlayer->GetPos(), m_pos));
    m_enemyAttack.timer = kStrongAttackDuration;
    m_enemyAttack.radius = kStrongAttackRadius;
    m_pEffectManager->EnemyStrongAttackEffect(m_enemyAttack.pos);
}

void BossEnemy::OnRangeAttack()
{
    m_enemyAttack.pos = m_pos;
    m_enemyAttack.active = true;
    m_attackPower = kRangeAttackPower;
    m_enemyAttack.dir = VNorm(VSub(m_pPlayer->GetPos(), m_pos));
    m_enemyAttack.timer = kRangeAttackDuration;
    m_enemyAttack.radius = kRangeAttackRadius;
    m_pEffectManager->BossEnemyRangeAttackEffect(m_enemyAttack.pos);
}

void BossEnemy::OnDamage(int damage, bool isHatePlayer)
{
    if (m_invincibilityTimer > 0.0f || m_state == BossEnemyState::DEAD) return;
    m_isHitFlag = true;
    m_hp -= damage;
    m_invincibilityTimer = kInvincibilityTime;
    if (isHatePlayer)
    {
        m_playerHate += (float)damage;
    }
    else
    {
        m_companionHate += (float)damage * 3;
    }
    //if (m_hp <= 0)
    //{
    //    m_enemyAttack.active = false;
    //    m_hp = 0;
    //    m_state = BossEnemyState::DEAD;
    //    ChangeAnim(m_modelHandle,kDeathAnimNo,false,0.4f);
    //    //m_pEffectManager->PlayBossDeathEffect(m_pos);
    //}
    if (m_hp < 0) m_hp = 0;
}

float BossEnemy::GetColRadius()
{
    return kColRadius;
}

void BossEnemy::UpdateDefault()
{
    // 一定間隔で次の行動を抽選
    m_actionCheckTimer -= 1.0f / kFramesPerSecond;
    if (m_actionCheckTimer > 0) return;
    m_actionCheckTimer = kActionCheckInterval;

    if (m_toPlayerDistance < kNormalAttackRange)
    {
        OnAttack();
        m_state = BossEnemyState::NORMAL_ATTACK;
    }
    else if (m_toPlayerDistance < kRangeAttackRadius)
    {
        int rand = GetRand(100);
        if (rand < 50)
        {
            m_state = BossEnemyState::STRONG_ATTACK_CHARGE;
            m_attackTimer = kChageTime; // 基準の溜め時間
            m_pEffectManager->EnemyStrongAttackChargeEffect(m_pos);
            //m_enemyAttack.active = true; // 描画を開始するためにここでONにする
        }
        else if (rand < 70)
        {
            m_state = BossEnemyState::RANGE_ATTACK_CHARGE;
            // 溜め時間を定数と一致させる
            m_attackTimer = kChageTime;
            //m_enemyAttack.active = true; // ONにする
        }
        else
        {
            m_state = BossEnemyState::MOVE;
        }
    }
    else if (m_toPlayerDistance < kTrackingRange)
    {
        // 遠距離：追いかける
        m_state = BossEnemyState::MOVE;
    }
}

void BossEnemy::UpdateMove()
{
    // プレイヤーの方へ移動
    m_pos = VAdd(m_pos, VScale(m_toPlayerDir, kMoveSpeed));
    ChangeAnim(m_modelHandle, kWalkAnimNo, true, kWalkAnimIncrement);

    // 攻撃圏内に入ったら思考ルーチンに戻す
    if (m_toPlayerDistance < kRangeAttackRadius * 0.8f) 
    {
        m_state = BossEnemyState::DEFAULT;
    }
}