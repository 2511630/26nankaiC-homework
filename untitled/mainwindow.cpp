#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "WeiYan.h"
#include "XuSheng.h"
#include "DeckManager.h"
#include "BasicCards.h"
#include <QFile>
#include <algorithm>
#include <QMessageBox>

// ==================== CardWidget ====================
CardWidget::CardWidget(const QString& cardName, int index, QWidget* parent)
    : QLabel(parent), cardName(cardName), index(index), selected(false) {
    setFixedSize(108, 154);
    setAlignment(Qt::AlignCenter);
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("QLabel { border: 2px solid rgba(214, 176, 116, 0.72); border-radius: 8px; }");
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
    : QMainWindow(parent), ui(new Ui::MainWindow), selectedHandCard(nullptr) {
    ui->setupUi(this);
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
    zhuangShiHpLoss = 0;
    discardPhaseCount = 0;
    flowState = FlowState::START_MENU;
    
    initUI();
}

MainWindow::~MainWindow() {
    delete ui;
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

    zhuangShiPanel = new QWidget();
    zhuangShiPanel->setVisible(false);
    QVBoxLayout* zhuangShiVLayout = new QVBoxLayout(zhuangShiPanel);
    zhuangShiVLayout->setContentsMargins(0, 0, 0, 0);
    zhuangShiVLayout->setSpacing(10);
    lblZhuangShiStep = new QLabel("【壮誓】请选择要弃置的牌");
    lblZhuangShiStep->setStyleSheet("QLabel { color: #F2DEB7; font-size: 16px; font-weight: bold; text-align: center; }");
    lblZhuangShiStep->setAlignment(Qt::AlignCenter);
    zhuangShiVLayout->addWidget(lblZhuangShiStep);

    QHBoxLayout* zhuangShiRow1 = new QHBoxLayout();
    zhuangShiRow1->setSpacing(10);
    zhuangShiRow1->setAlignment(Qt::AlignCenter);
    lblDiscard = new QLabel("弃牌数");
    lblDiscard->setStyleSheet("QLabel { color: #F2DEB7; font-weight: bold; }");
    lblDiscard->setVisible(false);
    zhuangShiRow1->addWidget(lblDiscard);
    spinZhuangShiDiscard = new QSpinBox();
    spinZhuangShiDiscard->setRange(0, 5);
    spinZhuangShiDiscard->setValue(0);
    spinZhuangShiDiscard->setVisible(false);
    spinZhuangShiDiscard->setFixedWidth(60);
    zhuangShiRow1->addWidget(spinZhuangShiDiscard);
    lblHpLoss = new QLabel("失血数");
    lblHpLoss->setStyleSheet("QLabel { color: #F2DEB7; font-weight: bold; }");
    lblHpLoss->setVisible(false);
    zhuangShiRow1->addWidget(lblHpLoss);
    spinZhuangShiHpLoss = new QSpinBox();
    spinZhuangShiHpLoss->setRange(0, 5);
    spinZhuangShiHpLoss->setValue(0);
    spinZhuangShiHpLoss->setVisible(false);
    spinZhuangShiHpLoss->setFixedWidth(60);
    zhuangShiRow1->addWidget(spinZhuangShiHpLoss);
    zhuangShiVLayout->addLayout(zhuangShiRow1);

    QHBoxLayout* zhuangShiButtons = new QHBoxLayout();
    zhuangShiButtons->setSpacing(12);
    zhuangShiButtons->setAlignment(Qt::AlignCenter);
    btnZhuangShiConfirm = new QPushButton("确定");
    btnZhuangShiConfirm->setFixedSize(100, 36);
    connect(btnZhuangShiConfirm, &QPushButton::clicked, this, &MainWindow::onZhuangShiDiscardConfirm);
    zhuangShiButtons->addWidget(btnZhuangShiConfirm);
    btnZhuangShiCancel = new QPushButton("取消");
    btnZhuangShiCancel->setFixedSize(100, 36);
    connect(btnZhuangShiCancel, &QPushButton::clicked, this, &MainWindow::onZhuangShiCancel);
    zhuangShiButtons->addWidget(btnZhuangShiCancel);
    zhuangShiVLayout->addLayout(zhuangShiButtons);
    centerPanelLayout->addWidget(zhuangShiPanel, 0, Qt::AlignCenter);

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
    btnConfirm->setFixedSize(104, 38);
    btnConfirm->setVisible(false);
    btnConfirm->setEnabled(false);
    connect(btnConfirm, &QPushButton::clicked, this, &MainWindow::resolveSelectedCardPlay);
    actionButtons->addWidget(btnConfirm);
    btnCancel = new QPushButton("取消");
    btnCancel->setFixedSize(104, 38);
    btnCancel->setVisible(false);
    btnCancel->setEnabled(false);
    connect(btnCancel, &QPushButton::clicked, [this]() {
        selectedHandCard = nullptr;
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
    handCardsArea = new QWidget();
    handCardsArea->setMinimumHeight(190);
    handCardsArea->setStyleSheet("QWidget { background: rgba(8, 6, 5, 0.35); border: 1px solid rgba(214, 176, 116, 0.33); border-radius: 12px; }");
    handCardsLayout = new QHBoxLayout(handCardsArea);
    handCardsLayout->setContentsMargins(18, 14, 18, 14);
    handCardsLayout->setSpacing(10);
    handCardsLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    bottomLayout->addWidget(handCardsArea, 1);
    heroSelf = new HeroCardWidget("界·徐盛", imagePath + "界徐盛登场.png", 4);
    heroSelf->setHp(4);
    bottomLayout->addWidget(heroSelf, 0, Qt::AlignBottom);
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
        if (flowState == FlowState::ZHUANGSHI_DISCARD || flowState == FlowState::DISCARD_PHASE) {
            for (int idx : selectedDiscardIndices) {
                if (idx == (int)i) {
                    isSelected = true;
                    cardWidget->selected = true;
                    break;
                }
            }
        }

        if (selectedHandCard == cardWidget) {
            cardWidget->setStyleSheet("QLabel { border: 3px solid #FFD700; border-radius: 8px; }");
        } else if (isSelected) {
            cardWidget->setStyleSheet("QLabel { border: 3px solid #FFA500; border-radius: 8px; }");
        } else {
            cardWidget->setStyleSheet("QLabel { border: 2px solid rgba(214, 176, 116, 0.72); border-radius: 8px; }");
        }

        connect(cardWidget, &CardWidget::clicked, this, &MainWindow::onCardClicked);

        handCardWidgets.append(cardWidget);
        handCardsLayout->addWidget(cardWidget);
    }
}

void MainWindow::onCardClicked(CardWidget* card) {
    if (flowState == FlowState::ZHUANGSHI_DISCARD) {
        auto it = std::find(selectedDiscardIndices.begin(), selectedDiscardIndices.end(), card->index);
        if (it != selectedDiscardIndices.end()) {
            selectedDiscardIndices.erase(it);
        } else {
            selectedDiscardIndices.push_back(card->index);
        }
        refreshHandCards();
        updateConfirmState();
    } else if (flowState == FlowState::DISCARD_PHASE) {
        auto it = std::find(selectedDiscardIndices.begin(), selectedDiscardIndices.end(), card->index);
        if (it != selectedDiscardIndices.end()) {
            selectedDiscardIndices.erase(it);
        } else if ((int)selectedDiscardIndices.size() < discardPhaseCount) {
            selectedDiscardIndices.push_back(card->index);
        }
        refreshHandCards();
        updateConfirmState();
    } else {
        if (selectedHandCard == card) {
            selectedHandCard = nullptr;
            pendingCardName.clear();
        } else {
            selectedHandCard = card;
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

    if (cardName == "桃") {
        return currentPlayer()->hp < currentPlayer()->maxHp || currentPlayer()->hp <= 0;
    }
    if (cardName == "酒") {
        if (currentPlayer()->hp <= 0) return true;
        return isWeiYanTurn ? !weiyanWineUsedThisTurn : !xushengWineUsedThisTurn;
    }
    if (cardName == "杀") {
        Player* p = isWeiYanTurn ? static_cast<Player*>(weiyan) : static_cast<Player*>(xusheng);
        return p->slashUsedThisTurn < p->slashCountLimit;
    }
    // 其他卡牌都能出
    return true;
}

void MainWindow::updateConfirmState() {
    if (flowState == FlowState::ZHUANGSHI_DISCARD) {
        btnConfirm->setVisible(true);
        btnCancel->setVisible(true);
        btnConfirm->setEnabled(!selectedDiscardIndices.empty());
        btnCancel->setEnabled(true);
    } else if (flowState == FlowState::DISCARD_PHASE) {
        btnConfirm->setVisible(true);
        btnCancel->setVisible(true);
        btnConfirm->setEnabled((int)selectedDiscardIndices.size() == discardPhaseCount);
        btnCancel->setEnabled(true);
    } else {
        bool canConfirm = selectedHandCard && !pendingCardName.isEmpty() && isCardPlayable(pendingCardName);
        bool hasSelection = selectedHandCard != nullptr;
        btnConfirm->setVisible(canConfirm);
        btnCancel->setVisible(hasSelection);
        btnConfirm->setEnabled(canConfirm);
        btnCancel->setEnabled(hasSelection);
    }
}

void MainWindow::resolveSelectedCardPlay() {
    if (flowState == FlowState::ZHUANGSHI_DISCARD) {
        onZhuangShiDiscardConfirm();
        return;
    }

    if (flowState == FlowState::DISCARD_PHASE) {
        onDiscardPhaseConfirm();
        return;
    }

    if (!selectedHandCard || pendingCardName.isEmpty()) return;

    Player* player = currentPlayer();
    Player* enemy = enemyPlayer();
    if (!player || !enemy) return;

    std::shared_ptr<Card> selectedCard = nullptr;
    for (auto it = player->handCards.begin(); it != player->handCards.end(); ++it) {
        if ((*it)->name == pendingCardName) {
            selectedCard = *it;
            player->handCards.erase(it);
            break;
        }
    }

    if (!selectedCard) return;

    if (pendingCardName == "杀") {
        useSlashEffect(player, enemy, isWeiYanTurn);
    } else if (pendingCardName == "桃") {
        player->heal(1);
        appendLog(QString("【%1】使用【桃】，回复1点体力").arg(player->name));
    } else if (pendingCardName == "酒") {
        if (player->hp <= 0) {
            player->heal(1);
            appendLog(QString("【%1】濒死使用【酒】，回复1点体力").arg(player->name));
        } else {
            if (isWeiYanTurn) {
                weiyanWineUsedThisTurn = true;
                weiyanWineBuffActive = true;
            } else {
                xushengWineUsedThisTurn = true;
                xushengWineBuffActive = true;
            }
            appendLog(QString("【%1】使用【酒】，下一张杀伤害+1").arg(player->name));
        }
    } else if (pendingCardName == "决斗") {
        useCardEffect(selectedCard, player, enemy);
    } else if (pendingCardName == "南蛮入侵") {
        useCardEffect(selectedCard, player, enemy);
    } else if (pendingCardName == "万箭齐发") {
        useCardEffect(selectedCard, player, enemy);
    } else if (pendingCardName == "顺手牵羊") {
        useCardEffect(selectedCard, player, enemy);
    } else if (pendingCardName == "过河拆桥") {
        useCardEffect(selectedCard, player, enemy);
    } else if (pendingCardName == "诸葛连弩" || pendingCardName == "古锭刀" || pendingCardName == "麒麟弓") {
        player->equipment.weapon = selectedCard;
        appendLog(QString("【%1】装备了武器【%2】").arg(player->name).arg(pendingCardName));
    } else if (pendingCardName == "仁王盾" || pendingCardName == "白银狮子") {
        player->equipment.armor = selectedCard;
        appendLog(QString("【%1】装备了防具【%2】").arg(player->name).arg(pendingCardName));
    } else if (pendingCardName == "+1马") {
        player->equipment.plusHorse = selectedCard;
        appendLog(QString("【%1】装备了+1马").arg(player->name));
    } else if (pendingCardName == "-1马") {
        player->equipment.minusHorse = selectedCard;
        appendLog(QString("【%1】装备了-1马").arg(player->name));
    }

    selectedHandCard = nullptr;
    pendingCardName.clear();
    refreshHandCards();
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
        }
    } else if (cardName == "顺手牵羊") {
        if (source->getCurrentDistance() > 1) {
            appendLog("【顺手牵羊】失败：距离超过1");
        } else if (!target->handCards.empty()) {
            source->handCards.push_back(target->handCards.back());
            target->handCards.pop_back();
            appendLog(QString("【顺手牵羊】成功：获得【%1】的手牌").arg(target->name));
        }
    } else if (cardName == "过河拆桥") {
        if (!target->handCards.empty()) {
            target->handCards.pop_back();
            appendLog(QString("【过河拆桥】弃置【%1】一张手牌").arg(target->name));
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

void MainWindow::useSlashEffect(Player* source, Player* target, bool useWeiYanSkill) {
    int damage = 1;
    if (useWeiYanSkill && weiyanWineBuffActive) {
        damage += 1;
        weiyanWineBuffActive = false;
        appendLog("【酒】生效：伤害+1");
    } else if (!useWeiYanSkill && xushengWineBuffActive) {
        damage += 1;
        xushengWineBuffActive = false;
        appendLog("【酒】生效：伤害+1");
    }

    appendLog(QString("【%1】使用【杀】").arg(source->name));

    if (useWeiYanSkill) {
        weiyan->useSlash(target, damage);
    } else {
        xusheng->useSlash(target, damage);
    }

    if (target->hp <= 0) {
        appendLog(QString("【%1】进入濒死状态").arg(target->name));
        handleNearDeath(target);
    } else if (useWeiYanSkill && weiyan->kuangGuCanTrigger) {
        appendLog(">>> 触发【狂骨】，请选择：");
        btnKuangGuHeal->setVisible(true);
        btnKuangGuDraw->setVisible(true);
        btnKuangGuSkip->setVisible(true);
        btnKuangGuHeal->setEnabled(true);
        btnKuangGuDraw->setEnabled(true);
        btnKuangGuSkip->setEnabled(true);
    }
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
    bool isZhuangShiPhase = flowState == FlowState::ZHUANGSHI_STEP1 || flowState == FlowState::ZHUANGSHI_STEP2 || flowState == FlowState::ZHUANGSHI_DISCARD;

    btnZhuangShi->setVisible(isWeiYanTurn && !zhuangShiUsed && weiyan->currentState != SUCCESS);
    btnZhuangShi->setEnabled(isWeiYanTurn && !zhuangShiUsed && weiyan->currentState != SUCCESS && weiyan->handCards.size() > 0);
    
    btnBeiShui->setVisible(isWeiYanTurn && weiyan->currentState == SUCCESS && !isDiscardPhase && !isZhuangShiPhase);
    btnBeiShui->setEnabled(isWeiYanTurn && weiyan->currentState == SUCCESS && !beiShuiUsed);
    
    btnEndTurn->setVisible(!isDiscardPhase && !isZhuangShiPhase);
    btnEndTurn->setEnabled(!isDiscardPhase && !isZhuangShiPhase);
}

void MainWindow::startZhuangShiStep1() {
    lblZhuangShiStep->setText("【壮誓】请选择要弃置的牌（点击选择，再次点击取消）");
    lblDiscard->setVisible(false);
    spinZhuangShiDiscard->setVisible(false);
    lblHpLoss->setVisible(false);
    spinZhuangShiHpLoss->setVisible(false);
    selectedDiscardIndices.clear();
    btnZhuangShiConfirm->disconnect();
    connect(btnZhuangShiConfirm, &QPushButton::clicked, this, &MainWindow::onZhuangShiDiscardConfirm);
    zhuangShiPanel->setVisible(true);
    enterFlowState(FlowState::ZHUANGSHI_DISCARD);
    appendLog(">>> 【壮誓】请选择要弃置的牌（至少选1张）");
    refreshHandCards();
}

void MainWindow::startZhuangShiStep2() {
    lblZhuangShiStep->setText("【壮誓】选择失去几点体力");
    lblDiscard->setVisible(false);
    spinZhuangShiDiscard->setVisible(false);
    lblHpLoss->setVisible(true);
    spinZhuangShiHpLoss->setVisible(true);
    spinZhuangShiHpLoss->setRange(0, weiyan->hp);
    spinZhuangShiHpLoss->setValue(0);
    btnZhuangShiConfirm->disconnect();
    connect(btnZhuangShiConfirm, &QPushButton::clicked, this, &MainWindow::onZhuangShiStep2Confirm);
    enterFlowState(FlowState::ZHUANGSHI_STEP2);
    appendLog(">>> 【壮誓】第2步：请选择失去几点体力");
}

void MainWindow::startZhuangShiDiscard() {
    startZhuangShiStep1();
}

void MainWindow::onZhuangShiStep1Confirm() {
}

void MainWindow::onZhuangShiStep2Confirm() {
    zhuangShiHpLoss = spinZhuangShiHpLoss->value();
    zhuangShiUsed = true;
    
    if (zhuangShiHpLoss >= weiyan->hp) {
        weiyan->currentState = FAILURE;
        weiyan->hp -= zhuangShiHpLoss;
        appendLog("【壮誓】扣血等于当前体力，使命失败！");
        if (weiyan->hp <= 0) {
            handleNearDeath(weiyan);
        }
    } else {
        weiyan->currentState = START;
        weiyan->zhuangShi_discarded = zhuangShiDiscardCount;
        weiyan->zhuangShi_hpLost = zhuangShiHpLoss;
        weiyan->hp -= zhuangShiHpLoss;
        weiyan->slashCountLimit += zhuangShiDiscardCount;
        appendLog(QString("【壮誓】弃置%1张牌，失去%2点体力，本回合杀次数+%3").arg(zhuangShiDiscardCount).arg(zhuangShiHpLoss).arg(zhuangShiDiscardCount));
    }
    
    zhuangShiPanel->setVisible(false);
    enterFlowState(FlowState::TURN_PLAY_SELECT);
    updateSkillPanelVisibility();
    refreshHandCards();
    updateUI();
}

void MainWindow::onZhuangShiDiscardConfirm() {
    if (selectedDiscardIndices.empty()) {
        appendLog("请至少选择1张牌！");
        return;
    }
    
    zhuangShiDiscardCount = selectedDiscardIndices.size();
    std::sort(selectedDiscardIndices.rbegin(), selectedDiscardIndices.rend());
    for (int idx : selectedDiscardIndices) {
        if (idx < (int)weiyan->handCards.size()) {
            weiyan->handCards.erase(weiyan->handCards.begin() + idx);
            discardPileCount++;
        }
    }
    discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
    selectedDiscardIndices.clear();
    
    if (weiyan->handCards.empty()) {
        weiyan->currentState = FAILURE;
        zhuangShiUsed = true;
        zhuangShiPanel->setVisible(false);
        enterFlowState(FlowState::TURN_PLAY_SELECT);
        updateSkillPanelVisibility();
        refreshHandCards();
        updateUI();
        return;
    }
    
    startZhuangShiStep2();
}

void MainWindow::onZhuangShiCancel() {
    weiyan->currentState = FAILURE;
    zhuangShiUsed = true;
    zhuangShiPanel->setVisible(false);
    selectedDiscardIndices.clear();
    enterFlowState(FlowState::TURN_PLAY_SELECT);
    appendLog("【壮誓】取消，使命失败！");
    updateSkillPanelVisibility();
    updateUI();
}

void MainWindow::onStartGame() {
    resetGame();
    
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
    heroSelf->setHp(4);
    
    flowState = FlowState::START_MENU;
    isWeiYanTurn = true;
    zhuangShiUsed = false;
    beiShuiUsed = false;
    selectedHandCard = nullptr;
    pendingCardName.clear();
    selectedDiscardIndices.clear();
}

void MainWindow::startWeiYanTurn() {
    isWeiYanTurn = true;
    zhuangShiUsed = false;
    beiShuiUsed = false;
    weiyan->slashUsedThisTurn = 0;
    weiyanWineUsedThisTurn = false;
    weiyanWineBuffActive = false;
    
    lblPhase->setText("出牌阶段 - 势·魏延");
    lblPhase->setStyleSheet("font-size: 20px; font-weight: bold; color: #F44336; text-shadow: 2px 2px 4px #000;");
    
    weiyan->drawCard(deck->draw());
    weiyan->drawCard(deck->draw());
    appendLog("=== 势·魏延 回合 ===");
    appendLog("势·魏延摸2张牌");
    
    if (weiyan->currentState != SUCCESS) {
        startZhuangShiStep1();
    } else {
        enterFlowState(FlowState::TURN_PLAY_SELECT);
    }
    
    refreshHandCards();
    updateSkillPanelVisibility();
    updateUI();
}

void MainWindow::startXuShengTurn() {
    isWeiYanTurn = false;
    xusheng->slashUsedThisTurn = 0;
    xushengWineUsedThisTurn = false;
    xushengWineBuffActive = false;
    
    lblPhase->setText("出牌阶段 - 界·徐盛");
    lblPhase->setStyleSheet("font-size: 20px; font-weight: bold; color: #2196F3; text-shadow: 2px 2px 4px #000;");
    
    xusheng->drawCard(deck->draw());
    xusheng->drawCard(deck->draw());
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
        lblPhase->setStyleSheet("font-size: 24px; font-weight: bold; color: #2196F3; text-shadow: 2px 2px 4px #000;");
        QMessageBox::information(this, "游戏结束", "界·徐盛 获胜！");
        btnStart->setVisible(true);
        btnZhuangShi->setVisible(false);
        btnBeiShui->setVisible(false);
        btnEndTurn->setVisible(false);
    } else if (xusheng->hp <= 0) {
        lblPhase->setText("游戏结束 - 势·魏延 获胜！");
        lblPhase->setStyleSheet("font-size: 24px; font-weight: bold; color: #F44336; text-shadow: 2px 2px 4px #000;");
        QMessageBox::information(this, "游戏结束", "势·魏延 获胜！");
        btnStart->setVisible(true);
        btnZhuangShi->setVisible(false);
        btnBeiShui->setVisible(false);
        btnEndTurn->setVisible(false);
    }
}

void MainWindow::updateUI() {
    heroWeiYan->setHp(weiyan->hp);
    heroXuSheng->setHp(xusheng->hp);
    heroSelf->setHp(isWeiYanTurn ? weiyan->hp : xusheng->hp);
    txtLog->ensureCursorVisible();
}

void MainWindow::appendLog(const QString& text) {
    txtLog->append(text);
}

void MainWindow::onWeiYanZhuangShi() {
    startZhuangShiStep1();
}

void MainWindow::onWeiYanBeiShui() {
    if (!weiyan || weiyan->currentState != SUCCESS) return;
    
    if (weiyan->handCards.empty()) {
        appendLog("没有手牌可弃置！");
        return;
    }
    
    weiyan->handCards.pop_back();
    weiyan->slashCountLimit++;
    beiShuiUsed = true;
    discardPileCount++;
    discardPileLabel->setText(QString("弃牌堆：%1").arg(discardPileCount));
    
    appendLog("【背水】弃1牌，本回合杀次数+1");
    
    refreshHandCards();
    updateSkillPanelVisibility();
    updateUI();
}

void MainWindow::onEndTurn() {
    enterFlowState(FlowState::TURN_END);
    if (isWeiYanTurn) {
        weiyan->endTurn();
        
        if (weiyan->currentState == FAILURE || (!zhuangShiUsed && weiyan->currentState != SUCCESS)) {
            appendLog("【困奋】触发：失1血，摸2牌");
            weiyan->takeDamage(1);
            auto card1 = deck->draw();
            if (card1) weiyan->drawCard(card1);
            auto card2 = deck->draw();
            if (card2) weiyan->drawCard(card2);
        }
        
        if (weiyan->currentState == SUCCESS) {
            if (weiyan->slashUsedThisTurn < (int)weiyan->zhuangShi_discarded) {
                appendLog("【背水】摸1牌");
                auto card = deck->draw();
                if (card) weiyan->drawCard(card);
            }
            if (weiyan->slashUsedThisTurn < weiyan->zhuangShi_hpLost) {
                appendLog("【背水】回1血");
                if (weiyan->hp < weiyan->maxHp) {
                    weiyan->heal(1);
                } else {
                    auto card = deck->draw();
                    if (card) weiyan->drawCard(card);
                }
            }
        }
        
        if (weiyan->hp <= 0) {
            checkGameOver();
            return;
        }
        
        startDiscardPhase(weiyan);
    } else {
        startDiscardPhase(xusheng);
    }
    updateUI();
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
    lblPhase->setStyleSheet("font-size: 18px; font-weight: bold; color: #FF9800; text-shadow: 2px 2px 4px #000;");
    
    btnZhuangShi->setVisible(false);
    btnBeiShui->setVisible(false);
    btnEndTurn->setVisible(false);
    
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

void MainWindow::handleNearDeath(Player* player) {
    if (player->hp > 0) return;
    
    appendLog(QString("【%1】进入濒死状态！").arg(player->name));
    
    bool canSave = false;
    for (auto& card : player->handCards) {
        if (card->getName() == "桃" || card->getName() == "酒") {
            canSave = true;
            break;
        }
    }
    
    if (!canSave) {
        appendLog(QString("【%1】无法自救！").arg(player->name));
        checkGameOver();
        return;
    }
    
    appendLog(">>> 请使用【桃】或【酒】自救");
    flowState = FlowState::NEAR_DEATH;
    refreshHandCards();
}

void MainWindow::onWeiYanKuangGuHeal() {
    if (!weiyan) return;
    weiyan->heal(1);
    appendLog("【狂骨】回复1点体力");
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

void MainWindow::clearTableCards() {
    tableCardLeft->setVisible(false);
    tableCardRight->setVisible(false);
}
