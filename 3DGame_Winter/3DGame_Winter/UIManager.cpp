#include "UIManager.h"
#include "Pad.h"
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

	constexpr int kEnemyBarWidth = 100;
	constexpr int kEnemyBarHeight = 10;
	constexpr float kNormalEnemyOffsetY = 155.0f;
	constexpr float kStrongEnemyOffsetY = 250.0f;
	constexpr int kEnemyBarFrameSrcX = 1350;
	constexpr int kEnemyBarFrameSrcY = 142;
	constexpr int kEnemyBarSrcX = 1304;
	constexpr int kEnemyBarSrcY = 122;

	constexpr int kWarriorIconPosX = 70;
	constexpr int kWarriorIconPosY = 630;
	constexpr int kWizardIconPosX = 640;
	constexpr int kWizardIconPosY = 620;

	constexpr int kWarriorHpPosX = 151;
	constexpr int kWarriorHpPosY = 648;
	constexpr int kWizardHpPosX = 751;
	constexpr int kWizardHpPosY = 648;

	constexpr int kWarriorSgPosX = 151;
	constexpr int kWarriorSgPosY = 678;
	constexpr int kWizardSgPosX = 751;
	constexpr int kWizardSgPosY = 678;

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
	constexpr int kManualFontTextSize = 10;
	constexpr int kFontSize = 15;
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
	m_enemyHPHandle(-1),
	m_enemyHPFrameHandle(-1),
	m_warriorIconHandle(-1),
	m_wizardIconHandle(-1),
	m_fontHandle(-1),
	m_manualFontHandle(-1),
	m_aButtonHandle(-1),
	m_bButtonHandle(-1),
	m_xButtonHandle(-1),
	m_yButtonHandle(-1),
	m_rbButtonHandle(-1),
	m_lbButtonHandle(-1),
	m_stickHandle(-1),
	m_manualFrameHandle(-1),
	m_manualIconHandle(-1),
	m_isDisplayManual(true)
{
}

UIManager::~UIManager()
{
}

void UIManager::Init(std::shared_ptr<Player> pPlayer, std::shared_ptr<Companion> pCompanion, std::shared_ptr<BossEnemy> pBoss,
				     std::vector<std::shared_ptr<NormalEnemy>> pNormalEnemies, std::vector<std::shared_ptr<StrongEnemy>> pStrongEnemies)
{
	m_pPlayer = pPlayer;
	m_pCompanion = pCompanion;
	m_pBossEnemy = pBoss;
	m_pNormalEnemies = pNormalEnemies;
	m_pStrongEnemies = pStrongEnemies;
	m_hpGaugeFrameHandle = LoadGraph(L"Data/UI/HPGaugeFrame.png");
	m_bossHpGaugeHandle = LoadGraph(L"Data/UI/Boss_Hp.png");
	m_hpGaugeHandle = LoadGraph(L"Data/UI/HP.png");
	m_sgGaugeHandle = LoadGraph(L"Data/UI/SG.png");
	m_warriorIconHandle = LoadGraph(L"Data/UI/Warrior_Icon.png");
	m_wizardIconHandle = LoadGraph(L"Data/UI/Wizard_Icon.png");
	m_bossHPGaugeFlameHandle = LoadGraph(L"Data/UI/BossHPFrame.png");
	m_enemyHPHandle = LoadGraph(L"Data/UI/EnemyHP.png");
	m_enemyHPFrameHandle = LoadGraph(L"Data/UI/EnemyHPFrame.png");
	m_aButtonHandle = LoadGraph(L"Data/UI/AButton.png");
	m_bButtonHandle = LoadGraph(L"Data/UI/BButton.png");
	m_xButtonHandle = LoadGraph(L"Data/UI/XButton.png");
	m_yButtonHandle = LoadGraph(L"Data/UI/YButton.png");
	m_rbButtonHandle = LoadGraph(L"Data/UI/RBButton.png");
	m_lbButtonHandle = LoadGraph(L"Data/UI/LBButton.png");
	m_manualIconHandle = LoadGraph(L"Data/UI/StartButton.png");
	m_stickHandle = LoadGraph(L"Data/UI/stick.png");
	m_manualFrameHandle = LoadGraph(L"Data/UI/frame.png");
	m_fontHandle = CreateFontToHandle(L"Arial Black", kFontSize, kFontThick, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	m_manualFontHandle = CreateFontToHandle(L"游明朝 Demibold", kFontSize, kFontThick, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
}

void UIManager::End()
{
	DeleteGraph(m_hpGaugeFrameHandle);
	DeleteGraph(m_hpGaugeHandle);
	DeleteGraph(m_sgGaugeHandle);
	DeleteGraph(m_bossHpGaugeHandle);
	DeleteGraph(m_warriorIconHandle);
	DeleteGraph(m_wizardIconHandle);
	DeleteGraph(m_bossHPGaugeFlameHandle);
	DeleteGraph(m_enemyHPHandle);
	DeleteGraph(m_enemyHPFrameHandle);
	DeleteGraph(m_aButtonHandle);
	DeleteGraph(m_bButtonHandle);
	DeleteGraph(m_xButtonHandle);
	DeleteGraph(m_yButtonHandle);
	DeleteGraph(m_rbButtonHandle);
	DeleteGraph(m_lbButtonHandle);
	DeleteGraph(m_stickHandle);
	DeleteGraph(m_manualFrameHandle);
	DeleteGraph(m_manualIconHandle);
	DeleteFontToHandle(m_fontHandle);
	DeleteFontToHandle(m_manualFontHandle);
}

void UIManager::Updata()
{
	// 残りHPの割合を更新
	m_playerHpGaugeRate = static_cast<float>(m_pPlayer->GetHp()) / m_pPlayer->GetMaxHp();
	m_companionHpGaugeRate = static_cast<float>(m_pCompanion->GetHp()) / m_pCompanion->GetMaxHp();
	m_bossHpGaugeRate = static_cast<float>(m_pBossEnemy->GetHp()) / 800;

	m_playerSpecialGaugeRate = static_cast<float>(m_pPlayer->GetSpecialGauge()) / 100.0f;
	m_companionSpecialGaugeRate = static_cast<float>(m_pCompanion->GetSpecialGauge()) / 100.0f;

	if (Pad::isTrigger(PAD_INPUT_8) && !m_isDisplayManual)
	{
		m_isDisplayManual = true;
	}
	else if (Pad::isTrigger(PAD_INPUT_8) && m_isDisplayManual)
	{
		m_isDisplayManual = false;
	}
}

void UIManager::Draw()
{
	DrawHp();
	DrawSg();
	DrawEnemyHP();
	if (m_isDisplayManual)
	{
		DrawManualUI();
	}
	if (VSize(VSub(m_pPlayer->GetPos(), m_pBossEnemy->GetPos())) <= 1000.0f)
	{
		DrawBossHp();
	}
	/*DINPUT_JOYSTATE input;
	int i;
	int Color;
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
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

void UIManager::DrawHp()
{
	// プレイヤーのHPバーの描画
	DrawRectGraph(kPlayerHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, kHpGaugeFrameWidth, kHpTextPosY, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kPlayerHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, static_cast<int>(kHpGaugeWidth * m_playerHpGaugeRate),
				  kHpTextPosY, m_hpGaugeHandle, true); 
	DrawGraph(kWarriorIconPosX,kWarriorIconPosY,m_warriorIconHandle,true);
	DrawFormatStringToHandle(kWarriorHpPosX, kWarriorHpPosY, 0x66cbaa, m_fontHandle, L"HP");

	// コンパニオンのHPバーの描画
	DrawRectGraph(kCompanionHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, kHpGaugeFrameWidth, kHpTextPosY, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kCompanionHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, static_cast<int>(kHpGaugeWidth * m_companionHpGaugeRate),
				  kHpTextPosY, m_hpGaugeHandle, true);
	DrawGraph(kWizardIconPosX, kWizardIconPosY, m_wizardIconHandle, true);
	DrawFormatStringToHandle(kWizardHpPosX, kWizardHpPosY, 0x66cbaa, m_fontHandle, L"HP");
}

void UIManager::DrawSg()
{
	// プレイヤーのSpecialゲージの描画
	DrawRectGraph(kPlayerSpecialGaugeLeft, kSpecialGaugeTop, kSrcX, kSrcY, kSpecialGaugeWidth, kHpTextPosY, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kPlayerSpecialGaugeLeft, kSpecialGaugeTop, kSgSrcX, kSgSrcY, static_cast<int>(kHpGaugeWidth * m_playerSpecialGaugeRate),
				  kHpTextPosY, m_sgGaugeHandle, true);
	DrawFormatStringToHandle(kWarriorSgPosX, kWarriorSgPosY, 0xffb770, m_fontHandle, L"SG");

	// コンパニオンのSpecialゲージの描画
	DrawRectGraph(kCompanionSpecialGaugeLeft, kSpecialGaugeTop, kSrcX, kSrcY, kSpecialGaugeWidth, kHpTextPosY, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kCompanionSpecialGaugeLeft, kSpecialGaugeTop, kSgSrcX, kSgSrcY, static_cast<int>(kHpGaugeWidth * m_companionSpecialGaugeRate),
				  kHpTextPosY, m_sgGaugeHandle, true);
	DrawFormatStringToHandle(kWizardSgPosX, kWizardSgPosY, 0xffb770, m_fontHandle, L"SG");
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

void UIManager::DrawEnemyHP()
{
	//DrawBillboard3D(VGet(m_pPlayer->GetPos().x,m_pPlayer->GetPos().y+120.0f, m_pPlayer->GetPos().z), 0.5f, 0.5f, 130.0f, 0.0f, m_enemyHPHandle, true);
	// NormalEnemyのループ
	for (const auto& enemy : m_pNormalEnemies)
	{
		// 敵が生きていれば描画
		if (enemy->IsDead()) continue;
		// 一定距離以下になったら描画
		if (VSize(VSub(enemy->GetPos(), m_pPlayer->GetPos())) <= 800.0f || VSize(VSub(enemy->GetPos(), m_pCompanion->GetPos())) <= 800.0f)
		{
			DrawSingleEnemyBar(enemy->GetPos(), enemy->GetHp(), enemy->GetMaxHp(),
				kEnemyBarWidth, kEnemyBarHeight, kNormalEnemyOffsetY);
		}
	}

	// StrongEnemyのループ
	for (const auto& enemy : m_pStrongEnemies)
	{
		if (enemy->IsDead()) continue;
		if (VSize(VSub(enemy->GetPos(), m_pPlayer->GetPos())) <= 800.0f || VSize(VSub(enemy->GetPos(), m_pCompanion->GetPos())) <= 800.0f)
		{
			DrawSingleEnemyBar(enemy->GetPos(), enemy->GetHp(), enemy->GetMaxHp(),
				kEnemyBarWidth, kEnemyBarHeight, kStrongEnemyOffsetY);
		}
	}
}

void UIManager::DrawSingleEnemyBar(VECTOR pos, int hp, int maxHp, float width, float height, float offsetY)
{
	// 1. 敵の頭上の3D座標を計算
	VECTOR headPos = VGet(pos.x, pos.y + offsetY, pos.z);

	// 2. 3D座標をスクリーン（2D）座標に変換
	VECTOR screenPos = ConvWorldPosToScreenPos(headPos);

	// 3. 画面外（カメラの後ろ）なら処理しない
	if (screenPos.z < 0.0f || screenPos.z > 1.0f) return;

	// 割合計算
	float rate = static_cast<float>(hp) / maxHp;
	if (rate < 0) rate = 0;

	// 描画開始位置（バーの中央が敵の真上にくるように調整）
	int x1 = static_cast<int>(screenPos.x - width / 2);
	int y1 = static_cast<int>(screenPos.y);
	int x2 = x1 + width;
	int y2 = y1 + height;

	// 4. 背景（枠）の描画
	// 元画像(kSrcWidth)を画面上のサイズ(width)に引き伸ばして描画
	DrawRectExtendGraph(x1, y1, x2, y2, 0, 0, kEnemyBarFrameSrcX, kEnemyBarFrameSrcY, m_enemyHPFrameHandle, TRUE);

	// 5. HPバー（中身）の描画
	// 現在のHPに合わせて「右端(x2)」と「切り出し範囲」を制限する
	int currentX2 = x1 + static_cast<int>(width * rate);
	int currentSrcWidth = static_cast<int>(kEnemyBarSrcX * rate);

	if (currentSrcWidth > 0)
	{
		DrawRectExtendGraph(x1, y1, currentX2, y2, 0, 0, currentSrcWidth, kEnemyBarSrcY, m_enemyHPHandle, TRUE);
	}
}

void UIManager::DrawManualUI()
{
	DrawRectExtendGraph(1000,13,1280,460,703,114,546,778,m_manualFrameHandle,true);
	DrawGraph(1050,20,m_aButtonHandle,true);
	DrawFormatStringToHandle(1100, 30, 0xffffff, m_manualFontHandle, L"ジャンプ");
	DrawGraph(1050,70,m_bButtonHandle,true);
	DrawFormatStringToHandle(1100, 80, 0xffffff, m_manualFontHandle, L"回避");
	DrawGraph(1050,120,m_xButtonHandle,true);
	DrawFormatStringToHandle(1100, 130, 0xffffff, m_manualFontHandle, L"弱攻撃");
	DrawGraph(1050,170,m_yButtonHandle,true);
	DrawFormatStringToHandle(1100, 180, 0xffffff, m_manualFontHandle, L"強攻撃");

	DrawGraph(1015, 221, m_xButtonHandle, true);
	DrawGraph(1048, 220, m_yButtonHandle, true);
	DrawGraph(1080, 220, m_yButtonHandle, true);
	DrawFormatStringToHandle(1120, 230, 0xffffff, m_manualFontHandle, L"フィニッシュ");

	DrawRectExtendGraph(1050, 270,1090,300, 766, 429, 379, 196, m_rbButtonHandle, true);
	DrawFormatStringToHandle(1100, 275, 0xffffff, m_manualFontHandle, L"キャラチェンジ");

	DrawRectExtendGraph(1050, 320,1090,350, 766, 429, 379, 196, m_lbButtonHandle, true);
	DrawFormatStringToHandle(1100, 325, 0xffffff, m_manualFontHandle, L"必殺技");

	DrawRectExtendGraph(1040, 370,1150,410, 820, 454, 379, 200, m_stickHandle, true);
	DrawFormatStringToHandle(1100, 365, 0xffffff, m_manualFontHandle, L"ロックオン");
	DrawFormatStringToHandle(1100, 395, 0xffffff, m_manualFontHandle, L"ロックオン解除");

	DrawFormatStringToHandle(1050, 428, 0xff4500, m_manualFontHandle, L"START　 閉じる");
	/*DrawRectGraph(1050,220,766,429,379,196,m_rbButtonHandle,true);
	DrawRectGraph(1050,270,766,429,379,196,m_lbButtonHandle,true);*/
}
