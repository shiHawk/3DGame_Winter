#include "UIManager.h"
namespace
{
	// HPゲージ
	constexpr int kHpGaugeWidth = 370;
	constexpr int kPlayerHpGaugeLeft = 150;
	constexpr int kCompanionHpGaugeLeft = 750;
	constexpr int kHpGaugeTop = 650;
	// Specialゲージ
	constexpr int kSpecialGaugeWidth = 370;
	constexpr int kPlayerSpecialGaugeLeft = 150;
	constexpr int kCompanionSpecialGaugeLeft = 750;
	constexpr int kSpecialGaugeTop = 680;

	constexpr int kHpTextPosY = 14;
	constexpr int kSrcX = 462; // 切り出し位置(X)
	constexpr int kSrcY = 293; // 切り出し位置(Y)
	constexpr int kSrcWidth = 150;
	constexpr int kSgSrcX = 333; // Secialゲージ切り出し位置(X)
	constexpr int kSgSrcY = 207; // Secialゲージ切り出し位置(Y)
	constexpr int kDistX1 = 435; // 修正：バーの開始位置をさらに右へ（345 + 50）
	constexpr int kDistY1 = 575;
	constexpr int kDistX2 = 1006; // 修正：フレームの終端を右へ（980 + 26）
	constexpr int kDistY2 = 605;
	constexpr int kWarriorIconPosX = 70;
	constexpr int kWarriorIconPosY = 630;
	constexpr int kHpGaugeFrameWidth = 370; // ゲージの横幅
	// スコアの位置
	constexpr int kScorePosX = 650;
	constexpr int kCharaTextPosY = 20;
	constexpr int kCharaPosY = 45;
	constexpr unsigned int kScoreColor = 0xffdead;
	// 経過時間の位置
	constexpr int kTimePosX = 830;
	constexpr unsigned int kTimeColor = 0xff4500;
	// フォントのサイズ、太さ
	constexpr int kFontTextSize = 20;
	constexpr int kFontSize = 30;
	constexpr int kFontThick = 5;
}

UIManager::UIManager():
	m_playerHpGaugeRate(0.0f),
	m_companionHpGaugeRate(0.0f),
	m_bossHpGaugeRate(0.0f),
	m_playerSpecialGaugeRate(0.0f),
	m_companionSpecialGaugeRate(0.0f),
	m_hpGaugeFrameHandle(-1),
	m_hpGaugeHandle(-1),
	m_sgGaugeHandle(-1),
	m_bossHpGaugeHandle(-1),
	m_bossHPGaugeFlameHandle(-1),
	m_warriorIconHandle(-1),
	m_wizardIconHandle(-1)
{
}

UIManager::~UIManager()
{
}

void UIManager::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<BossEnemy> pBoss)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	m_pBossEnemy = pBoss;
	m_hpGaugeFrameHandle = LoadGraph(L"Data/UI/HPGaugeFrame.png");
	m_bossHpGaugeHandle = LoadGraph(L"Data/UI/Boss_Hp.png");
	m_hpGaugeHandle = LoadGraph(L"Data/UI/HP.png");
	m_sgGaugeHandle = LoadGraph(L"Data/UI/SG.png");
	m_warriorIconHandle = LoadGraph(L"Data/UI/Player_Icon.png");
	m_bossHPGaugeFlameHandle = LoadGraph(L"Data/UI/BossHPFrame.png");
}

void UIManager::End()
{
	DeleteGraph(m_hpGaugeFrameHandle);
	DeleteGraph(m_hpGaugeHandle);
	DeleteGraph(m_sgGaugeHandle);
	DeleteGraph(m_bossHpGaugeHandle);
	DeleteGraph(m_warriorIconHandle);
	DeleteGraph(m_bossHPGaugeFlameHandle);
}

void UIManager::Updata()
{
	// 残りHPの割合を更新
	m_playerHpGaugeRate = static_cast<float>(m_pPlayer->GetHp()) / m_pPlayer->GetMaxHp();
	m_companionHpGaugeRate = static_cast<float>(m_pCompanion->GetHp()) / m_pCompanion->GetMaxHp();
	m_bossHpGaugeRate = static_cast<float>(m_pBossEnemy->GetHp()) / 500;

	m_playerSpecialGaugeRate = static_cast<float>(m_pPlayer->GetSpecialGauge()) / 100.0f;
	m_companionSpecialGaugeRate = static_cast<float>(m_pCompanion->GetSpecialGauge()) / 100.0f;
}

void UIManager::Draw()
{
	DrawHp();
	DrawSg();
	if (VSize(VSub(m_pPlayer->GetPos(), m_pBossEnemy->GetPos())) <= 1000.0f)
	{
		DrawBossHp();
	}
}

void UIManager::DrawHp()
{
	// プレイヤーのHPバーの描画
	DrawRectGraph(kPlayerHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, kHpGaugeFrameWidth, kHpTextPosY, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kPlayerHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, static_cast<int>(kHpGaugeWidth * m_playerHpGaugeRate),
				  kHpTextPosY, m_hpGaugeHandle, true); 
	DrawGraph(kWarriorIconPosX,kWarriorIconPosY,m_warriorIconHandle,true);

	// コンパニオンのHPバーの描画
	DrawRectGraph(kCompanionHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, kHpGaugeFrameWidth, kHpTextPosY, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kCompanionHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, static_cast<int>(kHpGaugeWidth * m_companionHpGaugeRate),
				  kHpTextPosY, m_hpGaugeHandle, true);
}

void UIManager::DrawSg()
{
	// プレイヤーのSpecialゲージの描画
	DrawRectGraph(kPlayerSpecialGaugeLeft, kSpecialGaugeTop, kSrcX, kSrcY, kSpecialGaugeWidth, kHpTextPosY, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kPlayerSpecialGaugeLeft, kSpecialGaugeTop, kSgSrcX, kSgSrcY, static_cast<int>(kHpGaugeWidth * m_playerSpecialGaugeRate),
				  kHpTextPosY, m_sgGaugeHandle, true);

	// コンパニオンのSpecialゲージの描画
	DrawRectGraph(kCompanionSpecialGaugeLeft, kSpecialGaugeTop, kSrcX, kSrcY, kSpecialGaugeWidth, kHpTextPosY, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kCompanionSpecialGaugeLeft, kSpecialGaugeTop, kSgSrcX, kSgSrcY, static_cast<int>(kHpGaugeWidth * m_companionSpecialGaugeRate),
				  kHpTextPosY, m_sgGaugeHandle, true);
}

void UIManager::DrawBossHp()
{
	// バーの最大幅を少し狭くして、左右の装飾にぶつからないように調整
	int maxDestWidth = 400; 
	int currentDestWidth = static_cast<int>(maxDestWidth * m_bossHpGaugeRate);

	// 1. ボスHPフレーム（背景・装飾）を重ねて描画
	DrawRectExtendGraph(274, 550, kDistX2, 647, 63, 153, 636, 187, m_bossHPGaugeFlameHandle, true);
	// 2. HPバーを描画（フレームの後ろ側になるように）
	DrawRectExtendGraph(kDistX1, kDistY1, kDistX1 + currentDestWidth, kDistY2,
		kSrcX, kSrcY, static_cast<int>(kHpGaugeWidth * m_bossHpGaugeRate),
		kSrcWidth, m_bossHpGaugeHandle, true);
	
}
