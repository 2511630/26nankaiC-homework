#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "WeiYan.h"
#include "XuSheng.h"
#include "DeckManager.h"
#include "BasicCards.h"
#include <QFile>
#include <algorithm>
#include <QMessageBox>
#include <QLineEdit>

DeckManager* gDeck = nullptr;

// ==================== CardWidget ====================
CardWidget::CardWidget(const QString& cardName, int index, QWidget* parent)
    : QLabel(parent), cardName(cardName), index(index), selected(false) {
    setFixedSize(108, 154);
    setAlignment(Qt::AlignCenter);
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("QLabel { border: 2px solid rgba(214, 176, 116, 0.72); border-radius: 8px; }");
}

void CardWidget::setSelected(bool sel) {
    selected = sel;
    if (selected) {
        setStyleSheet("QLabel { border: 3px solid gold; border-radius: 8px; background-color: rgba(255, 215, 0, 0.2); }");
    } else {
        setStyleSheet("QLabel { border: 2px solid rgba(214, 176, 116, 0.72); border-radius: 8px; }");
    }
}

void CardWidget::mousePressEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    emit clicked(this);
}

// ==================== HeroCardWidget ====================
HeroCardWidget::HeroCardWidget(const QString& heroName, const QString& imgPath, int maxHp, QWidget* parent)
    : QWidget(parent), heroName(heroName) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    heroImage = new QLabel();
    heroImage->setFixedSize(120, 160);
    heroImage->setAlignment(Qt::AlignCenter);
    heroImage->setStyleSheet("QLabel { border: 2px solid rgba(214, 176, 116, 0.72); border-radius: 8px; }");
    
    QPixmap pixmap(imgPath);
    if (!pixmap.isNull()) {
        heroImage->setPixmap(pixmap.scaled(115, 155, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        heroImage->setText(heroName);
    }
    layout->addWidget(heroImage);

    hpLayout = new QHBoxLayout();
    hpLayout->setSpacing(2);
    hpLayout->setAlignment(Qt::AlignCenter);
    
    for (int i = 0; i < maxHp; ++i) {
        QLabel* hpIcon = new QLabel();
        hpIcon->setFixedSize(20, 20);
        hpIcon->setAlignment(Qt::AlignCenter);
        hpIcon->setStyleSheet("QLabel { background: #D32F2F; border-radius: 10px; }");
        hpIcons.append(hpIcon);
        hpLayout->addWidget(hpIcon);
    }
    layout->addLayout(hpLayout);

    hpLabel = new QLabel(heroName);
    hpLabel->setStyleSheet("QLabel { color: #EED9AD; font-size: 13px; font-weight: bold; text-align: center; }");
    hpLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(hpLabel);
}

void HeroCardWidget::setHp(int hp) {
    for (int i = 0; i < hpIcons.size(); ++i) {
        if (i < hp) {
            hpIcons[i]->setStyleSheet("QLabel { background: #D32F2F; border-radius: 10px; }");
        } else {
            hpIcons[i]->setStyleSheet("QLabel { background: #4CAF50; border-radius: 10px; }");
        }
    }
}

void HeroCardWidget::setSelected(bool selected) {
    if (selected) {
        heroImage->setStyleSheet("QLabel { border: 3px solid #FFD700; border-radius: 8px; }");
    } else {
        heroImage->setStyleSheet("QLabel { border: 2px solid rgba(214, 176, 116, 0.72); border-radius: 8px; }");
    }
}

// ==================== MainWindow ====================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), selectedHandCardIndex(-1) {
    setWindowTitle("三国杀 - 势·魏延 VS 界·徐盛");
    setFixedSize(1280, 800);
    
    weiyan = new WeiYan();
    xusheng = new XuSheng();
    deck = new DeckManager();
    
    imagePath = "D:/OneDrive/Desktop/GitHub/26nankaiC-homework/C++作业图片/";
    
    isWeiYanTurn = true;
    zhuangShiUsed = false;
    beiShuiUsed = false;
    discardPileCount = 0;
    weiyanWineUsedThisTurn = false;
    weiyanWineBuffActive = false;
    xushengWineUsedThisTurn = false;
    xushengWineBuffActive = false;
    zhuangShiDiscardCount = 0;
    discardPhaseCount = 0;
    kunFenDiscardCount = 0;
    flowState = FlowState::START_MENU;

    yingZhanTarget = nullptr;
    yingZhanSelectedIndex = -1;
    
    initUI();
}

MainWindow::~MainWindow() {
    delete weiyan;
    delete xusheng;
    delete deck;
}

void MainWindow::initUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(16, 14, 16, 14);
    mainLayout->setSpacing(10);

    QString bgPath = imagePath + "牌局背景.jpg";
    QPalette palette;
    QPixmap bgImage(bgPath);
    if (bgImage.isNull()) bgImage.load(imagePath + "战局背景.jpg");
    if (bgImage.isNull()) palette.setColor(QPalette::Window, QColor(42, 35, 29));
    else palette.setBrush(QPalette::Window, bgImage.scaled(1280, 800, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    centralWidget->setPalette(palette);
    centralWidget->setAutoFillBackground(true);

    centralWidget->setStyleSheet(
        "QPushButton { background: rgba(52, 37, 25, 0.86); color: #F4E1B6; border: 1px solid rgba(196, 153, 95, 0.92); border-radius: 8px; font-weight: 700; padding: 4px 10px; }"
        "QPushButton:hover { background: rgba(78, 58, 40, 0.92); }"
        "QPushButton:pressed { background: rgba(35, 25, 18, 0.96); }"
        "QPushButton:disabled { color: rgba(230, 220, 200, 0.48); border-color: rgba(160, 140, 118, 0.45); background: rgba(22, 18, 16, 0.6); }"
        "QSpinBox { background: rgba(45, 35, 25, 0.9); color: #F4E1B6; border: 1px solid rgba(196, 153, 95, 0.92); border-radius: 6px; padding: 2px 4px; }"
    );

    QHBoxLayout* topLayout = new QHBoxLayout();
    heroXuSheng = new HeroCardWidget("界·徐盛", imagePath + "界徐盛登场.png", 4);
    topLayout->addStretch(1);
    topLayout->addWidget(heroXuSheng, 0, Qt::AlignCenter);

    QVBoxLayout* xushengEquipLayout = new QVBoxLayout();
    xushengEquipLayout->setSpacing(6);
    xushengEquipLayout->setAlignment(Qt::AlignCenter);
    lblXuShengHandCardCount = new QLabel("手牌：0");
    lblXuShengHandCardCount->setStyleSheet("QLabel { color: #EED9AD; font-size: 13px; font-weight: bold; }");
    xushengEquipLayout->addWidget(lblXuShengHandCardCount);
    lblXuShengWeapon = new QLabel("武器：无");
    lblXuShengWeapon->setStyleSheet("QLabel { color: #EED9AD; font-size: 12px; }");
    xushengEquipLayout->addWidget(lblXuShengWeapon);
    lblXuShengArmor = new QLabel("防具：无");
    lblXuShengArmor->setStyleSheet("QLabel { color: #EED9AD; font-size: 12px; }");
    xushengEquipLayout->addWidget(lblXuShengArmor);
    lblXuShengHorse = new QLabel("坐骑：无");
    lblXuShengHorse->setStyleSheet("QLabel { color: #EED9AD; font-size: 12px; }");
    xushengEquipLayout->addWidget(lblXuShengHorse);
    topLayout->addLayout(xushengEquipLayout);

    topLayout->addStretch(1);
    mainLayout->addLayout(topLayout);

    QHBoxLayout* middleLayout = new QHBoxLayout();
    middleLayout->setSpacing(14);

    QVBoxLayout* leftToolsLayout = new QVBoxLayout();
    leftToolsLayout->setSpacing(10);
    leftToolsLayout->setAlignment(Qt::AlignBottom);
    QPushButton* btnPaiXu = new QPushButton("牌序");
    btnPaiXu->setFixedSize(78, 38);
    leftToolsLayout->addWidget(btnPaiXu);
    QPushButton* btnPaiKu = new QPushButton("牌库");
    btnPaiKu->setFixedSize(78, 38);
    leftToolsLayout->addWidget(btnPaiKu);
    QPushButton* btnJiLu = new QPushButton("记录");
    btnJiLu->setFixedSize(78, 38);
    leftToolsLayout->addWidget(btnJiLu);
    middleLayout->addLayout(leftToolsLayout);

    QFrame* centerPanel = new QFrame();
    centerPanel->setStyleSheet("QFrame { background: rgba(16, 12, 10, 0.33); border: 1px solid rgba(214, 176, 116, 0.38); border-radius: 12px; }");
    QVBoxLayout* centerPanelLayout = new QVBoxLayout(centerPanel);
    centerPanelLayout->setContentsMargins(16, 16, 16, 16);
    centerPanelLayout->setSpacing(14);

    lblPhase = new QLabel("点击开始游戏");
    lblPhase->setStyleSheet("font-size: 24px; font-weight: bold; color: #F9E5B3; letter-spacing: 1px;");
    lblPhase->setAlignment(Qt::AlignCenter);
    centerPanelLayout->addWidget(lblPhase);

    skillAnimationLabel = new QLabel();
    skillAnimationLabel->setMinimumSize(560, 180);
    skillAnimationLabel->setAlignment(Qt::AlignCenter);
    skillAnimationLabel->setVisible(false);
    centerPanelLayout->addWidget(skillAnimationLabel, 0, Qt::AlignCenter);

    QHBoxLayout* tableCardsLayout = new QHBoxLayout();
    tableCardsLayout->setSpacing(20);
    tableCardsLayout->setAlignment(Qt::AlignCenter);
    tableCardLeft = new QLabel();
    tableCardLeft->setFixedSize(108, 154);
    tableCardLeft->setAlignment(Qt::AlignCenter);
    tableCardLeft->setStyleSheet("QLabel { border: 2px solid rgba(214, 176, 116, 0.72); border-radius: 8px; background: rgba(8, 6, 5, 0.45); color: #EED9AD; }");
    tableCardLeft->setVisible(false);
    tableCardsLayout->addWidget(tableCardLeft);
    tableCardRight = new QLabel();
    tableCardRight->setFixedSize(108, 154);
    tableCardRight->setAlignment(Qt::AlignCenter);
    tableCardRight->setStyleSheet("QLabel { border: 2px solid rgba(214, 176, 116, 0.72); border-radius: 8px; background: rgba(8, 6, 5, 0.45); color: #EED9AD; }");
    tableCardRight->setVisible(false);
    tableCardsLayout->addWidget(tableCardRight);
    discardPileLabel = new QLabel("弃牌堆：0");
    discardPileLabel->setStyleSheet("QLabel { color: #EED9AD; font-size: 14px; font-weight: bold; }");
    tableCardsLayout->addWidget(discardPileLabel);
    centerPanelLayout->addLayout(tableCardsLayout);

    QHBoxLayout* deckLayout = new QHBoxLayout();
    deckLayout->setSpacing(10);
    deckLayout->setAlignment(Qt::AlignRight);
    lblDeckCard = new QLabel();
    lblDeckCard->setFixedSize(80, 114);
    lblDeckCard->setAlignment(Qt::AlignCenter);
    QPixmap deckPixmap(getCardImagePath("卡牌背面"));
    if (!deckPixmap.isNull()) {
        lblDeckCard->setPixmap(deckPixmap.scaled(75, 107, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        lblDeckCard->setText("[牌堆]");
    }
    deckLayout->addWidget(lblDeckCard);
    lblDeckCount = new QLabel("0");
    lblDeckCount->setStyleSheet("QLabel { color: #FFFFFF; font-size: 20px; font-weight: bold; background: rgba(0,0,0,0.5); padding: 2px 6px; border-radius: 4px; }");
    lblDeckCount->setAlignment(Qt::AlignCenter);
    lblDeckCount->setFixedWidth(35);
    deckLayout->addWidget(lblDeckCount);
    centerPanelLayout->addLayout(deckLayout);

    zhuangShiPanel = new QWidget();
    zhuangShiPanel->setVisible(false);
    zhuangShiPanel->setMinimumSize(500, 200);
    zhuangShiPanel->setStyleSheet("QWidget { background: rgba(20, 15, 10, 0.95); border: 2px solid #D4A76A; border-radius: 10px; padding: 15px; }");
    QVBoxLayout* zhuangShiVLayout = new QVBoxLayout(zhuangShiPanel);
    zhuangShiVLayout->setContentsMargins(20, 20, 20, 20);
    zhuangShiVLayout->setSpacing(15);

    lblZhuangShiStep = new QLabel("【壮誓】请在下方选择要弃置的手牌");
    lblZhuangShiStep->setStyleSheet("QLabel { color: #F2DEB7; font-size: 16px; font-weight: bold; }");
    lblZhuangShiStep->setAlignment(Qt::AlignCenter);
    zhuangShiVLayout->addWidget(lblZhuangShiStep);

    QHBoxLayout* hpRow = new QHBoxLayout();
    hpRow->setAlignment(Qt::AlignCenter);
    QLabel* hpLabel = new QLabel("掉血：");
    hpLabel->setStyleSheet("QLabel { color: #F2DEB7; font-size: 16px; }");
    hpRow->addWidget(hpLabel);
    editZhuangShiHpLoss = new QLineEdit("0");
    editZhuangShiHpLoss->setFixedSize(100, 40);
    editZhuangShiHpLoss->setAlignment(Qt::AlignCenter);
    editZhuangShiHpLoss->setStyleSheet("QLineEdit { background: #4a2a1a; color: #FFD700; border: 2px solid #D4A76A; font-size: 20px; font-weight: bold; padding: 5px; border-radius: 5px; }");
    hpRow->addWidget(editZhuangShiHpLoss);
    zhuangShiVLayout->addLayout(hpRow);

    QHBoxLayout* zhuangShiButtons = new QHBoxLayout();
    zhuangShiButtons->setSpacing(12);
    zhuangShiButtons->setAlignment(Qt::AlignCenter);
    btnZhuangShiConfirm = new QPushButton("确定");
    btnZhuangShiConfirm->setMinimumSize(120, 45);
    btnZhuangShiConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #FFD700; border: 2px solid #D4A76A; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
    connect(btnZhuangShiConfirm, &QPushButton::clicked, this, &MainWindow::onZhuangShiConfirm);
    zhuangShiButtons->addWidget(btnZhuangShiConfirm);
    btnZhuangShiCancel = new QPushButton("取消（使命失败）");
    btnZhuangShiCancel->setMinimumSize(200, 45);
    btnZhuangShiCancel->setStyleSheet("QPushButton { background: #5a3a2a; color: #FFD700; border: 2px solid #D4A76A; font-size: 14px; font-weight: bold; border-radius: 8px; padding: 8px; }");
    connect(btnZhuangShiCancel, &QPushButton::clicked, this, &MainWindow::onZhuangShiCancel);
    zhuangShiButtons->addWidget(btnZhuangShiCancel);
    zhuangShiVLayout->addLayout(zhuangShiButtons);

    targetSelectPanel = new QWidget();
    targetSelectPanel->setVisible(false);
    targetSelectPanel->setMinimumSize(400, 200);
    targetSelectPanel->setStyleSheet("QWidget { background: rgba(20, 15, 10, 0.9); border: 2px solid #D4A76A; border-radius: 10px; }");
    QVBoxLayout* targetSelectLayout = new QVBoxLayout(targetSelectPanel);
    targetSelectLayout->setContentsMargins(10, 10, 10, 10);
    targetSelectLayout->setSpacing(8);
    lblTargetSelect = new QLabel("请选择一张牌");
    lblTargetSelect->setStyleSheet("QLabel { color: #F2DEB7; font-size: 15px; font-weight: bold; }");
    lblTargetSelect->setAlignment(Qt::AlignCenter);
    targetSelectLayout->addWidget(lblTargetSelect);

    pojunSelectPanel = new QWidget();
    pojunSelectPanel->setVisible(false);
    pojunSelectPanel->setMinimumSize(500, 280);
    pojunSelectPanel->setStyleSheet("QWidget { background: rgba(20, 15, 10, 0.9); border: 2px solid #D4A76A; border-radius: 10px; }");
    QVBoxLayout* pojunSelectLayout = new QVBoxLayout(pojunSelectPanel);
    pojunSelectLayout->setContentsMargins(10, 10, 10, 10);
    pojunSelectLayout->setSpacing(8);
    lblPojunInfo = new QLabel("【破军】请选择要扣置的牌");
    lblPojunInfo->setStyleSheet("QLabel { color: #F2DEB7; font-size: 15px; font-weight: bold; }");
    lblPojunInfo->setAlignment(Qt::AlignCenter);
    pojunSelectLayout->addWidget(lblPojunInfo);

    QHBoxLayout* skillButtons = new QHBoxLayout();
    skillButtons->setSpacing(10);
    skillButtons->setAlignment(Qt::AlignCenter);
    btnStart = new QPushButton("开始游戏");
    btnStart->setFixedSize(120, 40);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onStartGame);
    skillButtons->addWidget(btnStart);
    btnZhuangShi = new QPushButton("壮誓");
    btnZhuangShi->setFixedSize(88, 40);
    btnZhuangShi->setVisible(false);
    btnZhuangShi->setEnabled(false);
    connect(btnZhuangShi, &QPushButton::clicked, this, &MainWindow::onWeiYanZhuangShi);
    skillButtons->addWidget(btnZhuangShi);
    btnBeiShui = new QPushButton("背水");
    btnBeiShui->setFixedSize(88, 40);
    btnBeiShui->setVisible(false);
    btnBeiShui->setEnabled(false);
    connect(btnBeiShui, &QPushButton::clicked, this, &MainWindow::onWeiYanBeiShui);
    skillButtons->addWidget(btnBeiShui);
    btnEndTurn = new QPushButton("结束回合");
    btnEndTurn->setFixedSize(108, 40);
    btnEndTurn->setVisible(false);
    btnEndTurn->setEnabled(false);
    connect(btnEndTurn, &QPushButton::clicked, this, &MainWindow::onEndTurn);
    skillButtons->addWidget(btnEndTurn);
    centerPanelLayout->addLayout(skillButtons);

    QHBoxLayout* actionButtons = new QHBoxLayout();
    actionButtons->setSpacing(12);
    actionButtons->setAlignment(Qt::AlignCenter);
    btnConfirm = new QPushButton("确定");
    btnConfirm->setMinimumSize(120, 45);
    btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #FFD700; border: 2px solid #D4A76A; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
    btnConfirm->setVisible(false);
    btnConfirm->setEnabled(false);
    connect(btnConfirm, &QPushButton::clicked, this, &MainWindow::resolveSelectedCardPlay);
    actionButtons->addWidget(btnConfirm);
    btnCancel = new QPushButton("取消");
    btnCancel->setMinimumSize(180, 45);
    btnCancel->setStyleSheet("QPushButton { background: #5a3a2a; color: #FFD700; border: 2px solid #D4A76A; font-size: 14px; font-weight: bold; border-radius: 8px; padding: 8px; }");
    btnCancel->setVisible(false);
    btnCancel->setEnabled(false);
    connect(btnCancel, &QPushButton::clicked, [this]() {
        if (flowState == FlowState::SELECT_TRICK_TARGET) {
            targetSelectPanel->setVisible(false);
            currentTrickCardName.clear();
            enterFlowState(FlowState::TURN_PLAY_SELECT);
            appendLog("取消当前操作");
            return;
        }
        if (flowState == FlowState::SELECT_POJUN_TARGET) {
            pojunSelectedIndices.clear();
            pojunSelectPanel->setVisible(false);
            appendLog("【破军】跳过扣牌");
            appendLog("【破军】判断是否响应...");
            bool hasShan = false;
            int shanIndex = -1;
            for (size_t i = 0; i < pojunTarget->handCards.size(); ++i) {
                if (pojunTarget->handCards[i]->getName() == "闪") {
                    hasShan = true;
                    shanIndex = i;
                    break;
                }
            }
            if (hasShan && shanIndex >= 0) {
                appendLog(QString("%1使用【闪】响应！").arg(pojunTarget->name));
                pojunTarget->handCards.erase(pojunTarget->handCards.begin() + shanIndex);
                appendLog(QString("【%1】闪避成功，受到0点伤害").arg(pojunTarget->name));
                refreshHandCards();
                updateUI();
                
                enterFlowState(FlowState::TURN_PLAY_SELECT);
                lblPhase->setText("出牌阶段 - 界·徐盛");
            } else {
                appendLog(QString("%1没有【闪】，无法响应！").arg(pojunTarget->name));
                QTimer::singleShot(500, this, [this]() {
                    resolvePojunNoDodge(pojunTarget);
                });
            }
            return;
        }
        if (flowState == FlowState::ZHUANGSHI_DISCARD) {
            zhuangShiPanel->setVisible(false);
            weiyan->skipZhuangShi();
            zhuangShiUsed = true;
            appendLog("【壮誓】取消，使命失败！");
            enterFlowState(FlowState::TURN_PLAY_SELECT);
            selectedDiscardIndices.clear();
            refreshHandCards();
            updateSkillPanelVisibility();
            updateUI();
            return;
        }
        selectedHandCardIndex = -1;
        pendingCardName.clear();
        selectedDiscardIndices.clear();
        refreshHandCards();
        updateConfirmState();
        appendLog("取消当前操作");
    });
    actionButtons->addWidget(btnCancel);
    centerPanelLayout->addLayout(actionButtons);

    QHBoxLayout* kuangGuButtons = new QHBoxLayout();
    kuangGuButtons->setSpacing(10);
    kuangGuButtons->setAlignment(Qt::AlignCenter);
    btnKuangGuHeal = new QPushButton("回复1点体力");
    btnKuangGuHeal->setFixedSize(130, 38);
    btnKuangGuHeal->setVisible(false);
    btnKuangGuHeal->setEnabled(false);
    connect(btnKuangGuHeal, &QPushButton::clicked, this, &MainWindow::onWeiYanKuangGuHeal);
    kuangGuButtons->addWidget(btnKuangGuHeal);
    btnKuangGuDraw = new QPushButton("摸一张牌");
    btnKuangGuDraw->setFixedSize(130, 38);
    btnKuangGuDraw->setVisible(false);
    btnKuangGuDraw->setEnabled(false);
    connect(btnKuangGuDraw, &QPushButton::clicked, this, &MainWindow::onWeiYanKuangGuDraw);
    kuangGuButtons->addWidget(btnKuangGuDraw);
    btnKuangGuSkip = new QPushButton("不发动");
    btnKuangGuSkip->setFixedSize(130, 38);
    btnKuangGuSkip->setVisible(false);
    btnKuangGuSkip->setEnabled(false);
    connect(btnKuangGuSkip, &QPushButton::clicked, this, &MainWindow::onWeiYanKuangGuSkip);
    kuangGuButtons->addWidget(btnKuangGuSkip);
    centerPanelLayout->addLayout(kuangGuButtons);

    middleLayout->addWidget(centerPanel, 1);

    QVBoxLayout* selectPanelsLayout = new QVBoxLayout();
    selectPanelsLayout->setSpacing(5);
    selectPanelsLayout->setContentsMargins(0, 0, 0, 0);
    selectPanelsLayout->addWidget(zhuangShiPanel);
    selectPanelsLayout->addWidget(targetSelectPanel);
    selectPanelsLayout->addWidget(pojunSelectPanel);
    middleLayout->addLayout(selectPanelsLayout);
    selectPanelsLayout->setStretchFactor(zhuangShiPanel, 0);
    selectPanelsLayout->setStretchFactor(targetSelectPanel, 0);
    selectPanelsLayout->setStretchFactor(pojunSelectPanel, 0);

    QVBoxLayout* rightPanel = new QVBoxLayout();
    rightPanel->setSpacing(10);
    rightPanel->setAlignment(Qt::AlignBottom);
    txtLog = new QTextEdit();
    txtLog->setReadOnly(true);
    txtLog->setMinimumSize(250, 250);
    txtLog->setStyleSheet("QTextEdit { background: rgba(9, 7, 6, 0.76); color: #EFE3C5; font-size: 13px; border: 1px solid rgba(214, 176, 116, 0.72); border-radius: 10px; padding: 6px; }");
    rightPanel->addWidget(txtLog);
    middleLayout->addLayout(rightPanel);
    mainLayout->addLayout(middleLayout, 1);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(14);
    heroWeiYan = new HeroCardWidget("势·魏延", imagePath + "势魏延登场.png", 5);
    heroWeiYan->setHp(5);
    bottomLayout->addWidget(heroWeiYan, 0, Qt::AlignBottom);

    QVBoxLayout* equipLayout = new QVBoxLayout();
    equipLayout->setSpacing(8);
    equipLayout->setAlignment(Qt::AlignCenter);
    lblHandCardCount = new QLabel("手牌：0");
    lblHandCardCount->setStyleSheet("QLabel { color: #EED9AD; font-size: 14px; font-weight: bold; }");
    equipLayout->addWidget(lblHandCardCount);
    lblWeiYanWeapon = new QLabel("武器：无");
    lblWeiYanWeapon->setStyleSheet("QLabel { color: #EED9AD; font-size: 12px; }");
    equipLayout->addWidget(lblWeiYanWeapon);
    lblWeiYanArmor = new QLabel("防具：无");
    lblWeiYanArmor->setStyleSheet("QLabel { color: #EED9AD; font-size: 12px; }");
    equipLayout->addWidget(lblWeiYanArmor);
    lblWeiYanHorse = new QLabel("坐骑：无");
    lblWeiYanHorse->setStyleSheet("QLabel { color: #EED9AD; font-size: 12px; }");
    equipLayout->addWidget(lblWeiYanHorse);
    bottomLayout->addLayout(equipLayout);

    handCardsArea = new QWidget();
    handCardsArea->setMinimumHeight(190);
    handCardsArea->setStyleSheet("QWidget { background: rgba(8, 6, 5, 0.35); border: 1px solid rgba(214, 176, 116, 0.33); border-radius: 12px; }");
    handCardsLayout = new QHBoxLayout(handCardsArea);
    handCardsLayout->setContentsMargins(18, 14, 18, 14);
    handCardsLayout->setSpacing(10);
    handCardsLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    bottomLayout->addWidget(handCardsArea, 1);
    mainLayout->addLayout(bottomLayout);

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, [this]() {
        skillAnimationLabel->setVisible(false);
    });
}

void MainWindow::refreshHandCards() {
    qDeleteAll(handCardWidgets);
    handCardWidgets.clear();
    QLayoutItem* item;
    while ((item = handCardsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    Player* player = currentPlayer();
    if (!player) return;

    for (size_t i = 0; i < player->handCards.size(); ++i) {
        QString cardName = player->handCards[i]->getName();
        CardWidget* cardWidget = new CardWidget(cardName, i);
        
        QString imgPath = getCardImagePath(cardName);
        QPixmap pixmap(imgPath);
        if (!pixmap.isNull()) {
            cardWidget->setPixmap(pixmap.scaled(100, 146, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(cardName);
        }

        bool isSelected = false;
        if (flowState == FlowState::ZHUANGSHI_SELECT) {
            for (int idx : zhuangShiSelectedCards) {
                if (idx == (int)i) {
                    isSelected = true;
                    cardWidget->selected = true;
                    break;
                }
            }
        } else if (flowState == FlowState::ZHUANGSHI_DISCARD || flowState == FlowState::DISCARD_PHASE || flowState == FlowState::KUNFEN_END_PHASE) {
            for (int idx : selectedDiscardIndices) {
                if (idx == (int)i) {
                    isSelected = true;
                    cardWidget->selected = true;
                    break;
                }
            }
        } else if (flowState == FlowState::POJUN_SELECT) {
            for (int idx : pojunSelectedIndices) {
                if (idx == (int)i) {
                    isSelected = true;
                    cardWidget->selected = true;
                    break;
                }
            }
        }

        if (selectedHandCardIndex == (int)i) {
            cardWidget->setStyleSheet("QLabel { border: 3px solid #FFD700; border-radius: 8px; }");
        } else if (isSelected) {
            cardWidget->setStyleSheet("QLabel { border: 3px solid gold; border-radius: 8px; background-color: rgba(255, 215, 0, 0.2); }");
        } else {
            cardWidget->setStyleSheet("QLabel { border: 2px solid rgba(214, 176, 116, 0.72); border-radius: 8px; }");
        }

        connect(cardWidget, &CardWidget::clicked, this, &MainWindow::onCardClicked);

        handCardWidgets.append(cardWidget);
        handCardsLayout->addWidget(cardWidget);
    }
    updateEquipmentDisplay();
}

void MainWindow::onCardClicked(CardWidget* card) {
    if (flowState == FlowState::ZHUANGSHI_SELECT) {
        auto it = std::find(zhuangShiSelectedCards.begin(), zhuangShiSelectedCards.end(), card->index);
        if (it != zhuangShiSelectedCards.end()) {
            zhuangShiSelectedCards.erase(it);
        } else {
            zhuangShiSelectedCards.push_back(card->index);
        }
        refreshHandCards();
        updateConfirmState();
    } else if (flowState == FlowState::ZHUANGSHI_SELECT_CARDS) {
        if (selectedHandCardIndex == card->index) {
            selectedHandCardIndex = -1;
        } else {
            selectedHandCardIndex = card->index;
        }
        refreshHandCards();
        updateConfirmState();
    } else if (flowState == FlowState::ZHUANGSHI_DISCARD) {
        auto it = std::find(selectedDiscardIndices.begin(), selectedDiscardIndices.end(), card->index);
        if (it != selectedDiscardIndices.end()) {
            selectedDiscardIndices.erase(it);
        } else {
            selectedDiscardIndices.push_back(card->index);
        }
        refreshHandCards();
        updateConfirmState();
    } else if (flowState == FlowState::DISCARD_PHASE || flowState == FlowState::KUNFEN_END_PHASE) {
        auto it = std::find(selectedDiscardIndices.begin(), selectedDiscardIndices.end(), card->index);
        if (it != selectedDiscardIndices.end()) {
            selectedDiscardIndices.erase(it);
        } else {
            int maxDiscard = discardPhaseCount;
            if (flowState == FlowState::KUNFEN_END_PHASE) {
                maxDiscard = (int)currentPlayer()->handCards.size() - currentPlayer()->hp;
                if (maxDiscard < 0) maxDiscard = 0;
            }
            if ((int)selectedDiscardIndices.size() < maxDiscard) {
                selectedDiscardIndices.push_back(card->index);
            }
        }
        refreshHandCards();
        updateConfirmState();
    } else if (flowState == FlowState::POJUN_SELECT) {
        auto it = std::find(pojunSelectedIndices.begin(), pojunSelectedIndices.end(), card->index);
        if (it != pojunSelectedIndices.end()) {
            pojunSelectedIndices.erase(it);
        } else {
            if ((int)pojunSelectedIndices.size() < pojunKouZhiCount) {
                pojunSelectedIndices.push_back(card->index);
            }
        }
        int selectedCount = pojunSelectedIndices.size();
        lblPojunInfo->setText(QString("【破军】已选择%1张牌，最多可扣%2张").arg(selectedCount).arg(pojunKouZhiCount));
        refreshHandCards();
        updateConfirmState();
    } else {
        if (selectedHandCardIndex == card->index) {
            selectedHandCardIndex = -1;
            pendingCardName.clear();
        } else {
            selectedHandCardIndex = card->index;
            pendingCardName = card->cardName;
        }
        refreshHandCards();
        updateConfirmState();
    }
}

QString MainWindow::getCardImagePath(const QString& cardName) {
    QString cardPath = imagePath + cardName + ".jpg";
    QFile file(cardPath);
    if (file.exists()) return cardPath;
    
    QString altPath = imagePath + cardName + ".png";
    QFile altFile(altPath);
    if (altFile.exists()) return altPath;
    
    return imagePath + "卡牌背面.jpg";
}

bool MainWindow::isCardPlayable(const QString& cardName) const {
    if (!currentPlayer()) return false;

    // 闪：正常出牌阶段不能出，只有被攻击时才能用
    if (cardName == "闪") {
        return false;
    }

    if (cardName == "桃") {
        return currentPlayer()->hp < currentPlayer()->maxHp || currentPlayer()->hp <= 0;
    }
    if (cardName == "酒") {
        if (currentPlayer()->hp <= 0) return true;
        return isWeiYanTurn ? !weiyanWineUsedThisTurn : !xushengWineUsedThisTurn;
    }
    if (cardName == "杀") {
        Player* p = isWeiYanTurn ? static_cast<Player*>(weiyan) : static_cast<Player*>(xusheng);
        Player* enemy = isWeiYanTurn ? static_cast<Player*>(xusheng) : static_cast<Player*>(weiyan);
        
        // 检查距离，壮誓无距离限制
        bool zhuangShiNoLimit = isWeiYanTurn && weiyan->zhuangShiNoLimitRemaining > 0;
        if (!zhuangShiNoLimit) {
            int distance = p->getCurrentDistanceTo(enemy);
            int attackRange = p->getAttackRange();
            if (distance > attackRange) {
                return false;
            }
        }
        
        if (isWeiYanTurn) {
            int noLimitExtra = 0;
            if (weiyan->zhuangShiHpLost > 0) {
                noLimitExtra = weiyan->zhuangShiHpLost - weiyan->zhuangShiNoLimitUsed;
                if (noLimitExtra < 0) noLimitExtra = 0;
            }
            return weiyan->slashUsedThisTurn < (weiyan->slashCountLimit + noLimitExtra);
        }
        return p->slashUsedThisTurn < p->slashCountLimit;
    }
    if (cardName == "顺手牵羊") {
        Player* player = isWeiYanTurn ? static_cast<Player*>(weiyan) : static_cast<Player*>(xusheng);
        Player* enemy = isWeiYanTurn ? static_cast<Player*>(xusheng) : static_cast<Player*>(weiyan);
        
        // 检查距离
        int distance = player->getCurrentDistanceTo(enemy);
        if (distance > 1) {
            return false;
        }
        return enemy->handCards.size() > 0 || enemy->equipment.weapon || enemy->equipment.armor ||
               enemy->equipment.plusHorse || enemy->equipment.minusHorse;
    }
    if (cardName == "过河拆桥") {
        Player* enemy = isWeiYanTurn ? static_cast<Player*>(xusheng) : static_cast<Player*>(weiyan);
        return enemy->handCards.size() > 0 || enemy->equipment.weapon || enemy->equipment.armor ||
               enemy->equipment.plusHorse || enemy->equipment.minusHorse;
    }
    if (cardName == "决斗" || cardName == "南蛮入侵" || cardName == "万箭齐发") {
        Player* enemy = isWeiYanTurn ? static_cast<Player*>(xusheng) : static_cast<Player*>(weiyan);
        return enemy->isAlive();
    }
    return true;
}

void MainWindow::updateConfirmState() {
    if (flowState == FlowState::ZHUANGSHI_SELECT) {
        btnConfirm->setVisible(false);
        btnCancel->setVisible(false);
    } else if (flowState == FlowState::ZHUANGSHI_DISCARD) {
        btnConfirm->setVisible(true);
        btnCancel->setVisible(true);
        bool enabled = (int)selectedDiscardIndices.size() == zhuangShiDiscardCount;
        btnConfirm->setEnabled(enabled);
        if (enabled) {
            btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #FFD700; border: 2px solid #D4A76A; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
        } else {
            btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #888888; border: 2px solid #666666; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
        }
        btnConfirm->setText("确定弃牌");
        btnCancel->setText("取消");
        btnCancel->setEnabled(true);
    } else if (flowState == FlowState::DISCARD_PHASE) {
        btnConfirm->setVisible(true);
        btnCancel->setVisible(true);
        bool enabled = (int)selectedDiscardIndices.size() == discardPhaseCount;
        btnConfirm->setEnabled(enabled);
        if (enabled) {
            btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #FFD700; border: 2px solid #D4A76A; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
        } else {
            btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #888888; border: 2px solid #666666; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
        }
        btnConfirm->setText("确定");
        btnCancel->setText("取消");
        btnCancel->setEnabled(true);
    } else if (flowState == FlowState::KUNFEN_END_PHASE) {
        int needDiscard = (int)weiyan->handCards.size() - weiyan->hp;
        if (needDiscard < 0) needDiscard = 0;
        btnConfirm->setVisible(true);
        btnCancel->setVisible(false);
        bool enabled = (int)selectedDiscardIndices.size() == needDiscard;
        btnConfirm->setEnabled(enabled);
        if (enabled) {
            btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #FFD700; border: 2px solid #D4A76A; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
        } else {
            btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #888888; border: 2px solid #666666; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
        }
        btnConfirm->setText("确定弃牌");
    } else if (flowState == FlowState::SELECT_POJUN_TARGET) {
        btnConfirm->setVisible(true);
        btnCancel->setVisible(true);
        btnConfirm->setEnabled(true);
        btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #FFD700; border: 2px solid #D4A76A; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
        btnConfirm->setText("确定扣牌");
        btnCancel->setText("跳过扣牌");
    } else if (flowState == FlowState::POJUN_SELECT) {
        btnConfirm->setVisible(true);
        btnCancel->setVisible(false);
        btnConfirm->setEnabled(true);
        btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #FFD700; border: 2px solid #D4A76A; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
        btnConfirm->setText("确定扣牌");
    } else if (flowState == FlowState::SELECT_TRICK_TARGET || flowState == FlowState::SELECT_YINGZHAN_CARD) {
        btnConfirm->setVisible(false);
        btnCancel->setVisible(false);
    } else {
        bool canConfirm = selectedHandCardIndex >= 0 && !pendingCardName.isEmpty() && isCardPlayable(pendingCardName);
        bool hasSelection = selectedHandCardIndex >= 0;
        btnConfirm->setVisible(true);
        btnCancel->setVisible(true);
        btnConfirm->setText("出牌");
        btnCancel->setText("取消");
        btnConfirm->setEnabled(canConfirm);
        btnCancel->setEnabled(hasSelection);
        
        if (canConfirm) {
            btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #FFD700; border: 2px solid #D4A76A; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
        } else {
            btnConfirm->setStyleSheet("QPushButton { background: #5a3a2a; color: #888888; border: 2px solid #666666; font-size: 16px; font-weight: bold; border-radius: 8px; padding: 8px; }");
        }
    }
}

void MainWindow::resolveSelectedCardPlay() {
    if (flowState == FlowState::DISCARD_PHASE) {
        onDiscardPhaseConfirm();
        return;
    }

    if (flowState == FlowState::KUNFEN_END_PHASE) {
        onKunFenEndPhaseConfirm();
        return;
    }

    if (flowState == FlowState::SELECT_POJUN_TARGET) {
        onPojunConfirm();
        return;
    }

    if (flowState == FlowState::POJUN_SELECT) {
        resolvePojunSelect();
        return;
    }

    if (selectedHandCardIndex < 0 || pendingCardName.isEmpty()) return;

    Player* player = currentPlayer();
    Player* enemy = enemyPlayer();
    if (!player || !enemy) return;

    bool zhuangShiNoLimitActive = false;
    if (isWeiYanTurn && weiyan->zhuangShiNoLimitRemaining > 0) {
        zhuangShiNoLimitActive = true;
        weiyan->zhuangShiNoLimitRemaining--;
        weiyan->zhuangShiNoLimitUsed++;
        appendLog(QString("【壮誓】此牌无距离限制且不可被响应（剩余%1次）").arg(weiyan->zhuangShiNoLimitRemaining));
    }

    std::shared_ptr<Card> selectedCard = nullptr;
    int idx = 0;
    for (auto it = player->handCards.begin(); it != player->handCards.end(); ++it) {
        if (idx == selectedHandCardIndex) {
            selectedCard = *it;
            player->handCards.erase(it);
            break;
        }
        idx++;
    }

    if (!selectedCard) return;

    if (pendingCardName == "杀") {
        if (zhuangShiNoLimitActive) {
            weiyan->cardsUsedThisTurn++;
            int damage = 1; // 基础伤害，酒的buff在WeiYan类中处理
            bool yingZhanJiaShang = false;
            bool chengShiTriggered = false;
            bool needYingZhan = weiyan->useSlash(enemy, damage, yingZhanJiaShang, chengShiTriggered);
            
            if (yingZhanJiaShang) {
                appendLog("【饮战】势魏延体力≤目标，伤害+1");
            }
            if (chengShiTriggered) {
                appendLog("【乘势】两个条件均满足！");
            }
            
            if (needYingZhan) {
                showYingZhanCardSelection(enemy);
            } else {
                triggerKuangGu(weiyan, enemy, damage);
                if (enemy->hp <= 0) {
                    appendLog(QString("【%1】进入濒死状态").arg(enemy->name));
                    handleNearDeath(enemy);
                }
            }
        } else {
            useSlashEffect(player, enemy, isWeiYanTurn);
        }
    } else if (pendingCardName == "桃") {
        player->heal(1);
        appendLog(QString("【%1】使用【桃】，回复1点体力").arg(player->name));
    } else if (pendingCardName == "酒") {
        if (player->hp <= 0) {
            player->heal(1);
            appendLog(QString("【%1】濒死使用【酒】，回复1点体力").arg(player->name));
        } else {
            if (isWeiYanTurn) {
                // 检查是否在壮誓不计入次数的范围内
                if (weiyan->zhuangShiNoLimitRemaining > 0) {
                    // 不计入次数，可以喝多个
                    weiyan->wineBuffCount++;
                    weiyan->zhuangShiNoLimitRemaining--;
                    weiyan->zhuangShiNoLimitUsed++;
                    appendLog(QString("【%1】使用【酒】（不计入次数），伤害buff +1，当前%d层").arg(player->name).arg(weiyan->wineBuffCount));
                } else {
                    // 正常只能喝一个
                    if (weiyanWineBuffActive) {
                        appendLog("已经喝过酒了！");
                        player->handCards.push_back(selectedCard);
                        selectedHandCardIndex = -1;
                        pendingCardName.clear();
                        refreshHandCards();
                        return;
                    }
                    weiyanWineBuffActive = true;
                    appendLog(QString("【%1】使用【酒】，下一张杀伤害+1").arg(player->name));
                }
            } else {
                if (xushengWineBuffActive) {
                    appendLog("已经喝过酒了！");
                    player->handCards.push_back(selectedCard);
                    selectedHandCardIndex = -1;
                    pendingCardName.clear();
                    refreshHandCards();
                    return;
                }
                xushengWineBuffActive = true;
                appendLog(QString("【%1】使用【酒】，下一张杀伤害+1").arg(player->name));
            }
        }
    } else if (pendingCardName == "决斗" || pendingCardName == "南蛮入侵" || pendingCardName == "万箭齐发") {
        useCardEffect(selectedCard, player, enemy);
    } else if (pendingCardName == "顺手牵羊") {
        if (!zhuangShiNoLimitActive && player->getCurrentDistance() > 1) {
            appendLog("【顺手牵羊】失败：距离超过1");
            player->handCards.push_back(selectedCard);
            selectedHandCardIndex = -1;
            pendingCardName.clear();
            refreshHandCards();
            return;
        }
        showTargetCardSelection(enemy, "顺手牵羊");
        return;
    } else if (pendingCardName == "过河拆桥") {
        showTargetCardSelection(enemy, "过河拆桥");
        return;
    } else if (pendingCardName == "诸葛连弩" || pendingCardName == "古锭刀" || pendingCardName == "麒麟弓") {
        player->equipment.weapon = selectedCard;
        appendLog(QString("【%1】装备了武器【%2】").arg(player->name).arg(pendingCardName));
    } else if (pendingCardName == "仁王盾" || pendingCardName == "白银狮子") {
        player->equipment.armor = selectedCard;
        appendLog(QString("【%1】装备了防具【%2】").arg(player->name).arg(pendingCardName));
    } else if (pendingCardName == "加一马" || pendingCardName == "+1马") {
        player->equipment.plusHorse = selectedCard;
        appendLog(QString("【%1】装备了+1马").arg(player->name));
    } else if (pendingCardName == "减一马" || pendingCardName == "-1马") {
        player->equipment.minusHorse = selectedCard;
        appendLog(QString("【%1】装备了-1马").arg(player->name));
    } else {
        selectedCard->execute(player, enemy);
    }

    if (isWeiYanTurn) {
        weiyan->cardsUsedThisTurn++;
    }

    selectedHandCardIndex = -1;
    pendingCardName.clear();
    refreshHandCards();
    updateSkillPanelVisibility();
    updateUI();
}

void MainWindow::useCardEffect(std::shared_ptr<Card> card, Player* source, Player* target) {
    QString cardName = card->name;
    appendLog(QString("【%1】使用【%2】").arg(source->name).arg(cardName));

    if (cardName == "决斗") {
        Player* attacker = source;
        Player* defender = target;
        while (true) {
            bool hasSlash = false;
            for (auto it = defender->handCards.begin(); it != defender->handCards.end(); ++it) {
                if ((*it)->name == "杀") {
                    defender->handCards.erase(it);
                    hasSlash = true;
                    appendLog(QString("【%1】打出【杀】").arg(defender->name));
                    std::swap(attacker, defender);
                    break;
                }
            }
            if (!hasSlash) {
                defender->takeDamage(1);
                appendLog(QString("【%1】无法打出【杀】，受到1点伤害").arg(defender->name));
                if (isWeiYanTurn && attacker == weiyan) {
                    weiyan->cardsUsedThisTurn++;
                    triggerKuangGu(weiyan, defender, 1);
                }
                if (defender->hp <= 0) {
                    handleNearDeath(defender);
                }
                break;
            }
        }
    } else if (cardName == "南蛮入侵") {
        bool hasSlash = false;
        for (auto it = target->handCards.begin(); it != target->handCards.end(); ++it) {
            if ((*it)->name == "杀") {
                target->handCards.erase(it);
                hasSlash = true;
                appendLog(QString("【%1】打出【杀】，抵消伤害").arg(target->name));
                break;
            }
        }
        if (!hasSlash) {
            target->takeDamage(1);
            appendLog(QString("【%1】无法打出【杀】，受到1点伤害").arg(target->name));
            if (isWeiYanTurn && source == weiyan) {
                weiyan->cardsUsedThisTurn++;
                triggerKuangGu(weiyan, target, 1);
            }
            if (target->hp <= 0) {
                handleNearDeath(target);
            }
        }
    } else if (cardName == "万箭齐发") {
        bool hasDodge = false;
        for (auto it = target->handCards.begin(); it != target->handCards.end(); ++it) {
            if ((*it)->name == "闪") {
                target->handCards.erase(it);
                hasDodge = true;
                appendLog(QString("【%1】打出【闪】，抵消伤害").arg(target->name));
                break;
            }
        }
        if (!hasDodge) {
            target->takeDamage(1);
            appendLog(QString("【%1】无法打出【闪】，受到1点伤害").arg(target->name));
            if (isWeiYanTurn && source == weiyan) {
                weiyan->cardsUsedThisTurn++;
                triggerKuangGu(weiyan, target, 1);
            }
            if (target->hp <= 0) {
                handleNearDeath(target);
            }
        }
    } else if (cardName == "顺手牵羊") {
        if (source->getCurrentDistance() > 1) {
            appendLog("【顺手牵羊】失败：距离超过1");
        } else if (!target->handCards.empty()) {
            source->handCards.push_back(target->handCards.back());
            target->handCards.pop_back();
            appendLog(QString("【顺手牵羊】成功：获得【%1】的手牌").arg(target->name));
        } else if (target->equipment.weapon) {
            source->handCards.push_back(target->equipment.weapon);
            target->equipment.weapon.reset();
            appendLog(QString("【顺手牵羊】成功：获得【%1】的武器").arg(target->name));
        } else if (target->equipment.armor) {
            source->handCards.push_back(target->equipment.armor);
            target->equipment.armor.reset();
            appendLog(QString("【顺手牵羊】成功：获得【%1】的防具").arg(target->name));
        } else if (target->equipment.plusHorse) {
            source->handCards.push_back(target->equipment.plusHorse);
            target->equipment.plusHorse.reset();
            appendLog(QString("【顺手牵羊】成功：获得【%1】的+1马").arg(target->name));
        } else if (target->equipment.minusHorse) {
            source->handCards.push_back(target->equipment.minusHorse);
            target->equipment.minusHorse.reset();
            appendLog(QString("【顺手牵羊】成功：获得【%1】的-1马").arg(target->name));
        }
    } else if (cardName == "过河拆桥") {
        if (!target->handCards.empty()) {
            target->handCards.pop_back();
            discardPileCount++;
            discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
            appendLog(QString("【过河拆桥】弃置【%1】一张手牌").arg(target->name));
        } else if (target->equipment.weapon) {
            target->equipment.weapon.reset();
            discardPileCount++;
            discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
            appendLog(QString("【过河拆桥】弃置【%1】的武器").arg(target->name));
        } else if (target->equipment.armor) {
            target->equipment.armor.reset();
            discardPileCount++;
            discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
            appendLog(QString("【过河拆桥】弃置【%1】的防具").arg(target->name));
        } else if (target->equipment.plusHorse) {
            target->equipment.plusHorse.reset();
            discardPileCount++;
            discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
            appendLog(QString("【过河拆桥】弃置【%1】的+1马").arg(target->name));
        } else if (target->equipment.minusHorse) {
            target->equipment.minusHorse.reset();
            discardPileCount++;
            discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
            appendLog(QString("【过河拆桥】弃置【%1】的-1马").arg(target->name));
        }
    } else if (cardName == "诸葛连弩" || cardName == "古锭刀" || cardName == "麒麟弓") {
        source->equipment.weapon = card;
        appendLog(QString("【%1】装备了武器【%2】").arg(source->name).arg(cardName));
    } else if (cardName == "仁王盾" || cardName == "白银狮子") {
        source->equipment.armor = card;
        appendLog(QString("【%1】装备了防具【%2】").arg(source->name).arg(cardName));
    } else if (cardName == "+1马") {
        source->equipment.plusHorse = card;
        appendLog(QString("【%1】装备了+1马").arg(source->name));
    } else if (cardName == "-1马") {
        source->equipment.minusHorse = card;
        appendLog(QString("【%1】装备了-1马").arg(source->name));
    } else {
        card->execute(source, target);
    }
}

void MainWindow::triggerKuangGu(WeiYan* wy, Player* target, int /*damage*/) {
    if (wy->zhongAoState != MISSION_ACTIVE && wy->zhongAoState != MISSION_SUCCESS) return;
    
    int distance = wy->getCurrentDistanceTo(target);
    if (distance > 1) {
        appendLog(QString("【狂骨】目标距离%1超过1，不触发").arg(distance));
        return;
    }
    
    appendLog(QString("【狂骨】对距离1以内角色造成伤害，请选择："));
    btnKuangGuHeal->setVisible(true);
    btnKuangGuDraw->setVisible(true);
    btnKuangGuSkip->setVisible(true);
    btnKuangGuHeal->setEnabled(true);
    btnKuangGuDraw->setEnabled(true);
    btnKuangGuSkip->setEnabled(true);
    
    if (target->hp <= 0) {
        wy->onKill(target);
    }
}

void MainWindow::useSlashEffect(Player* source, Player* target, bool useWeiYanSkill) {
    int damage = 1;
    if (useWeiYanSkill) {
        // 现在酒的buff在WeiYan类中处理，这里不需要额外处理
    } else if (!useWeiYanSkill && xushengWineBuffActive) {
        damage += 1;
        xushengWineBuffActive = false;
        appendLog("【酒】生效：伤害+1");
    }

    appendLog(QString("【%1】使用【杀】").arg(source->name));

    if (useWeiYanSkill) {
        weiyan->cardsUsedThisTurn++;
        bool yingZhanJiaShang = false;
        bool chengShiTriggered = false;
        bool needYingZhan = weiyan->useSlash(target, damage, yingZhanJiaShang, chengShiTriggered);

        if (yingZhanJiaShang) {
            appendLog("【饮战】势魏延体力≤目标，伤害+1");
        }
        if (chengShiTriggered) {
            appendLog("【乘势】两个条件均满足！");
        }

        if (needYingZhan) {
            showYingZhanCardSelection(target);
            return;
        } else {
            triggerKuangGu(weiyan, target, damage);
            if (target->hp <= 0) {
                appendLog(QString("【%1】进入濒死状态").arg(target->name));
                handleNearDeath(target);
            }
        }

    } else {
        xushengWineUsedThisTurn = true;
        xusheng->pojunTarget = target;
        xusheng->pojunKouZhiCount = target->hp;
        appendLog("【破军】请等待对方响应...");
        showPojunDodgeOption(target);
    }
}

void MainWindow::showPojunDodgeOption(Player* target) {
    showPojunCardSelection(target);
}

void MainWindow::resolvePojunSelect() {
    if (flowState != FlowState::POJUN_SELECT) return;
    if (!xusheng->pojunTarget) return;

    Player* target = xusheng->pojunTarget;

    if (!pojunSelectedIndices.empty()) {
        std::sort(pojunSelectedIndices.rbegin(), pojunSelectedIndices.rend());
        for (int idx : pojunSelectedIndices) {
            if (idx >= 0 && idx < (int)target->handCards.size()) {
                xusheng->kouZhiCards.push_back(target->handCards[idx]);
                target->handCards.erase(target->handCards.begin() + idx);
            }
        }
        appendLog(QString("【破军】扣置%1张牌").arg(pojunSelectedIndices.size()));
    }

    pojunSelectedIndices.clear();

    appendLog("【破军】判断是否响应...");
    bool hasShan = false;
    int shanIndex = -1;
    for (size_t i = 0; i < target->handCards.size(); ++i) {
        if (target->handCards[i]->getName() == "闪") {
            hasShan = true;
            shanIndex = i;
            break;
        }
    }

    if (hasShan && shanIndex >= 0) {
        appendLog(QString("%1使用【闪】响应！").arg(target->name));
        target->handCards.erase(target->handCards.begin() + shanIndex);
        appendLog(QString("【%1】闪避成功，受到0点伤害").arg(target->name));
        refreshHandCards();
        updateUI();
        
        enterFlowState(FlowState::TURN_PLAY_SELECT);
        lblPhase->setText("出牌阶段 - 界·徐盛");
    } else {
        appendLog(QString("%1没有【闪】，无法响应！").arg(target->name));
        QTimer::singleShot(500, this, [this, target]() {
            resolvePojunNoDodge(target);
        });
    }
}

void MainWindow::resolvePojunNoDodge(Player* target) {
    if (!xusheng->pojunTarget) return;
    if (!target) target = xusheng->pojunTarget;

    int myCards = xusheng->handCards.size();
    int myEquips = 0;
    if (xusheng->equipment.weapon) myEquips++;
    if (xusheng->equipment.armor) myEquips++;
    if (xusheng->equipment.plusHorse) myEquips++;
    if (xusheng->equipment.minusHorse) myEquips++;

    int targetCards = target->handCards.size();
    int targetEquips = 0;
    if (target->equipment.weapon) targetEquips++;
    if (target->equipment.armor) targetEquips++;
    if (target->equipment.plusHorse) targetEquips++;
    if (target->equipment.minusHorse) targetEquips++;

    int damage = 1;
    if (targetCards <= myCards && targetEquips <= myEquips) {
        damage = 2;
        appendLog(QString("【破军】徐盛手牌数(%1)%2%3张，装备数(%4)%5%6件")
            .arg(myCards).arg(myCards >= targetCards ? "≥" : "<").arg(targetCards)
            .arg(myEquips).arg(myEquips >= targetEquips ? "≥" : "<").arg(targetEquips));
        appendLog("【破军】伤害+1！");
    } else {
        appendLog(QString("【破军】徐盛手牌数%1%2%3张，装备数%4%5%6件")
            .arg(myCards).arg(myCards >= targetCards ? "≥" : "<").arg(targetCards)
            .arg(myEquips).arg(myEquips >= targetEquips ? "≥" : "<").arg(targetEquips));
    }

    target->takeDamage(damage);
    appendLog(QString("【%1】受到%2点伤害，当前体力：%3").arg(target->name).arg(damage).arg(target->hp));

    if (target->hp <= 0) {
        discardPileCount += target->kouZhiCards.size();
        target->kouZhiCards.clear();
        appendLog("【破军】目标死亡，扣置的牌置入弃牌堆");
        discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
        appendLog(QString("【%1】进入濒死状态").arg(target->name));
        handleNearDeath(target);
    }

    xusheng->pojunTarget = nullptr;
    xusheng->pojunKouZhiCount = 0;
    enterFlowState(FlowState::TURN_PLAY_SELECT);
    refreshHandCards();
    updateUI();
}

void MainWindow::resolvePojunDodge() {
    if (!xusheng->pojunTarget) return;
    Player* target = xusheng->pojunTarget;

    appendLog(QString("%1使用【闪】响应，伤害取消！").arg(target->name));
    appendLog("【破军】扣置的牌保留到回合结束");

    xusheng->pojunTarget = nullptr;
    xusheng->pojunKouZhiCount = 0;
    enterFlowState(FlowState::TURN_PLAY_SELECT);
    refreshHandCards();
    updateUI();
}

void MainWindow::enterFlowState(FlowState state) {
    flowState = state;
}

Player* MainWindow::currentPlayer() const {
    return isWeiYanTurn ? static_cast<Player*>(weiyan) : static_cast<Player*>(xusheng);
}

Player* MainWindow::enemyPlayer() const {
    return isWeiYanTurn ? static_cast<Player*>(xusheng) : static_cast<Player*>(weiyan);
}

void MainWindow::showCardOnTable(QLabel* slot, const QString& cardName) {
    QString imgPath = getCardImagePath(cardName);
    QPixmap pixmap(imgPath);
    if (!pixmap.isNull()) {
        slot->setPixmap(pixmap.scaled(100, 146, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        slot->setText(cardName);
    }
    slot->setVisible(true);
}

void MainWindow::updateSkillPanelVisibility() {
    bool isDiscardPhase = flowState == FlowState::DISCARD_PHASE;
    bool isZhuangShiPhase = flowState == FlowState::ZHUANGSHI_SELECT || 
                            flowState == FlowState::ZHUANGSHI_SELECT_CARDS || 
                            flowState == FlowState::ZHUANGSHI_SELECT_HP ||
                            flowState == FlowState::ZHUANGSHI_DISCARD;

    btnZhuangShi->setVisible(isWeiYanTurn && !zhuangShiUsed && weiyan->zhongAoState == MISSION_ACTIVE && !isDiscardPhase && !isZhuangShiPhase);
    btnZhuangShi->setEnabled(isWeiYanTurn && !zhuangShiUsed && weiyan->zhongAoState == MISSION_ACTIVE && weiyan->handCards.size() > 0);

    btnBeiShui->setVisible(isWeiYanTurn && weiyan->kuangGuLevel >= 2 && !beiShuiUsed && !isDiscardPhase && !isZhuangShiPhase && weiyan->handCards.size() > 0);
    btnBeiShui->setEnabled(isWeiYanTurn && weiyan->kuangGuLevel >= 2 && !beiShuiUsed && weiyan->handCards.size() > 0);

    btnEndTurn->setVisible(!isDiscardPhase && !isZhuangShiPhase);
    btnEndTurn->setEnabled(!isDiscardPhase && !isZhuangShiPhase);
}

void MainWindow::onZhuangShiConfirm() {
    bool ok;
    int hpLossCount = editZhuangShiHpLoss->text().toInt(&ok);
    if (!ok || hpLossCount < 0 || hpLossCount >= weiyan->hp) {
        appendLog("请输入有效的掉血数量（0到" + QString::number(weiyan->hp - 1) + "之间）！");
        return;
    }
    int discardCount = (int)zhuangShiSelectedCards.size();
    
    if (discardCount == 0 && hpLossCount == 0) {
        // 使命失败
        appendLog("【壮誓】既不弃牌也不掉血，使命失败！失去壮誓，获得困奋");
        weiyan->skipZhuangShi();
        zhuangShiUsed = true;
        zhuangShiPanel->setVisible(false);
        enterFlowState(FlowState::TURN_PLAY_SELECT);
        updateSkillPanelVisibility();
        updateConfirmState();
        refreshHandCards();
        updateUI();
        return;
    }
    
    zhuangShiUsed = true;
    zhuangShiPanel->setVisible(false);
    
    if (discardCount > 0) {
        // 执行弃牌
        std::sort(zhuangShiSelectedCards.rbegin(), zhuangShiSelectedCards.rend());
        for (int idx : zhuangShiSelectedCards) {
            if (idx < (int)weiyan->handCards.size()) {
                weiyan->handCards.erase(weiyan->handCards.begin() + idx);
                discardPileCount++;
            }
        }
        discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
        weiyan->zhuangShiDiscarded = discardCount;
        appendLog(QString("【壮誓】弃置%1张牌，本回合前%1张牌【无距离限制且不可响应】").arg(discardCount));
    }
    
    if (hpLossCount > 0) {
        // 执行掉血
        weiyan->zhuangShiHpLost = hpLossCount;
        weiyan->loseHp(hpLossCount);
        appendLog(QString("【壮誓】失去%1点体力，本回合有%1张牌【不计入次数】").arg(hpLossCount));
        
        if (weiyan->hp <= 0) {
            handleNearDeath(weiyan);
        }
    }
    
    weiyan->zhuangShiUsedThisTurn = true;
    weiyan->zhuangShiNoLimitRemaining = discardCount + hpLossCount;
    weiyan->zhuangShiNoLimitUsed = 0;
    
    if (weiyan->hp > 0) {
        enterFlowState(FlowState::TURN_PLAY_SELECT);
    }
    updateSkillPanelVisibility();
    updateConfirmState();
    refreshHandCards();
    updateUI();
}

void MainWindow::onZhuangShiCancel() {
    weiyan->skipZhuangShi();
    zhuangShiUsed = true;
    zhuangShiPanel->setVisible(false);
    selectedDiscardIndices.clear();
    enterFlowState(FlowState::TURN_PLAY_SELECT);
    appendLog("【壮誓】取消，使命失败！失去壮誓，获得困奋");
    updateSkillPanelVisibility();
    updateConfirmState();
    refreshHandCards();
    updateUI();
}

void MainWindow::onStartGame() {
    resetGame();
    
    gDeck = deck;
    deck->shuffle();
    for (int i = 0; i < 4; ++i) {
        weiyan->drawCard(deck->draw());
        xusheng->drawCard(deck->draw());
    }
    
    isWeiYanTurn = true;
    zhuangShiUsed = false;
    beiShuiUsed = false;
    weiyanWineUsedThisTurn = false;
    weiyanWineBuffActive = false;
    xushengWineUsedThisTurn = false;
    xushengWineBuffActive = false;
    discardPileCount = 0;
    discardPileLabel->setText("弃牌堆：0");
    
    btnStart->setVisible(false);
    startWeiYanTurn();
}

void MainWindow::resetGame() {
    delete weiyan;
    delete xusheng;
    delete deck;
    
    weiyan = new WeiYan();
    xusheng = new XuSheng();
    deck = new DeckManager();
    
    heroWeiYan->setHp(5);
    heroXuSheng->setHp(4);
    
    flowState = FlowState::START_MENU;
    isWeiYanTurn = true;
    zhuangShiUsed = false;
    beiShuiUsed = false;
    selectedHandCardIndex = -1;
    pendingCardName.clear();
    selectedDiscardIndices.clear();
    inKunFenNearDeath = false;
    
    weiyanWineBuffActive = false;
    xushengWineBuffActive = false;
    
    yingZhanTarget = nullptr;
    yingZhanSelectedIndex = -1;
    
    btnKuangGuHeal->setVisible(false);
    btnKuangGuDraw->setVisible(false);
    btnKuangGuSkip->setVisible(false);
}

void MainWindow::startWeiYanTurn() {
    if (!xusheng->kouZhiCards.empty()) {
        appendLog(QString("【破军】回合结束，%1张扣置的牌归还给魏延").arg(xusheng->kouZhiCards.size()));
        for (auto& card : xusheng->kouZhiCards) {
            weiyan->handCards.push_back(card);
        }
        xusheng->kouZhiCards.clear();
    }

    isWeiYanTurn = true;
    zhuangShiUsed = false;
    beiShuiUsed = false;
    weiyan->startTurn();
    weiyanWineUsedThisTurn = false;
    weiyanWineBuffActive = false;
    zhuangShiSelectedCards.clear();
    editZhuangShiHpLoss->setText("0");
    
    lblPhase->setText("出牌阶段 - 势·魏延");
    lblPhase->setStyleSheet("font-size: 20px; font-weight: bold; color: #F44336;");
    
    weiyan->drawCard(deck->draw());
    weiyan->drawCard(deck->draw());
    updateDeckDisplay();
    appendLog("=== 势·魏延 回合 ===");
    appendLog("势·魏延摸2张牌");
    
    if (weiyan->zhongAoState == MISSION_ACTIVE) {
        enterFlowState(FlowState::ZHUANGSHI_SELECT);
        zhuangShiPanel->setVisible(true);
        lblZhuangShiStep->setText("【壮誓】请在下方选择要弃置的手牌，然后输入掉血数量");
        editZhuangShiHpLoss->setText("0");
        btnConfirm->setVisible(false);
        btnCancel->setVisible(false);
        btnEndTurn->setVisible(false);
        appendLog("【壮誓】请直接点击手牌选择要弃置的牌（可多选），然后输入掉血数量");
    } else {
        enterFlowState(FlowState::TURN_PLAY_SELECT);
    }
    
    refreshHandCards();
    updateSkillPanelVisibility();
    updateConfirmState();
    updateUI();
}

void MainWindow::startXuShengTurn() {
    isWeiYanTurn = false;
    xusheng->slashUsedThisTurn = 0;
    xusheng->slashCountLimit = 1;
    xushengWineUsedThisTurn = false;
    xushengWineBuffActive = false;

    lblPhase->setText("出牌阶段 - 界·徐盛");
    lblPhase->setStyleSheet("font-size: 20px; font-weight: bold; color: #2196F3;");

    xusheng->drawCard(deck->draw());
    xusheng->drawCard(deck->draw());
    updateDeckDisplay();
    appendLog("=== 界·徐盛 回合 ===");
    appendLog("界·徐盛摸2张牌");

    enterFlowState(FlowState::TURN_PLAY_SELECT);
    refreshHandCards();
    updateSkillPanelVisibility();
    updateUI();
}

void MainWindow::checkGameOver() {
    if (weiyan->hp <= 0) {
        lblPhase->setText("游戏结束 - 界·徐盛 获胜！");
        lblPhase->setStyleSheet("font-size: 24px; font-weight: bold; color: #2196F3;");
        QMessageBox::information(this, "游戏结束", "界·徐盛 获胜！");
        btnStart->setVisible(true);
        btnZhuangShi->setVisible(false);
        btnBeiShui->setVisible(false);
        btnEndTurn->setVisible(false);
        btnKuangGuHeal->setVisible(false);
        btnKuangGuDraw->setVisible(false);
        btnKuangGuSkip->setVisible(false);
    } else if (xusheng->hp <= 0) {
        lblPhase->setText("游戏结束 - 势·魏延 获胜！");
        lblPhase->setStyleSheet("font-size: 24px; font-weight: bold; color: #F44336;");
        QMessageBox::information(this, "游戏结束", "势·魏延 获胜！");
        btnStart->setVisible(true);
        btnZhuangShi->setVisible(false);
        btnBeiShui->setVisible(false);
        btnEndTurn->setVisible(false);
        btnKuangGuHeal->setVisible(false);
        btnKuangGuDraw->setVisible(false);
        btnKuangGuSkip->setVisible(false);
    }
}

void MainWindow::updateUI() {
    heroWeiYan->setHp(weiyan->hp);
    heroXuSheng->setHp(xusheng->hp);
    txtLog->ensureCursorVisible();
}

void MainWindow::appendLog(const QString& text) {
    txtLog->append(text);
}

void MainWindow::onWeiYanZhuangShi() {
    if (weiyan->zhongAoState != MISSION_ACTIVE) {
        appendLog("壮誓已失败，无法使用！");
        return;
    }
    zhuangShiPanel->setVisible(true);
    zhuangShiSelectedCards.clear();
    editZhuangShiHpLoss->setText("0");
    lblZhuangShiStep->setText("【壮誓】请在下方选择要弃置的手牌，然后输入掉血数量");
    btnZhuangShi->setVisible(false);
    btnBeiShui->setVisible(false);
    btnEndTurn->setVisible(false);
    btnConfirm->setVisible(true);
    btnCancel->setVisible(true);
    enterFlowState(FlowState::ZHUANGSHI_SELECT);
    appendLog("【壮誓】请直接点击手牌选择要弃置的牌（可多选），然后输入掉血数量");
}

void MainWindow::onWeiYanBeiShui() {
    if (weiyan->kuangGuLevel < 2) {
        appendLog("狂骨未升级，无法使用背水！");
        return;
    }
    if (weiyan->handCards.empty()) {
        appendLog("没有手牌可弃置！");
        return;
    }
    
    auto card = weiyan->handCards.back();
    weiyan->handCards.pop_back();
    weiyan->slashCountLimit++;
    beiShuiUsed = true;
    discardPileCount++;
    discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
    
    appendLog(QString("【背水】弃置【%1】，本回合杀次数+1").arg(card->name));
    
    refreshHandCards();
    updateSkillPanelVisibility();
    updateUI();
}

void MainWindow::onEndTurn() {
    enterFlowState(FlowState::TURN_END);
    if (isWeiYanTurn) {
        weiyan->endTurn();

        if (weiyan->zhongAoState == MISSION_FAILED) {
            selectedDiscardIndices.clear();
            lblPhase->setText("结束阶段 - 请弃牌");
            lblPhase->setStyleSheet("font-size: 18px; font-weight: bold; color: #FF9800;");
            btnZhuangShi->setVisible(false);
            btnBeiShui->setVisible(false);
            btnEndTurn->setVisible(false);
            btnKuangGuHeal->setVisible(false);
            btnKuangGuDraw->setVisible(false);
            btnKuangGuSkip->setVisible(false);
            discardPhaseCount = 0;
            enterFlowState(FlowState::KUNFEN_END_PHASE);
            appendLog("结束阶段：势·魏延弃牌至体力值");
            refreshHandCards();
            updateConfirmState();
            return;
        }

        if (weiyan->zhongAoState == MISSION_SUCCESS) {
            if (weiyan->cardsUsedThisTurn < weiyan->zhuangShiDiscarded) {
                auto card = deck->draw();
                if (card) {
                    weiyan->drawCard(card);
                    appendLog("【忠傲·成功】使用牌数<壮誓弃牌数，摸1牌");
                }
            }
            if (weiyan->cardsUsedThisTurn < weiyan->zhuangShiHpLost) {
                if (weiyan->hp < weiyan->maxHp) {
                    weiyan->heal(1);
                    appendLog("【忠傲·成功】使用牌数<壮誓掉血数，回1血");
                } else {
                    auto card = deck->draw();
                    if (card) {
                        weiyan->drawCard(card);
                        appendLog("【忠傲·成功】使用牌数<壮誓掉血数（体力满），摸1牌");
                    }
                }
            }
            updateDeckDisplay();
        }

        if (weiyan->hp <= 0) {
            checkGameOver();
            return;
        }

        startDiscardPhase(weiyan);
    } else {
        // 界·徐盛回合结束，将扣置的牌还给魏延
        if (!xusheng->kouZhiCards.empty()) {
            appendLog(QString("【破军】归还%1张牌给势·魏延").arg(xusheng->kouZhiCards.size()));
            for (auto& card : xusheng->kouZhiCards) {
                weiyan->handCards.push_back(card);
            }
            xusheng->kouZhiCards.clear();
        }
        startDiscardPhase(xusheng);
    }
    updateUI();
}

void MainWindow::updateEquipmentDisplay() {
    if (isWeiYanTurn) {
        lblHandCardCount->setText(QString("手牌：%1").arg(weiyan->handCards.size()));
        lblWeiYanWeapon->setText(weiyan->equipment.weapon ? QString("武器：%1").arg(weiyan->equipment.weapon->name) : "武器：无");
        lblWeiYanArmor->setText(weiyan->equipment.armor ? QString("防具：%1").arg(weiyan->equipment.armor->name) : "防具：无");
        QString horseText = "坐骑：";
        if (weiyan->equipment.plusHorse) horseText += QString("%1 ").arg(weiyan->equipment.plusHorse->name);
        if (weiyan->equipment.minusHorse) horseText += QString("%1 ").arg(weiyan->equipment.minusHorse->name);
        if (horseText == "坐骑：") horseText += "无";
        lblWeiYanHorse->setText(horseText);

        lblXuShengHandCardCount->setText(QString("手牌：%1").arg(xusheng->handCards.size()));
        lblXuShengWeapon->setText(xusheng->equipment.weapon ? QString("武器：%1").arg(xusheng->equipment.weapon->name) : "武器：无");
        lblXuShengArmor->setText(xusheng->equipment.armor ? QString("防具：%1").arg(xusheng->equipment.armor->name) : "防具：无");
        QString xushengHorseText = "坐骑：";
        if (xusheng->equipment.plusHorse) xushengHorseText += QString("%1 ").arg(xusheng->equipment.plusHorse->name);
        if (xusheng->equipment.minusHorse) xushengHorseText += QString("%1 ").arg(xusheng->equipment.minusHorse->name);
        if (xushengHorseText == "坐骑：") xushengHorseText += "无";
        lblXuShengHorse->setText(xushengHorseText);
    } else {
        lblHandCardCount->setText(QString("手牌：%1").arg(xusheng->handCards.size()));
        lblWeiYanWeapon->setText(weiyan->equipment.weapon ? QString("武器：%1").arg(weiyan->equipment.weapon->name) : "武器：无");
        lblWeiYanArmor->setText(weiyan->equipment.armor ? QString("防具：%1").arg(weiyan->equipment.armor->name) : "防具：无");
        QString horseText = "坐骑：";
        if (weiyan->equipment.plusHorse) horseText += QString("%1 ").arg(weiyan->equipment.plusHorse->name);
        if (weiyan->equipment.minusHorse) horseText += QString("%1 ").arg(weiyan->equipment.minusHorse->name);
        if (horseText == "坐骑：") horseText += "无";
        lblWeiYanHorse->setText(horseText);

        lblXuShengHandCardCount->setText(QString("手牌：%1").arg(xusheng->handCards.size()));
        lblXuShengWeapon->setText(xusheng->equipment.weapon ? QString("武器：%1").arg(xusheng->equipment.weapon->name) : "武器：无");
        lblXuShengArmor->setText(xusheng->equipment.armor ? QString("防具：%1").arg(xusheng->equipment.armor->name) : "防具：无");
        QString xushengHorseText = "坐骑：";
        if (xusheng->equipment.plusHorse) xushengHorseText += QString("%1 ").arg(xusheng->equipment.plusHorse->name);
        if (xusheng->equipment.minusHorse) xushengHorseText += QString("%1 ").arg(xusheng->equipment.minusHorse->name);
        if (xushengHorseText == "坐骑：") xushengHorseText += "无";
        lblXuShengHorse->setText(xushengHorseText);
    }
}

void MainWindow::updateDeckDisplay() {
    if (!deck) return;
    int deckCount = deck->deck.size();
    lblDeckCount->setText(QString::number(deckCount));
    
    if (deckCount == 0) {
        appendLog("牌堆已空，重新洗牌...");
        for (int i = 0; i < 50; ++i) {
            auto card = deck->draw();
            if (!card) break;
        }
        deck->shuffle();
        deckCount = deck->deck.size();
        lblDeckCount->setText(QString::number(deckCount));
        appendLog(QString("牌堆重新洗牌，当前牌数：%1").arg(deckCount));
    }
}

void MainWindow::startDiscardPhase(Player* player) {
    int excessCards = player->handCards.size() - player->hp;
    if (excessCards <= 0) {
        if (isWeiYanTurn) {
            startXuShengTurn();
        } else {
            startWeiYanTurn();
        }
        return;
    }

    discardPhaseCount = excessCards;
    selectedDiscardIndices.clear();

    lblPhase->setText(QString("弃牌阶段 - %1").arg(player->name));
    lblPhase->setStyleSheet("font-size: 18px; font-weight: bold; color: #FF9800;");

    btnZhuangShi->setVisible(false);
    btnBeiShui->setVisible(false);
    btnEndTurn->setVisible(false);
    btnKuangGuHeal->setVisible(false);
    btnKuangGuDraw->setVisible(false);
    btnKuangGuSkip->setVisible(false);

    enterFlowState(FlowState::DISCARD_PHASE);
    appendLog(QString(">>> 弃牌阶段：请选择弃置%1张牌").arg(excessCards));
    refreshHandCards();
}

void MainWindow::onDiscardPhaseConfirm() {
    Player* player = isWeiYanTurn ? static_cast<Player*>(weiyan) : static_cast<Player*>(xusheng);
    
    if ((int)selectedDiscardIndices.size() != discardPhaseCount) {
        appendLog(QString("请选择%1张牌！").arg(discardPhaseCount));
        return;
    }
    
    std::sort(selectedDiscardIndices.rbegin(), selectedDiscardIndices.rend());
    for (int idx : selectedDiscardIndices) {
        if (idx < (int)player->handCards.size()) {
            player->handCards.erase(player->handCards.begin() + idx);
            discardPileCount++;
        }
    }
    discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
    
    selectedDiscardIndices.clear();
    btnConfirm->setVisible(false);
    btnCancel->setVisible(false);
    
    if (isWeiYanTurn) {
        startXuShengTurn();
    } else {
        startWeiYanTurn();
    }
}

void MainWindow::onKunFenEndPhaseConfirm() {
    int needDiscard = (int)weiyan->handCards.size() - weiyan->hp;
    if (needDiscard < 0) needDiscard = 0;

    if ((int)selectedDiscardIndices.size() != needDiscard) {
        appendLog(QString("请选择弃置%1张牌！").arg(needDiscard));
        return;
    }

    std::sort(selectedDiscardIndices.rbegin(), selectedDiscardIndices.rend());
    for (int idx : selectedDiscardIndices) {
        if (idx < (int)weiyan->handCards.size()) {
            weiyan->handCards.erase(weiyan->handCards.begin() + idx);
            discardPileCount++;
        }
    }
    discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));

    appendLog(QString("结束阶段：势·魏延弃置%1张牌").arg(needDiscard));
    selectedDiscardIndices.clear();
    btnConfirm->setVisible(false);
    btnCancel->setVisible(false);

    appendLog("【困奋】失去1点体力");
    weiyan->loseHp(1);

    if (weiyan->hp <= 0) {
        inKunFenNearDeath = true;
        handleNearDeath(weiyan);
        return;
    }

    inKunFenNearDeath = false;
    auto card1 = deck->draw();
    if (card1) weiyan->drawCard(card1);
    auto card2 = deck->draw();
    if (card2) weiyan->drawCard(card2);
    appendLog("【困奋】摸2张牌");
    updateDeckDisplay();

    refreshHandCards();
    updateUI();

    startXuShengTurn();
}

void MainWindow::handleNearDeath(Player* player) {
    if (player->hp > 0) return;
    
    appendLog(QString("【%1】进入濒死状态！体力：%2").arg(player->name).arg(player->hp));
    
    if (isWeiYanTurn && player == weiyan) {
        weiyan->skipZhuangShi();
        appendLog("【忠傲】进入濒死，使命失败！失去壮誓，获得困奋");
    }
    
    // 自动自救：优先用酒，没有酒用桃
    bool saved = false;
    
    // 1. 先找酒
    for (auto it = player->handCards.begin(); it != player->handCards.end(); ++it) {
        if ((*it)->getName() == "酒") {
            player->handCards.erase(it);
            player->heal(1);
            appendLog(QString("【%1】濒死使用【酒】自救，回复1点体力，当前体力：%2").arg(player->name).arg(player->hp));
            saved = true;
            break;
        }
    }
    
    // 2. 如果没有酒，找桃
    if (!saved) {
        for (auto it = player->handCards.begin(); it != player->handCards.end(); ++it) {
            if ((*it)->getName() == "桃") {
                player->handCards.erase(it);
                player->heal(1);
                appendLog(QString("【%1】濒死使用【桃】自救，回复1点体力，当前体力：%2").arg(player->name).arg(player->hp));
                saved = true;
                break;
            }
        }
    }
    
    if (saved) {
        appendLog(QString("【%1】濒死获救！").arg(player->name));
        refreshHandCards();
        updateUI();
        
        if (inKunFenNearDeath) {
            inKunFenNearDeath = false;
            auto card1 = deck->draw();
            if (card1) weiyan->drawCard(card1);
            auto card2 = deck->draw();
            if (card2) weiyan->drawCard(card2);
            appendLog("【困奋】摸2张牌");
            updateDeckDisplay();
            
            refreshHandCards();
            updateUI();
            
            startXuShengTurn();
        } else {
            enterFlowState(FlowState::TURN_PLAY_SELECT);
        }
    } else {
        // 没救了，阵亡
        appendLog(QString("【%1】无法自救，死亡！").arg(player->name));
        player->hp = 0;
        checkGameOver();
    }
}

void MainWindow::onWeiYanKuangGuHeal() {
    if (!weiyan) return;
    if (weiyan->hp < weiyan->maxHp) {
        weiyan->heal(1);
        appendLog("【狂骨】回复1点体力");
    } else {
        auto card = deck->draw();
        if (card) weiyan->drawCard(card);
        appendLog("【狂骨】体力已满，摸1张牌");
        updateDeckDisplay();
        refreshHandCards();
    }
    btnKuangGuHeal->setVisible(false);
    btnKuangGuDraw->setVisible(false);
    btnKuangGuSkip->setVisible(false);
    updateUI();
}

void MainWindow::onWeiYanKuangGuDraw() {
    if (!weiyan) return;
    auto card = deck->draw();
    if (card) weiyan->drawCard(card);
    appendLog("【狂骨】摸1张牌");
    updateDeckDisplay();
    btnKuangGuHeal->setVisible(false);
    btnKuangGuDraw->setVisible(false);
    btnKuangGuSkip->setVisible(false);
    refreshHandCards();
    updateUI();
}

void MainWindow::onWeiYanKuangGuSkip() {
    appendLog("【狂骨】不发动");
    btnKuangGuHeal->setVisible(false);
    btnKuangGuDraw->setVisible(false);
    btnKuangGuSkip->setVisible(false);
}

void MainWindow::showSkillAnimation(const QString& skillName, const QString& heroName) {
    Q_UNUSED(skillName);
    Q_UNUSED(heroName);
}

void MainWindow::showTargetCardSelection(Player* target, const QString& cardName) {
    qDeleteAll(targetCardWidgets);
    targetCardWidgets.clear();

    QLayoutItem* item;
    while ((item = ((QVBoxLayout*)targetSelectPanel->layout())->takeAt(1)) != nullptr) {
        delete item->widget();
        delete item;
    }

    QHBoxLayout* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(10);

    if (!target->handCards.empty()) {
        for (size_t i = 0; i < target->handCards.size(); ++i) {
            CardWidget* cardWidget = new CardWidget(target->handCards[i]->getName(), i);
            QString imgPath = getCardImagePath("卡牌背面");
            QPixmap pixmap(imgPath);
            if (!pixmap.isNull()) {
                cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                cardWidget->setText("[手牌]");
            }
            cardWidget->setToolTip("手牌");
            connect(cardWidget, &CardWidget::clicked, this, &MainWindow::onTargetCardClicked);
            targetCardWidgets.append(cardWidget);
            cardsLayout->addWidget(cardWidget);
        }
    }

    if (target->equipment.weapon) {
        CardWidget* cardWidget = new CardWidget(target->equipment.weapon->getName(), 100);
        QString imgPath = getCardImagePath(target->equipment.weapon->getName());
        QPixmap pixmap(imgPath);
        if (!pixmap.isNull()) {
            cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(target->equipment.weapon->name);
        }
        cardWidget->index = 100;
        connect(cardWidget, &CardWidget::clicked, this, &MainWindow::onTargetCardClicked);
        targetCardWidgets.append(cardWidget);
        cardsLayout->addWidget(cardWidget);
    }

    if (target->equipment.armor) {
        CardWidget* cardWidget = new CardWidget(target->equipment.armor->getName(), 101);
        QString imgPath = getCardImagePath(target->equipment.armor->getName());
        QPixmap pixmap(imgPath);
        if (!pixmap.isNull()) {
            cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(target->equipment.armor->name);
        }
        cardWidget->index = 101;
        connect(cardWidget, &CardWidget::clicked, this, &MainWindow::onTargetCardClicked);
        targetCardWidgets.append(cardWidget);
        cardsLayout->addWidget(cardWidget);
    }

    if (target->equipment.plusHorse) {
        CardWidget* cardWidget = new CardWidget(target->equipment.plusHorse->getName(), 102);
        QString imgPath = getCardImagePath(target->equipment.plusHorse->getName());
        QPixmap pixmap(imgPath);
        if (!pixmap.isNull()) {
            cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(target->equipment.plusHorse->name);
        }
        cardWidget->index = 102;
        connect(cardWidget, &CardWidget::clicked, this, &MainWindow::onTargetCardClicked);
        targetCardWidgets.append(cardWidget);
        cardsLayout->addWidget(cardWidget);
    }

    if (target->equipment.minusHorse) {
        CardWidget* cardWidget = new CardWidget(target->equipment.minusHorse->getName(), 103);
        QString imgPath = getCardImagePath(target->equipment.minusHorse->getName());
        QPixmap pixmap(imgPath);
        if (!pixmap.isNull()) {
            cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(target->equipment.minusHorse->name);
        }
        cardWidget->index = 103;
        connect(cardWidget, &CardWidget::clicked, this, &MainWindow::onTargetCardClicked);
        targetCardWidgets.append(cardWidget);
        cardsLayout->addWidget(cardWidget);
    }

    ((QVBoxLayout*)targetSelectPanel->layout())->addLayout(cardsLayout);
    targetSelectPanel->setVisible(true);
    targetSelectPanel->raise();

    lblTargetSelect->setText(QString("【%1】请选择要%2的牌").arg(cardName == "顺手牵羊" ? "顺手牵羊" : "过河拆桥").arg(cardName == "顺手牵羊" ? "牵走" : "拆"));
    currentTrickCardName = cardName;
    enterFlowState(FlowState::SELECT_TRICK_TARGET);

    btnConfirm->setVisible(false);
    btnCancel->setVisible(false);
}

void MainWindow::onTargetCardClicked(CardWidget* card) {
    Player* source = currentPlayer();
    Player* target = enemyPlayer();

    if (currentTrickCardName == "顺手牵羊") {
        if (card->index < 100) {
            source->handCards.push_back(target->handCards[card->index]);
            target->handCards.erase(target->handCards.begin() + card->index);
            appendLog(QString("【顺手牵羊】获得【%1】的手牌【%2】").arg(target->name).arg(card->cardName));
        } else if (card->index == 100 && target->equipment.weapon) {
            source->handCards.push_back(target->equipment.weapon);
            target->equipment.weapon.reset();
            appendLog(QString("【顺手牵羊】获得【%1】的武器【%2】").arg(target->name).arg(card->cardName));
        } else if (card->index == 101 && target->equipment.armor) {
            source->handCards.push_back(target->equipment.armor);
            target->equipment.armor.reset();
            appendLog(QString("【顺手牵羊】获得【%1】的防具【%2】").arg(target->name).arg(card->cardName));
        } else if (card->index == 102 && target->equipment.plusHorse) {
            source->handCards.push_back(target->equipment.plusHorse);
            target->equipment.plusHorse.reset();
            appendLog(QString("【顺手牵羊】获得【%1】的+1马【%2】").arg(target->name).arg(card->cardName));
        } else if (card->index == 103 && target->equipment.minusHorse) {
            source->handCards.push_back(target->equipment.minusHorse);
            target->equipment.minusHorse.reset();
            appendLog(QString("【顺手牵羊】获得【%1】的-1马【%2】").arg(target->name).arg(card->cardName));
        }
    } else if (currentTrickCardName == "过河拆桥") {
        if (card->index < 100) {
            target->handCards.erase(target->handCards.begin() + card->index);
            discardPileCount++;
            discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
            appendLog(QString("【过河拆桥】弃置【%1】的手牌【%2】").arg(target->name).arg(card->cardName));
        } else if (card->index == 100 && target->equipment.weapon) {
            target->equipment.weapon.reset();
            discardPileCount++;
            discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
            appendLog(QString("【过河拆桥】弃置【%1】的武器【%2】").arg(target->name).arg(card->cardName));
        } else if (card->index == 101 && target->equipment.armor) {
            target->equipment.armor.reset();
            discardPileCount++;
            discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
            appendLog(QString("【过河拆桥】弃置【%1】的防具【%2】").arg(target->name).arg(card->cardName));
        } else if (card->index == 102 && target->equipment.plusHorse) {
            target->equipment.plusHorse.reset();
            discardPileCount++;
            discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
            appendLog(QString("【过河拆桥】弃置【%1】的+1马【%2】").arg(target->name).arg(card->cardName));
        } else if (card->index == 103 && target->equipment.minusHorse) {
            target->equipment.minusHorse.reset();
            discardPileCount++;
            discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
            appendLog(QString("【过河拆桥】弃置【%1】的-1马【%2】").arg(target->name).arg(card->cardName));
        }
    }

    targetSelectPanel->setVisible(false);
    currentTrickCardName.clear();
    enterFlowState(FlowState::TURN_PLAY_SELECT);
    refreshHandCards();
    updateUI();
}

void MainWindow::clearTableCards() {
    tableCardLeft->setVisible(false);
    tableCardRight->setVisible(false);
}

void MainWindow::showPojunCardSelection(Player* target) {
    qDeleteAll(pojunCardWidgets);
    pojunCardWidgets.clear();

    QLayoutItem* item;
    while ((item = ((QVBoxLayout*)pojunSelectPanel->layout())->takeAt(1)) != nullptr) {
        delete item->widget();
        delete item;
    }

    QHBoxLayout* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(10);

    int kouZhiLimit = target->hp;
    lblPojunInfo->setText(QString("【破军】请选择要扣置的牌（0-%1张）").arg(kouZhiLimit));

    if (!target->handCards.empty()) {
        for (size_t i = 0; i < target->handCards.size(); ++i) {
            CardWidget* cardWidget = new CardWidget(target->handCards[i]->getName(), i);
            QString imgPath = getCardImagePath("卡牌背面");
            QPixmap pixmap(imgPath);
            if (!pixmap.isNull()) {
                cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                cardWidget->setText("[手牌]");
            }
            cardWidget->setToolTip("手牌");
            connect(cardWidget, &CardWidget::clicked, this, [this, cardWidget](CardWidget*) {
                if (flowState != FlowState::SELECT_POJUN_TARGET) return;
                if (!pojunTarget) return;

                int idx = cardWidget->index;

                auto it = std::find(pojunSelectedIndices.begin(), pojunSelectedIndices.end(), idx);
                if (it != pojunSelectedIndices.end()) {
                    pojunSelectedIndices.erase(it);
                    cardWidget->setStyleSheet("");
                    cardWidget->setProperty("selected", false);
                } else {
                    if ((int)pojunSelectedIndices.size() < pojunKouZhiCount) {
                        pojunSelectedIndices.push_back(idx);
                        cardWidget->setStyleSheet("QLabel { border: 3px solid #FFD700; border-radius: 8px; }");
                        cardWidget->setProperty("selected", true);
                    }
                }

                int selectedCount = pojunSelectedIndices.size();
                lblPojunInfo->setText(QString("【破军】已选择%1张牌，最多可扣%2张").arg(selectedCount).arg(pojunKouZhiCount));
                updateConfirmState();
            });
            pojunCardWidgets.append(cardWidget);
            cardsLayout->addWidget(cardWidget);
        }
    }

    ((QVBoxLayout*)pojunSelectPanel->layout())->addLayout(cardsLayout);
    pojunSelectPanel->setVisible(true);
    pojunSelectPanel->raise();

    pojunTarget = target;
    pojunKouZhiCount = kouZhiLimit;
    enterFlowState(FlowState::SELECT_POJUN_TARGET);

    btnConfirm->setVisible(true);
    btnConfirm->setText("确定扣牌");
    btnConfirm->setEnabled(true);
    btnCancel->setVisible(true);
    btnCancel->setText("跳过扣牌");
}

void MainWindow::onPojunCardClicked(CardWidget* card) {
    if (!pojunTarget) return;
    if (flowState != FlowState::SELECT_POJUN_TARGET) return;

    int idx = card->index;

    auto it = std::find(pojunSelectedIndices.begin(), pojunSelectedIndices.end(), idx);
    if (it != pojunSelectedIndices.end()) {
        pojunSelectedIndices.erase(it);
        card->setStyleSheet("");
        card->setProperty("selected", false);
    } else {
        if ((int)pojunSelectedIndices.size() < pojunKouZhiCount) {
            pojunSelectedIndices.push_back(idx);
            card->setStyleSheet("QLabel { border: 3px solid #FFD700; border-radius: 8px; }");
            card->setProperty("selected", true);
        }
    }

    int selectedCount = pojunSelectedIndices.size();
    lblPojunInfo->setText(QString("【破军】已选择%1张牌，最多可扣%2张").arg(selectedCount).arg(pojunKouZhiCount));
    updateConfirmState();
}

void MainWindow::onPojunConfirm() {
    if (!pojunTarget) return;

    if (!pojunSelectedIndices.empty()) {
        std::sort(pojunSelectedIndices.rbegin(), pojunSelectedIndices.rend());
        for (int idx : pojunSelectedIndices) {
            if (idx >= 0 && idx < (int)pojunTarget->handCards.size()) {
                xusheng->kouZhiCards.push_back(pojunTarget->handCards[idx]);
                pojunTarget->handCards.erase(pojunTarget->handCards.begin() + idx);
            }
        }
        appendLog(QString("【破军】扣置%1张牌").arg(pojunSelectedIndices.size()));
    }

    pojunSelectPanel->setVisible(false);

    appendLog("【破军】判断是否响应...");
    bool hasShan = false;
    int shanIndex = -1;
    for (size_t i = 0; i < pojunTarget->handCards.size(); ++i) {
        if (pojunTarget->handCards[i]->getName() == "闪") {
            hasShan = true;
            shanIndex = i;
            break;
        }
    }

    if (hasShan && shanIndex >= 0) {
        appendLog(QString("%1使用【闪】响应！").arg(pojunTarget->name));
        pojunTarget->handCards.erase(pojunTarget->handCards.begin() + shanIndex);
        appendLog(QString("【%1】闪避成功，受到0点伤害").arg(pojunTarget->name));
        refreshHandCards();
        updateUI();
        
        enterFlowState(FlowState::TURN_PLAY_SELECT);
        lblPhase->setText("出牌阶段 - 界·徐盛");
    } else {
        appendLog(QString("%1没有【闪】，无法响应！").arg(pojunTarget->name));
        QTimer::singleShot(500, this, [this]() {
            resolvePojunNoDodge(pojunTarget);
        });
    }
}



void MainWindow::showYingZhanCardSelection(Player* target) {
    qDeleteAll(targetCardWidgets);
    targetCardWidgets.clear();

    QLayoutItem* item;
    while ((item = ((QVBoxLayout*)targetSelectPanel->layout())->takeAt(1)) != nullptr) {
        delete item->widget();
        delete item;
    }

    QHBoxLayout* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(10);

    if (!target->handCards.empty()) {
        for (size_t i = 0; i < target->handCards.size(); ++i) {
            CardWidget* cardWidget = new CardWidget(target->handCards[i]->name, i);
            QString imgPath = getCardImagePath("卡牌背面");
            QPixmap pixmap(imgPath);
            if (!pixmap.isNull()) {
                cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                cardWidget->setText("[手牌]");
            }
            cardWidget->setToolTip("手牌");
            connect(cardWidget, &CardWidget::clicked, this, [this, cardWidget, target](CardWidget*) {
                cardWidget->selected = !cardWidget->selected;
                yingZhanTarget = target;
                yingZhanSelectedIndex = cardWidget->index;
                onYingZhanCardSelected();
            });
            targetCardWidgets.append(cardWidget);
            cardsLayout->addWidget(cardWidget);
        }
    }

    if (target->equipment.weapon) {
        CardWidget* cardWidget = new CardWidget(target->equipment.weapon->name, 100);
        QString imgPath = getCardImagePath(target->equipment.weapon->name);
        QPixmap pixmap(imgPath);
        if (!pixmap.isNull()) {
            cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(target->equipment.weapon->name);
        }
        cardWidget->setToolTip("武器");
        connect(cardWidget, &CardWidget::clicked, this, [this, cardWidget, target](CardWidget*) {
            cardWidget->selected = !cardWidget->selected;
            yingZhanTarget = target;
            yingZhanSelectedIndex = 100;
            onYingZhanCardSelected();
        });
        targetCardWidgets.append(cardWidget);
        cardsLayout->addWidget(cardWidget);
    }

    if (target->equipment.armor) {
        CardWidget* cardWidget = new CardWidget(target->equipment.armor->name, 101);
        QString imgPath = getCardImagePath(target->equipment.armor->name);
        QPixmap pixmap(imgPath);
        if (!pixmap.isNull()) {
            cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(target->equipment.armor->name);
        }
        cardWidget->setToolTip("防具");
        connect(cardWidget, &CardWidget::clicked, this, [this, cardWidget, target](CardWidget*) {
            cardWidget->selected = !cardWidget->selected;
            yingZhanTarget = target;
            yingZhanSelectedIndex = 101;
            onYingZhanCardSelected();
        });
        targetCardWidgets.append(cardWidget);
        cardsLayout->addWidget(cardWidget);
    }

    if (target->equipment.plusHorse) {
        CardWidget* cardWidget = new CardWidget(target->equipment.plusHorse->name, 102);
        QString imgPath = getCardImagePath(target->equipment.plusHorse->name);
        QPixmap pixmap(imgPath);
        if (!pixmap.isNull()) {
            cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(target->equipment.plusHorse->name);
        }
        cardWidget->setToolTip("+1马");
        connect(cardWidget, &CardWidget::clicked, this, [this, cardWidget, target](CardWidget*) {
            cardWidget->selected = !cardWidget->selected;
            yingZhanTarget = target;
            yingZhanSelectedIndex = 102;
            onYingZhanCardSelected();
        });
        targetCardWidgets.append(cardWidget);
        cardsLayout->addWidget(cardWidget);
    }

    if (target->equipment.minusHorse) {
        CardWidget* cardWidget = new CardWidget(target->equipment.minusHorse->name, 103);
        QString imgPath = getCardImagePath(target->equipment.minusHorse->name);
        QPixmap pixmap(imgPath);
        if (!pixmap.isNull()) {
            cardWidget->setPixmap(pixmap.scaled(80, 114, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(target->equipment.minusHorse->name);
        }
        cardWidget->setToolTip("-1马");
        connect(cardWidget, &CardWidget::clicked, this, [this, cardWidget, target](CardWidget*) {
            cardWidget->selected = !cardWidget->selected;
            yingZhanTarget = target;
            yingZhanSelectedIndex = 103;
            onYingZhanCardSelected();
        });
        targetCardWidgets.append(cardWidget);
        cardsLayout->addWidget(cardWidget);
    }

    ((QVBoxLayout*)targetSelectPanel->layout())->addLayout(cardsLayout);
    targetSelectPanel->setVisible(true);
    targetSelectPanel->raise();

    lblTargetSelect->setText("【饮战】请选择要弃置的牌（可选手牌或装备区）");
    enterFlowState(FlowState::SELECT_YINGZHAN_CARD);
    btnConfirm->setVisible(false);
    btnCancel->setVisible(false);
    refreshHandCards();
}

void MainWindow::onYingZhanCardSelected() {
    if (!yingZhanTarget || yingZhanSelectedIndex < 0) {
        targetSelectPanel->setVisible(false);
        enterFlowState(FlowState::TURN_PLAY_SELECT);
        refreshHandCards();
        updateUI();
        return;
    }

    std::shared_ptr<Card> discardedCard = nullptr;

    if (yingZhanSelectedIndex < 100) {
        if (yingZhanSelectedIndex >= (int)yingZhanTarget->handCards.size()) {
            appendLog("【饮战】选择的牌无效！");
            targetSelectPanel->setVisible(false);
            yingZhanSelectedIndex = -1;
            enterFlowState(FlowState::TURN_PLAY_SELECT);
            refreshHandCards();
            updateUI();
            return;
        }
        discardedCard = yingZhanTarget->handCards[yingZhanSelectedIndex];
        yingZhanTarget->handCards.erase(yingZhanTarget->handCards.begin() + yingZhanSelectedIndex);
        appendLog(QString("【饮战】弃置了【%1】的手牌【%2】").arg(yingZhanTarget->name).arg(discardedCard->name));
    } else if (yingZhanSelectedIndex == 100 && yingZhanTarget->equipment.weapon) {
        discardedCard = yingZhanTarget->equipment.weapon;
        yingZhanTarget->equipment.weapon.reset();
        appendLog(QString("【饮战】弃置了【%1】的武器【%2】").arg(yingZhanTarget->name).arg(discardedCard->name));
    } else if (yingZhanSelectedIndex == 101 && yingZhanTarget->equipment.armor) {
        discardedCard = yingZhanTarget->equipment.armor;
        yingZhanTarget->equipment.armor.reset();
        appendLog(QString("【饮战】弃置了【%1】的防具【%2】").arg(yingZhanTarget->name).arg(discardedCard->name));
    } else if (yingZhanSelectedIndex == 102 && yingZhanTarget->equipment.plusHorse) {
        discardedCard = yingZhanTarget->equipment.plusHorse;
        yingZhanTarget->equipment.plusHorse.reset();
        appendLog(QString("【饮战】弃置了【%1】的+1马【%2】").arg(yingZhanTarget->name).arg(discardedCard->name));
    } else if (yingZhanSelectedIndex == 103 && yingZhanTarget->equipment.minusHorse) {
        discardedCard = yingZhanTarget->equipment.minusHorse;
        yingZhanTarget->equipment.minusHorse.reset();
        appendLog(QString("【饮战】弃置了【%1】的-1马【%2】").arg(yingZhanTarget->name).arg(discardedCard->name));
    } else {
        appendLog("【饮战】选择的牌不存在！");
        targetSelectPanel->setVisible(false);
        yingZhanSelectedIndex = -1;
        enterFlowState(FlowState::TURN_PLAY_SELECT);
        refreshHandCards();
        updateUI();
        return;
    }

    weiyan->chengShiDiscardedCard = discardedCard;

    targetSelectPanel->setVisible(false);

    if (weiyan->chengShiCanTrigger) {
        weiyan->heal(1);
        weiyan->handCards.push_back(discardedCard);
        appendLog(QString("【乘势】触发！回1血，获得【%1】").arg(discardedCard->name));
        weiyan->chengShiDiscardedCard = nullptr;
        weiyan->chengShiCanTrigger = false;
    } else {
        discardPileCount++;
        discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
    }

    triggerKuangGu(weiyan, yingZhanTarget, 0);

    if (yingZhanTarget->hp <= 0) {
        appendLog(QString("【%1】进入濒死状态").arg(yingZhanTarget->name));
        handleNearDeath(yingZhanTarget);
    }

    yingZhanTarget = nullptr;
    yingZhanSelectedIndex = -1;
    enterFlowState(FlowState::TURN_PLAY_SELECT);
    refreshHandCards();
    updateUI();
}
