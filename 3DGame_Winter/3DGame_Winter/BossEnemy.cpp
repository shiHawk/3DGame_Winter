#include "BossEnemy.h"
namespace
{
    constexpr float kAttackRadius = 30.0f;
    constexpr float kColRadius = 50.0f;
    constexpr VECTOR kDefaultPos = { 500.0f,-59.0f,500.0f };
    constexpr float kModelScale = 70.0f; // モデルのスケール
    constexpr float kChageTime = 1.5f;
    constexpr float kCoolDownTime = 1.5f;
    constexpr float kRangeAttackRadius = 150.0f;
    constexpr float kRangeAttackDuration = 30.0f;
    constexpr float kTrackingRange = 400.0f;
    constexpr float kActionCheckInterval = 0.5f; // 抽選頻度
    constexpr float kNormalAttackDuration = 20.0f;
    constexpr float kNormalAttackRange = 90.0f;
    constexpr float kMoveSpeed = 5.0f;
    // 各アニメーション番号
    constexpr int kIdleAnimNo = 9;
    constexpr int kWalkAnimNo = 15;
    constexpr int kAttackAnimNo = 13;
    constexpr int kRangeAttackAnimNo = 0;
    constexpr int kDamageAnimNo = 8;
    // アニメーション速度
    constexpr float kWalkAnimIncrement = 0.15f; // 歩行アニメーションの再生速度
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
    constexpr int kMaxHp = 1000;
}

BossEnemy::BossEnemy() :
    m_state(BossEnemyState::DEFAULT),
    m_enemyAttack(kAttackRadius, { 0.0f,0.0f,0.0f }, false, 0.0f, { 0.0f,0.0f,0.0f }),
    m_alpha(0.0f),
    m_targetAngle(0.0f),
    m_attackTimer(0.0f),
    m_actionCheckTimer(0.0f)
{
}

BossEnemy::~BossEnemy()
{
}

void BossEnemy::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion)
{
    Enemy::Init(pPlayer, pCompanion);
    m_pos = kDefaultPos;
    m_hp = kMaxHp;
}

void BossEnemy::End()
{
}

void BossEnemy::Update()
{
    // 1. プレイヤーとの距離と方向を計算
    m_toPlayerDistance = VSize(VSub(m_pPlayer->GetPos(), m_pos));
    m_toPlayerDir = VNorm(VSub(m_pPlayer->GetPos(), m_pos));
    m_targetAngle = atan2f(m_toPlayerDir.x, m_toPlayerDir.z);

    // 2. 無敵時間とダメージ処理
    if (m_invincibilityTimer > 0.0f) {
        m_invincibilityTimer--;
        if (m_invincibilityTimer <= 0.0f) m_isHitFlag = false;
    }

    // 3. 状態遷移
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
        ChangeAnim(m_modelHandle, kAttackAnimNo, false, kAttackAnimIncrement);
        if (m_enemyAttack.timer <= 0)
        {
            m_state = BossEnemyState::COOLDOWN;
            m_attackTimer = kCoolDownTime;
        }
        break;

    case BossEnemyState::STRONG_ATTACK_CHARGE: // 強攻撃の溜め（予兆）
        m_attackTimer -= 1.0f / kFramesPerSecond;
        ChangeAnim(m_modelHandle, kAttackAnimNo, false, 0.2f); // ゆっくり溜めるアニメ
        if (m_attackTimer <= 0)
        {
            OnStrongAttack(); // 強攻撃実行
            m_state = BossEnemyState::STRONG_ATTACK;
        }
        break;

    case BossEnemyState::RANGE_ATTACK_CHARGE: // 範囲攻撃の溜め（予兆）
        m_attackTimer -= 1.0f / kFramesPerSecond;
        ChangeAnim(m_modelHandle, kRangeAttackAnimNo, false, 0.2f);
        if (m_attackTimer <= 0)
        {
            OnRangeAttack(); // 範囲攻撃実行
            m_state = BossEnemyState::RANGE_ATTACK;
        }
        break;

    case BossEnemyState::STRONG_ATTACK:
    case BossEnemyState::RANGE_ATTACK:
        // 攻撃演出中
        m_enemyAttack.timer--;
        if (m_enemyAttack.timer <= 0)
        {
            m_state = BossEnemyState::COOLDOWN;
            m_attackTimer = kCoolDownTime * (m_hp < kMaxHp * 0.5f ? 0.5f : 1.0f); // HP半分以下で隙短縮
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

    // 4. 共通処理（死亡チェック、アニメーション更新、座標反映）
    if (m_isDead)
    {
        m_enemyAttack.active = false;
    }
    UpdateAnim();
    MV1SetPosition(m_modelHandle, m_pos);
    MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, m_targetAngle + DX_PI_F, 0.0f));
}

void BossEnemy::Draw()
{
    DrawSphere3D(m_pos, kColRadius, kDivNum, 0xff0000, 0xffffff, true);
}

void BossEnemy::OnAttack()
{
    m_enemyAttack.timer = kNormalAttackDuration;
    m_enemyAttack.active = true;
    m_enemyAttack.dir = VNorm(VGet(sinf(m_targetAngle), 0.0f, cosf(m_targetAngle)));
    m_enemyAttack.pos = VAdd(m_pos, VScale(m_enemyAttack.dir, kNormalAttackRange));
}

void BossEnemy::OnStrongAttack()
{
}

void BossEnemy::OnRangeAttack()
{
}

void BossEnemy::OnDamage()
{
    if (m_invincibilityTimer > 0.0f) return;
    if (m_hp <= 0)
    {
        m_isDead = true;
    }
    m_isHitFlag = true;
    m_hp -= m_pPlayer->GetAttackPower();
    m_invincibilityTimer = kInvincibilityTime;
}

float BossEnemy::GetColRadius()
{
    return kColRadius;
}

VECTOR BossEnemy::GetDir()
{
    return VECTOR();
}

int BossEnemy::GetBossEnemyAttackPower()
{
    return m_attackPower;
}

void BossEnemy::UpdateDefault()
{
    // 一定間隔で次の行動を抽選
    m_actionCheckTimer -= 1.0f / kFramesPerSecond;
    if (m_actionCheckTimer > 0) return;
    m_actionCheckTimer = kActionCheckInterval;

    // --- 距離による分岐 ---
    if (m_toPlayerDistance < kNormalAttackRange)
    {
        // 至近距離：通常攻撃
        OnAttack();
        m_state = BossEnemyState::NORMAL_ATTACK;
    }
    else if (m_toPlayerDistance < kRangeAttackRadius)
    {
        // 中距離：強攻撃か範囲攻撃をランダムで
        int rand = GetRand(100);
        if (rand < 40)
        {
            m_state = BossEnemyState::STRONG_ATTACK_CHARGE;
            m_attackTimer = kChageTime;
        }
        else if (rand < 70)
        {
            m_state = BossEnemyState::RANGE_ATTACK_CHARGE;
            m_attackTimer = kChageTime * 1.5f; // 範囲攻撃は溜めを長く
        }
        else
        {
            m_state = BossEnemyState::MOVE; // 30%でさらに近づく
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
    if (m_toPlayerDistance < kRangeAttackRadius * 0.8f) {
        m_state = BossEnemyState::DEFAULT;
    }
}