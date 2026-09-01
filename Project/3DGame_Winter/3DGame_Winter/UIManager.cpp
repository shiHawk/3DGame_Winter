#include "UIManager.h"
#include "Pad.h"
namespace
{
	// HPゲージ
	constexpr int kHpGaugeWidth = 370;
	constexpr int kPlayerHpGaugeLeft = 150;
	constexpr int kCompanionHpGaugeLeft = 750;
	constexpr int kHpGaugeTop = 640;
	// Specialゲージ
	constexpr int kSpecialGaugeWidth = 370;
	constexpr int kPlayerSpecialGaugeLeft = 150;
	constexpr int kCompanionSpecialGaugeLeft = 750;
	constexpr int kSpecialGaugeTop = 680;

	constexpr int kBarHeight = 25;
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
	constexpr int kWeakIconOffset = 1;
	constexpr float kWeakPointIconScale = 5.0f;

	constexpr int kWarriorIconPosX = 70;
	constexpr int kWarriorIconPosY = 630;
	constexpr int kWizardIconPosX = 640;
	constexpr int kWizardIconPosY = 620;

	constexpr int kWarriorHpPosX = 151;
	constexpr int kWarriorHpPosY = 639;
	constexpr int kWizardHpPosX = 751;
	constexpr int kWizardHpPosY = 639;

	constexpr int kWarriorSgPosX = 151;
	constexpr int kWarriorSgPosY = 678;
	constexpr int kWizardSgPosX = 751;
	constexpr int kWizardSgPosY = 678;

	// ボスHPゲージ
	constexpr int kMaxDestWidth = 400;
	constexpr int kBossMaxHp = 2000; // ボスの最大HP
	constexpr int kBossHpFrameX1 = 274;
	constexpr int kBossHpFrameY1 = 550;
	constexpr int kBossHpFrameX2 = 1006; // kDistX2と同じ
	constexpr int kBossHpFrameY2 = 647;
	constexpr int kBossHpSrcX = 63;
	constexpr int kBossHpSrcY = 153;
	constexpr int kBossHpSrcW = 636;
	constexpr int kBossHpSrcH = 187;
	constexpr int kBossHpBarMaxDestWidth = 400;
	constexpr float kBossDrawDistance = 1000.0f;

	// マニュアルUI
	constexpr int kManualItemStartY = 20; // 最初の項目のY座標
	constexpr int kManualItemOffsetV = 50; // 各項目の垂直間隔
	constexpr int kTextOffsetV = 10; // ボタン画像に対するテキストのY軸ズレ補正
	constexpr int kManualFrameX1 = 1000;
	constexpr int kManualFrameY1 = 13;
	constexpr int kManualFrameX2 = 1280;
	constexpr int kManualFrameY2 = 460;
	constexpr int kManualFrameSrcX = 703;
	constexpr int kManualFrameSrcY = 114;
	constexpr int kManualFrameSrcW = 546;
	constexpr int kManualFrameSrcH = 778;
	// コンボ・フィニッシュ関連
	constexpr int kFinishTextPosX = 1120;
	constexpr int kFinishTextPosY = 390;
	constexpr int kButtonPosY = 380;

	// 特殊ボタン（RB/LB/Stick）の描画サイズと切り出し
	constexpr int kSpecialButtonWidth = 40;  // 1090 - 1050
	constexpr int kSpecialButtonHeight = 30;
	constexpr int kSpecialButtonSrcX = 766;
	constexpr int kSpecialButtonSrcY = 429;
	constexpr int kSpecialButtonSrcW = 379;
	constexpr int kSpecialButtonSrcH = 196;
	// 特殊ボタン（RB/LB/Stick）の各Y座標
	constexpr int kRbButtonPosY = 270;
	constexpr int kLbButtonPosY = 320;
	constexpr int kStickPosY = 300;
	constexpr int kStickPosX = 1020;
	constexpr int kStickWidth = 110; // 1150 - 1040
	constexpr int kStickHeight = 40; // 410 - 370

	// スティック画像の切り出し
	constexpr int kStickSrcX = 820;
	constexpr int kStickSrcY = 454;
	constexpr int kStickSrcW = 379;
	constexpr int kStickSrcH = 200;

	// テキスト個別の微調整
	constexpr int kSpecialTextOffsetV = 5;       // RB/LB用
	constexpr int kStickText1PosY = 295;         // ロックオン
	constexpr int kStickText2PosY = 325;         // 解除
	constexpr int kCloseTextPosY = 428;          // 閉じる
	// 目的地アイコン
	constexpr VECTOR kDestinationPos = { -5405.0f,2741.0f,9199.0f };
	constexpr float kDestionScale = 3000.0f;

	// バフUIの位置
	constexpr int kWarriorBuffUIPosX = 280;
	constexpr int kWizardBuffUIPosX = 880;

	constexpr int kHpGaugeFrameWidth = 370; // ゲージの横幅
	// スコアの位置
	constexpr int kScorePosX = 650;
	constexpr int kCharaTextPosY = 20;
	constexpr int kCharaPosY = 45;
	constexpr unsigned int kScoreColor = 0xffdead;
	constexpr float kDisplayEnemyHp = 800.0f;
	constexpr float kDisplayBossEnemyHp = 1000.0f;
	constexpr int kBuffIconPosY = 580;
	constexpr float kDamagePosOffSetY = 200.0f;
	constexpr float kDisplayTime = 10.0f;
	constexpr float kFloatSpeed = 0.5f;
	constexpr int kUIPosX = 1050;
	constexpr int kUITextPosX = 1100;
	constexpr int kComboButtonX1 = 1015;
	constexpr int kComboButtonX2 = 1048;
	constexpr int kComboButtonX3 = 1080;
	// フォントのサイズ、太さ
	constexpr int kFontTextSize = 20;
	constexpr int kManualFontTextSize = 10;
	constexpr int kFontSize = 15;
	constexpr int kFontSmallSize = 10;
	constexpr int kDamageFontSize = 25;
	constexpr int kFontThick = 5;
	constexpr int kFontSmallThick = 10;
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
	m_damageFontHandle(-1),
	m_manualFontHandle(-1),
	m_smallManualFontHandle(-1),
	m_aButtonHandle(-1),
	m_bButtonHandle(-1),
	m_xButtonHandle(-1),
	m_yButtonHandle(-1),
	m_rbButtonHandle(-1),
	m_lbButtonHandle(-1),
	m_triggerButtonHandle(-1),
	m_stickHandle(-1),
	m_manualFrameHandle(-1),
	m_manualIconHandle(-1),
	m_buffIconHandle(-1),
	m_buffSwapIconHandle(-1),
	m_operationPlayerUI(-1),
	m_destinationIconHandle(-1),
	m_manualCloaseHandel(-1),
	m_isDisplayManual(true),
	m_isDisplayBossHp(false),
	m_isOperationWarrior(true),
	m_sgUpIconHandle(-1),
	m_weakIconAxeHandle(-1),
	m_weakIconstickHandle(-1),
	m_sgUpPopUp({ {0.0f,0.0f,0.0f}, 0.0f, 0.0f, false })
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
	m_triggerButtonHandle = LoadGraph(L"Data/UI/TriggeButton.png");
	m_manualIconHandle = LoadGraph(L"Data/UI/StartButton.png");
	m_stickHandle = LoadGraph(L"Data/UI/stick.png");
	m_buffIconHandle = LoadGraph(L"Data/UI/powerUpIcon.png");
	m_buffSwapIconHandle = LoadGraph(L"Data/UI/powerUpSwapIcon.png");
	m_operationPlayerUI = LoadGraph(L"Data/UI/OperationUI.png");
	m_manualFrameHandle = LoadGraph(L"Data/UI/frame.png");
	m_destinationIconHandle = LoadGraph(L"Data/UI/Destination.png");
	m_manualCloaseHandel = LoadGraph(L"Data/UI/Close.png");
	m_sgUpIconHandle = LoadGraph(L"Data/UI/SGUP.png");
	m_weakIconAxeHandle = LoadGraph(L"Data/UI/AxeIcon.png");
	m_weakIconstickHandle = LoadGraph(L"Data/UI/StickIcon.png");
	m_fontHandle = CreateFontToHandle(L"Arial Black", kFontSize, kFontThick, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	m_manualFontHandle = CreateFontToHandle(L"游明朝 Demibold", kFontSize, kFontThick, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	m_smallManualFontHandle = CreateFontToHandle(L"游明朝 Demibold", kFontSmallSize, kFontSmallThick, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	m_damageFontHandle = CreateFontToHandle(L"Arial Black", kDamageFontSize, kFontThick, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
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
	DeleteGraph(m_triggerButtonHandle);
	DeleteGraph(m_stickHandle);
	DeleteGraph(m_manualFrameHandle);
	DeleteGraph(m_manualIconHandle);
	DeleteGraph(m_buffIconHandle);
	DeleteGraph(m_buffSwapIconHandle);
	DeleteGraph(m_operationPlayerUI);
	DeleteGraph(m_destinationIconHandle);
	DeleteGraph(m_manualCloaseHandel);
	DeleteGraph(m_sgUpIconHandle);
	DeleteFontToHandle(m_fontHandle);
	DeleteFontToHandle(m_manualFontHandle);
	DeleteFontToHandle(m_smallManualFontHandle);
	DeleteFontToHandle(m_damageFontHandle);
	DeleteFontToHandle(m_weakIconAxeHandle);
	DeleteFontToHandle(m_weakIconstickHandle);
	m_isDisplayBossHp = false;
}

void UIManager::Updata()
{
	// 残りHPの割合を更新
	m_playerHpGaugeRate = static_cast<float>(m_pPlayer->GetHp()) / m_pPlayer->GetMaxHp();
	m_companionHpGaugeRate = static_cast<float>(m_pCompanion->GetHp()) / m_pCompanion->GetMaxHp();
	m_bossHpGaugeRate = static_cast<float>(m_pBossEnemy->GetHp()) / kBossMaxHp;

	m_playerSpecialGaugeRate = static_cast<float>(m_pPlayer->GetSpecialGauge()) / 100.0f;
	m_companionSpecialGaugeRate = static_cast<float>(m_pCompanion->GetSpecialGauge()) / 100.0f;

	for (auto it = m_damageTexts.begin(); it != m_damageTexts.end();)
	{
		// 表示時間を減らす
		it->timer -= 1.0f / kDisplayTime;
		// テキストを浮上させる
		it->worldPos.y += kFloatSpeed;
		// テキストを徐々に透明にする
		if (it->timer < 3.0f)
		{
			it->alpha = it->timer / 3.0f;
		}
		// 表示時間が終わったら削除
		if (it->timer <= 0)
		{
			it = m_damageTexts.erase(it);
		}
		else
		{
			it++;
		}
	}

	// SG UPアイコンの更新
	if (m_sgUpPopUp.isVisible)
	{
		m_sgUpPopUp.timer -= 1.0f / kDisplayTime;
		m_sgUpPopUp.worldPos.y += kFloatSpeed;

		if (m_sgUpPopUp.timer < 3.0f)
		{
			m_sgUpPopUp.alpha = m_sgUpPopUp.timer / 3.0f;
		}

		if (m_sgUpPopUp.timer <= 0)
		{
			m_sgUpPopUp.isVisible = false; // 時間切れで非表示にする
		}
	}

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
	if (VSize(VSub(m_pPlayer->GetPos(), m_pBossEnemy->GetPos())) <= kDisplayBossEnemyHp)
	{
		m_isDisplayBossHp = true;
	}
	if (m_isDisplayBossHp)
	{
		DrawBossHp();
	}
	else
	{
		DrawDestination();
	}
	DrawOperationPlayer();
	DrawBuffUI();
	
	for (const auto& info : m_damageTexts)
	{
		VECTOR screenPos = ConvWorldPosToScreenPos(info.worldPos);
		// 透明度の設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * info.alpha));
		// ダメージの描画
		// 見やすいように影の描画
		DrawFormatStringToHandle(static_cast<int>(screenPos.x + 2), static_cast<int>(screenPos.y + 2), 
								 0x000000, m_damageFontHandle, L"%d", info.damage);
		// 実際のダメージ表示
		DrawFormatStringToHandle(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y),
								 info.color, m_damageFontHandle, L"%d", info.damage);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	if (m_sgUpPopUp.isVisible)
	{
		VECTOR screenPos = ConvWorldPosToScreenPos(m_sgUpPopUp.worldPos); // スクリーン座標に変換
		if (screenPos.z >= 0.0f && screenPos.z <= 1.0f)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255 * m_sgUpPopUp.alpha)); // 透明度の適応
			// 画像のサイズに合わせてDrawRotaGraphを使う
			DrawRotaGraph(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y),
				1.0, 0.0, m_sgUpIconHandle, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
}

void UIManager::RegisterDamageUI(VECTOR pos, int damage, unsigned int color)
{
	// ダメージの情報
	DamageText newDamageText;
	newDamageText.damage = damage;
	newDamageText.worldPos = pos;
	newDamageText.worldPos.y = pos.y+ kDamagePosOffSetY;
	newDamageText.color = color;
	// 演出用のパラメータ
	newDamageText.timer = kDisplayTime;
	newDamageText.alpha = 1.0f;
	// リストに登録
	m_damageTexts.push_back(newDamageText);
}

void UIManager::RegisterSgUpUI(VECTOR pos)
{
	m_sgUpPopUp.worldPos = pos;
	m_sgUpPopUp.worldPos.y += kDamagePosOffSetY;
	m_sgUpPopUp.timer = kDisplayTime;
	m_sgUpPopUp.alpha = 1.0f;
	m_sgUpPopUp.isVisible = true;
}

void UIManager::DrawHp()
{
	// プレイヤーのHPバーの描画
	DrawRectGraph(kPlayerHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, kHpGaugeFrameWidth, kBarHeight, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kPlayerHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, static_cast<int>(kHpGaugeWidth * m_playerHpGaugeRate),
				  kBarHeight, m_hpGaugeHandle, true); 
	DrawGraph(kWarriorIconPosX,kWarriorIconPosY,m_warriorIconHandle,true);
	DrawFormatStringToHandle(kWarriorHpPosX, kWarriorHpPosY, 0x66cbaa, m_fontHandle, L"HP");

	// コンパニオンのHPバーの描画
	DrawRectGraph(kCompanionHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, kHpGaugeFrameWidth, kBarHeight, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kCompanionHpGaugeLeft, kHpGaugeTop, kSrcX, kSrcY, static_cast<int>(kHpGaugeWidth * m_companionHpGaugeRate),
				  kBarHeight, m_hpGaugeHandle, true);
	DrawGraph(kWizardIconPosX, kWizardIconPosY, m_wizardIconHandle, true);
	DrawFormatStringToHandle(kWizardHpPosX, kWizardHpPosY, 0x66cbaa, m_fontHandle, L"HP");
}

void UIManager::DrawSg()
{
	// プレイヤーのSpecialゲージの描画
	DrawRectGraph(kPlayerSpecialGaugeLeft, kSpecialGaugeTop, kSrcX, kSrcY, kSpecialGaugeWidth, kBarHeight, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kPlayerSpecialGaugeLeft, kSpecialGaugeTop, kSgSrcX, kSgSrcY, static_cast<int>(kHpGaugeWidth * m_playerSpecialGaugeRate),
				  kBarHeight, m_sgGaugeHandle, true);
	DrawFormatStringToHandle(kWarriorSgPosX, kWarriorSgPosY, 0xffb770, m_fontHandle, L"SG");

	// コンパニオンのSpecialゲージの描画
	DrawRectGraph(kCompanionSpecialGaugeLeft, kSpecialGaugeTop, kSrcX, kSrcY, kSpecialGaugeWidth, kBarHeight, m_hpGaugeFrameHandle, true);
	DrawRectGraph(kCompanionSpecialGaugeLeft, kSpecialGaugeTop, kSgSrcX, kSgSrcY, static_cast<int>(kHpGaugeWidth * m_companionSpecialGaugeRate),
				  kBarHeight, m_sgGaugeHandle, true);
	DrawFormatStringToHandle(kWizardSgPosX, kWizardSgPosY, 0xffb770, m_fontHandle, L"SG");
}

void UIManager::DrawBossHp()
{
	// バーの最大幅を少し狭くして、左右の装飾にぶつからないように調整
	int currentDestWidth = static_cast<int>(kMaxDestWidth * m_bossHpGaugeRate);

	// 1. ボスHPフレーム（背景・装飾）を重ねて描画
	DrawRectExtendGraph(kBossHpFrameX1, kBossHpFrameY1, kDistX2, kBossHpFrameY2, 
					    kBossHpSrcX, kBossHpSrcY, kBossHpSrcW, kBossHpSrcH, m_bossHPGaugeFlameHandle, true);
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
		if (VSize(VSub(enemy->GetPos(), m_pPlayer->GetPos())) <= 800.0f || VSize(VSub(enemy->GetPos(), m_pCompanion->GetPos())) <= kDisplayEnemyHp)
		{
			DrawSingleEnemyBar(enemy->GetPos(), enemy->GetHp(), enemy->GetMaxHp(),kEnemyBarWidth, kEnemyBarHeight, kNormalEnemyOffsetY, m_weakIconstickHandle);
		}
	}

	// StrongEnemyのループ
	for (const auto& enemy : m_pStrongEnemies)
	{
		if (enemy->IsDead()) continue;
		if (VSize(VSub(enemy->GetPos(), m_pPlayer->GetPos())) <= 800.0f || VSize(VSub(enemy->GetPos(), m_pCompanion->GetPos())) <= kDisplayEnemyHp)
		{
			DrawSingleEnemyBar(enemy->GetPos(), enemy->GetHp(), enemy->GetMaxHp(),
				kEnemyBarWidth, kEnemyBarHeight, kStrongEnemyOffsetY, m_weakIconAxeHandle);
		}
	}
}

void UIManager::DrawSingleEnemyBar(VECTOR pos, int hp, int maxHp, float width, float height, float offsetY, int weakIconHnadle)
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
	int x2 = x1 + static_cast<int>(width);
	int y2 = y1 + static_cast<int>(height);

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

	if (weakIconHnadle != -1)
	{
		int srcW, srcH;
		GetGraphSize(weakIconHnadle, &srcW, &srcH);

		int iconH = static_cast<int>(height* kWeakPointIconScale);
		int iconW = static_cast<int>(srcW * (iconH / static_cast<float>(srcH)));

		int iconX = x1 - kWeakIconOffset - iconW;
		int iconY = y1 - (iconH - static_cast<int>(height)) / 2; // 中央を基準に上下に広げる

		DrawExtendGraph(iconX, iconY, iconX + iconW, iconY + iconH, weakIconHnadle, TRUE);
	}
}

void UIManager::DrawManualUI()
{
	// 操作説明UIの枠
	DrawRectExtendGraph(kManualFrameX1, kManualFrameY1, kManualFrameX2, kManualFrameY2, 
					    kManualFrameSrcX, kManualFrameSrcY, kManualFrameSrcW, kManualFrameSrcH,m_manualFrameHandle,true);
	// Aボタン　ジャンプ
	DrawGraph(1120, 130, m_aButtonHandle, true); 
	DrawFormatStringToHandle(1155, 140, 0xffffff, m_manualFontHandle, L"ジャンプ");
	// Bボタン　回避
	DrawGraph(1155, 94, m_bButtonHandle, true); 
	DrawFormatStringToHandle(1195, 105, 0xffffff, m_manualFontHandle, L"回避");
	// Xボタン　弱攻撃
	DrawGraph(1085, 95, m_xButtonHandle, true); 
	DrawFormatStringToHandle(1015, 105, 0xffffff, m_manualFontHandle, L"弱攻撃");
	// Yボタン　強攻撃
	DrawGraph(1120, 60, m_yButtonHandle, true); 
	DrawFormatStringToHandle(1155, 70, 0xffffff, m_manualFontHandle, L"強攻撃");
	
	DrawGraph(1040,140,m_triggerButtonHandle,true);
	DrawFormatStringToHandle(1195, 217, 0xffffff, m_manualFontHandle, L"}");
	DrawFormatStringToHandle(1200, 212, 0xffffff, m_smallManualFontHandle, L"キャラ");
	DrawFormatStringToHandle(1200, 230, 0xffffff, m_smallManualFontHandle, L"チェンジ");
	DrawFormatStringToHandle(1075, 217, 0xffffff, m_manualFontHandle, L"{");
	DrawFormatStringToHandle(1025, 218, 0xffffff, m_smallManualFontHandle, L"必殺技");
	
	// 右スティック押し込み　ロックオン
	DrawRectExtendGraph(kStickPosX, kStickPosY, kStickPosX + kStickWidth, kStickPosY + kStickHeight,
		kStickSrcX, kStickSrcY, kStickSrcW, kStickSrcH, m_stickHandle, true);
	DrawFormatStringToHandle(kUITextPosX, kStickText1PosY, 0xffffff, m_manualFontHandle, L"ロックオン");
	DrawFormatStringToHandle(kUITextPosX, kStickText2PosY, 0xffffff, m_manualFontHandle, L"ロックオン解除");
	// コンボ
	DrawGraph(kComboButtonX1, kButtonPosY+2, m_xButtonHandle, true);
	DrawGraph(kComboButtonX2, kButtonPosY, m_yButtonHandle, true);
	DrawGraph(kComboButtonX3, kButtonPosY, m_yButtonHandle, true);
	DrawFormatStringToHandle(kFinishTextPosX, kFinishTextPosY, 0xffffff, m_manualFontHandle, L"コンボ");
	// START　操作説明を閉じる
	DrawGraph(1210,440, m_manualCloaseHandel,true);
	//DrawFormatStringToHandle(kUIPosX, kCloseTextPosY, 0xff4500, m_manualFontHandle, L"START");
	DrawString(1230, kCloseTextPosY, L"START", 0xffffff);
}

void UIManager::DrawBuffUI()
{
	if (m_pPlayer->GetPowerUpBonus() > 0)
	{
		if (!m_pPlayer->IsDead())
		{
			DrawGraph(kWarriorBuffUIPosX, kBuffIconPosY, m_buffIconHandle, true);
		}
		if (!m_pCompanion->IsDead())
		{
			DrawGraph(kWizardBuffUIPosX, kBuffIconPosY, m_buffIconHandle, true);
		}
	}
	
	if (m_pPlayer->GetChangePowerUpBonus() > 0)
	{
		DrawGraph(220, kBuffIconPosY, m_buffSwapIconHandle, true);
	}
	if (m_pCompanion->GetChangePowerUpBonus() > 0)
	{
		DrawGraph(820, kBuffIconPosY, m_buffSwapIconHandle, true);
	}
}

void UIManager::DrawDestination()
{
	float angle = (GetNowCount() % 2000) / 2000.0f * DX_PI_F * 2.0f;
	int alpha = (int)((cos(angle) * 0.5f + 0.5f) * 255);
	DrawBillboard3D(VGet(kDestinationPos.x, kDestinationPos.y+ alpha, kDestinationPos.z), 0.5f, 0.5f, kDestionScale, 0.0f, m_destinationIconHandle, true);
	
	SetDrawBlendMode(DX_BLENDMODE_ADD, alpha);
	DrawBillboard3D(VGet(kDestinationPos.x, kDestinationPos.y + alpha, kDestinationPos.z), 0.5f, 0.5f, kDestionScale, 0.0f, m_destinationIconHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
}

void UIManager::DrawOperationPlayer()
{
	if (m_isOperationWarrior)
	{
		DrawGraph(170, kBuffIconPosY, m_operationPlayerUI, true);
	}
	else
	{
		DrawGraph(770, kBuffIconPosY, m_operationPlayerUI, true);
	}
}
