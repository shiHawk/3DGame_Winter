#include "UIManager.h"
namespace
{
	// HPゲージ
	constexpr int kHpGaugeWidth = 370;
	constexpr int kPlayerHpGaugeLeft = 150;
	constexpr int kCompanionHpGaugeLeft = 750;
	constexpr int kHpGaugeTop = 650;
	constexpr int kHpTextPosY = 14;
	constexpr int kSrcX = 462; // 切り出し位置(X)
	constexpr int kSrcY = 293; // 切り出し位置(Y)
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
	m_hpGaugeFrameHandle(-1),
	m_hpGaugeHandle(-1)
{
}

UIManager::~UIManager()
{
}

void UIManager::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	m_hpGaugeFrameHandle = LoadGraph(L"Data/UI/HPGaugeFrame.png");
	m_hpGaugeHandle = LoadGraph(L"Data/UI/HP.png");
}

void UIManager::End()
{
	DeleteGraph(m_hpGaugeFrameHandle);
	DeleteGraph(m_hpGaugeHandle);
}

void UIManager::Updata()
{
	// 残りHPの割合を更新
	m_playerHpGaugeRate = static_cast<float>(m_pPlayer->GetHp()) / m_pPlayer->GetMaxHp();
	m_companionHpGaugeRate = static_cast<float>(m_pCompanion->GetHp()) / m_pCompanion->GetMaxHp(); 
}

void UIManager::Draw()
{
	DrawHp();
}

void UIManager::DrawHp()
{
	// プレイヤーのHPバーの描画
	DrawRectGraph(kPlayerHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, kHpGaugeFrameWidth, kHpTextPosY, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kPlayerHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, static_cast<int>(kHpGaugeWidth * m_playerHpGaugeRate),
				  kHpTextPosY, m_hpGaugeHandle, true); 

	// コンパニオンのHPバーの描画
	DrawRectGraph(kCompanionHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, kHpGaugeFrameWidth, kHpTextPosY, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kCompanionHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, static_cast<int>(kHpGaugeWidth * m_playerHpGaugeRate),
		kHpTextPosY, m_hpGaugeHandle, true);
}

void UIManager::DrawSg()
{
}
