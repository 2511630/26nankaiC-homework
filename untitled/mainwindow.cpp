#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPalette>
#include <QBrush>
#include <QPixmap>
#include <QPainter>

CardWidget::CardWidget(const QString& cardName, QWidget* parent) : QLabel(parent), cardName(cardName), selected(false) {
    setFixedSize(108, 154);
    setAlignment(Qt::AlignCenter);
    setStyleSheet(
        "QLabel {"
        " border: 2px solid rgba(176, 139, 85, 0.95);"
        " border-radius: 10px;"
        " background: rgba(22, 18, 15, 0.85);"
        " color: #F5E6C8;"
        "}"
    );
    setCursor(Qt::PointingHandCursor);
}

void CardWidget::mousePressEvent(QMouseEvent* event) {
    selected = !selected;
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(selected ? 24 : 0);
    shadow->setOffset(0, selected ? -4 : 0);
    shadow->setColor(QColor(245, 210, 120, selected ? 220 : 0));
    setGraphicsEffect(shadow);

    if (selected) {
        setStyleSheet(
            "QLabel {"
            " border: 3px solid #F2C66D;"
            " border-radius: 10px;"
            " background: rgba(22, 18, 15, 0.85);"
            " color: #FFF3D0;"
            "}"
        );
    } else {
        setStyleSheet(
            "QLabel {"
            " border: 2px solid rgba(176, 139, 85, 0.95);"
            " border-radius: 10px;"
            " background: rgba(22, 18, 15, 0.85);"
            " color: #F5E6C8;"
            "}"
        );
    }
    emit clicked(this);
    QLabel::mousePressEvent(event);
}

HeroCardWidget::HeroCardWidget(const QString& heroName, const QString& imgPath, int maxHp, QWidget* parent) 
    : QWidget(parent), heroName(heroName) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(4);

    heroImage = new QLabel();
    heroImage->setFixedSize(106, 142);
    heroImage->setScaledContents(true);
    heroImage->setStyleSheet("QLabel { border: 2px solid rgba(255, 227, 171, 0.95); border-radius: 10px; background: rgba(0, 0, 0, 0.35); }");
    
    QPixmap pixmap(imgPath);
    if (!pixmap.isNull()) {
        heroImage->setPixmap(pixmap.scaled(106, 142, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
    layout->addWidget(heroImage, 0, Qt::AlignCenter);

    hpLayout = new QHBoxLayout();
    hpLayout->setSpacing(1);
    hpLayout->setAlignment(Qt::AlignCenter);

    for (int i = 0; i < maxHp; ++i) {
        QLabel* hpIcon = new QLabel();
        hpIcon->setFixedSize(16, 16);
        QPixmap hpPixmap("D:/OneDrive/Desktop/GitHub/26nankaiC-homework/C++作业图片/red_hp.png");
        if (!hpPixmap.isNull()) {
            hpIcon->setPixmap(hpPixmap.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        hpIcons.append(hpIcon);
        hpLayout->addWidget(hpIcon);
    }
    
    layout->addLayout(hpLayout);
    
    hpLabel = new QLabel(heroName);
    hpLabel->setStyleSheet("QLabel { font-size: 13px; color: #F6E6BF; text-align: center; font-weight: bold; }");
    hpLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(hpLabel);

    setLayout(layout);
}

void HeroCardWidget::setHp(int hp) {
    for (int i = 0; i < hpIcons.size(); ++i) {
        QString path = (i < hp) ? "D:/OneDrive/Desktop/GitHub/26nankaiC-homework/C++作业图片/red_hp.png" : "D:/OneDrive/Desktop/GitHub/26nankaiC-homework/C++作业图片/green_hp.png";
        QPixmap pixmap(path);
        if (!pixmap.isNull()) {
            hpIcons[i]->setPixmap(pixmap.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void HeroCardWidget::setSelected(bool selected) {
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(heroImage);
    effect->setColor(selected ? QColor(110, 240, 130, 240) : Qt::transparent);
    effect->setBlurRadius(selected ? 28 : 0);
    effect->setOffset(0, 0);
    heroImage->setGraphicsEffect(effect);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , selectedHandCard(nullptr)
    , discardPileCount(0)
{
    ui->setupUi(this);
    imagePath = "D:/OneDrive/Desktop/GitHub/26nankaiC-homework/C++作业图片/";
    setWindowTitle("三国杀 - 势·魏延 VS 界·徐盛");
    setMinimumSize(1200, 760);
    resize(1280, 800);
    initUI();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete weiyan;
    delete xusheng;
    delete deck;
}

void MainWindow::initUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(16, 14, 16, 14);
    mainLayout->setSpacing(10);

    QString bgPath = imagePath + "牌局背景.jpg";
    QPalette palette;
    QPixmap bgImage(bgPath);
    if (bgImage.isNull()) {
        bgImage.load(imagePath + "战局背景.jpg");
    }
    if (bgImage.isNull()) {
        palette.setColor(QPalette::Window, QColor(42, 35, 29));
    } else {
        palette.setBrush(QPalette::Window, bgImage.scaled(1400, 900, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    centralWidget->setPalette(palette);
    centralWidget->setAutoFillBackground(true);

    centralWidget->setStyleSheet(
        "QPushButton {"
        " background: rgba(52, 37, 25, 0.86);"
        " color: #F4E1B6;"
        " border: 1px solid rgba(196, 153, 95, 0.92);"
        " border-radius: 8px;"
        " font-weight: 700;"
        " padding: 4px 10px;"
        "}"
        "QPushButton:hover {"
        " background: rgba(78, 58, 40, 0.92);"
        "}"
        "QPushButton:pressed {"
        " background: rgba(35, 25, 18, 0.96);"
        "}"
        "QPushButton:disabled {"
        " color: rgba(230, 220, 200, 0.48);"
        " border-color: rgba(160, 140, 118, 0.45);"
        " background: rgba(22, 18, 16, 0.6);"
        "}"
    );

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(0);

    heroXuSheng = new HeroCardWidget("界·徐盛", imagePath + "界徐盛.webp", 4);
    heroXuSheng->setHp(4);
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

    QHBoxLayout* actionButtons = new QHBoxLayout();
    actionButtons->setSpacing(12);
    actionButtons->setAlignment(Qt::AlignCenter);

    btnConfirm = new QPushButton("确定");
    btnConfirm->setFixedSize(104, 38);
    btnConfirm->setEnabled(false);
    connect(btnConfirm, &QPushButton::clicked, this, &MainWindow::resolveSelectedCardPlay);
    actionButtons->addWidget(btnConfirm);

    btnCancel = new QPushButton("取消");
    btnCancel->setFixedSize(104, 38);
    btnCancel->setEnabled(false);
    connect(btnCancel, &QPushButton::clicked, [this]() {
        selectedHandCard = nullptr;
        pendingCardName.clear();
        refreshHandCards();
        updateConfirmState();
        appendLog("取消当前选牌");
    });
    actionButtons->addWidget(btnCancel);

    centerPanelLayout->addLayout(actionButtons);

    QHBoxLayout* skillButtons = new QHBoxLayout();
    skillButtons->setSpacing(10);
    skillButtons->setAlignment(Qt::AlignCenter);

    btnStart = new QPushButton("开始游戏");
    btnStart->setFixedSize(110, 40);
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onStartGame);
    skillButtons->addWidget(btnStart);

    btnZhuangShi = new QPushButton("壮誓");
    btnZhuangShi->setFixedSize(88, 40);
    btnZhuangShi->setEnabled(false);
    connect(btnZhuangShi, &QPushButton::clicked, this, &MainWindow::onWeiYanZhuangShi);
    skillButtons->addWidget(btnZhuangShi);

    btnSlash = new QPushButton("出杀");
    btnSlash->setFixedSize(88, 40);
    btnSlash->setEnabled(false);
    connect(btnSlash, &QPushButton::clicked, this, &MainWindow::onWeiYanSlash);
    skillButtons->addWidget(btnSlash);

    btnBeiShui = new QPushButton("背水");
    btnBeiShui->setFixedSize(88, 40);
    btnBeiShui->setEnabled(false);
    connect(btnBeiShui, &QPushButton::clicked, this, &MainWindow::onWeiYanBeiShui);
    skillButtons->addWidget(btnBeiShui);

    btnXuShengSlash = new QPushButton("徐盛出杀");
    btnXuShengSlash->setFixedSize(96, 40);
    btnXuShengSlash->setEnabled(false);
    connect(btnXuShengSlash, &QPushButton::clicked, this, &MainWindow::onXuShengSlash);
    skillButtons->addWidget(btnXuShengSlash);

    btnEndTurn = new QPushButton("结束回合");
    btnEndTurn->setFixedSize(108, 40);
    btnEndTurn->setEnabled(false);
    connect(btnEndTurn, &QPushButton::clicked, this, &MainWindow::onEndTurn);
    skillButtons->addWidget(btnEndTurn);

    centerPanelLayout->addLayout(skillButtons);

    QHBoxLayout* kuangGuButtons = new QHBoxLayout();
    kuangGuButtons->setSpacing(10);
    kuangGuButtons->setAlignment(Qt::AlignCenter);

    btnKuangGuHeal = new QPushButton("回复1点体力");
    btnKuangGuHeal->setFixedSize(130, 38);
    btnKuangGuHeal->setEnabled(false);
    connect(btnKuangGuHeal, &QPushButton::clicked, this, &MainWindow::onWeiYanKuangGuHeal);
    kuangGuButtons->addWidget(btnKuangGuHeal);

    btnKuangGuDraw = new QPushButton("摸一张牌");
    btnKuangGuDraw->setFixedSize(130, 38);
    btnKuangGuDraw->setEnabled(false);
    connect(btnKuangGuDraw, &QPushButton::clicked, this, &MainWindow::onWeiYanKuangGuDraw);
    kuangGuButtons->addWidget(btnKuangGuDraw);

    centerPanelLayout->addLayout(kuangGuButtons);

    middleLayout->addWidget(centerPanel, 1);

    QVBoxLayout* rightPanel = new QVBoxLayout();
    rightPanel->setSpacing(10);
    rightPanel->setAlignment(Qt::AlignBottom);
    txtLog = new QTextEdit();
    txtLog->setReadOnly(true);
    txtLog->setMinimumSize(250, 250);
    txtLog->setStyleSheet(
        "QTextEdit {"
        " background: rgba(9, 7, 6, 0.76);"
        " color: #EFE3C5;"
        " font-size: 13px;"
        " border: 1px solid rgba(214, 176, 116, 0.72);"
        " border-radius: 10px;"
        " padding: 6px;"
        "}"
    );
    rightPanel->addWidget(txtLog);

    middleLayout->addLayout(rightPanel);
    mainLayout->addLayout(middleLayout, 1);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(14);

    heroWeiYan = new HeroCardWidget("势·魏延", imagePath + "4.webp", 5);
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

    heroSelf = new HeroCardWidget("势·魏延", imagePath + "4.webp", 5);
    heroSelf->setHp(5);
    bottomLayout->addWidget(heroSelf, 0, Qt::AlignBottom);

    mainLayout->addLayout(bottomLayout);

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, [this]() {
        skillAnimationLabel->setVisible(false);
    });
}

void MainWindow::showSkillAnimation(const QString& skillName, const QString& heroName) {
    QPixmap animationPixmap(450, 150);
    animationPixmap.fill(Qt::transparent);
    
    QPainter painter(&animationPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPixmap heroImg;
    if (heroName == "势·魏延") {
        heroImg.load(imagePath + "4.webp");
    } else {
        heroImg.load(imagePath + "界徐盛.webp");
    }

    if (!heroImg.isNull()) {
        painter.drawPixmap(30, 15, heroImg.scaled(100, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    painter.setFont(QFont("SimHei", 42, QFont::Bold));
    QPen pen(QColor(255, 215, 0));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawText(150, 95, skillName);

    painter.setFont(QFont("SimHei", 14));
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(150, 130, heroName + "发动了技能");

    skillAnimationLabel->setPixmap(animationPixmap);
    skillAnimationLabel->setVisible(true);
    animationTimer->start(2000);
}

void MainWindow::resetGame()
{
    delete weiyan;
    delete xusheng;
    delete deck;

    weiyan = new WeiYan();
    xusheng = new XuSheng();
    deck = new DeckManager();
    deck->shuffle();

    isWeiYanTurn = true;
    zhuangShiUsed = false;
    kuangGuTriggered = false;
    beiShuiAsked = false;
    selectedHandCard = nullptr;
    pendingCardName.clear();
    discardPileCount = 0;
    discardPileLabel->setText("弃牌堆：0");
    clearTableCards();

    for(int i=0; i<4; ++i) {
        weiyan->drawCard(deck->draw());
        xusheng->drawCard(deck->draw());
    }

    appendLog("=== 势·魏延 VS 界·徐盛 ===");
    appendLog("双方各摸4张手牌");
    updateUI();
}

void MainWindow::onStartGame()
{
    txtLog->clear();
    resetGame();
    btnStart->setEnabled(false);
    startWeiYanTurn();
}

void MainWindow::startWeiYanTurn()
{
    isWeiYanTurn = true;
    zhuangShiUsed = false;
    kuangGuTriggered = false;
    beiShuiAsked = false;
    selectedHandCard = nullptr;
    pendingCardName.clear();
    weiyan->startTurn();
    weiyan->drawCard(deck->draw());
    weiyan->drawCard(deck->draw());
    appendLog("势·魏延摸2张牌");

    lblPhase->setText("出牌阶段 - 势·魏延");
    lblPhase->setStyleSheet("font-size: 18px; font-weight: bold; color: #F44336; text-shadow: 2px 2px 4px #000;");

    btnZhuangShi->setEnabled(!zhuangShiUsed && weiyan->currentState != SUCCESS);
    btnSlash->setEnabled(false);
    btnBeiShui->setEnabled(weiyan->currentState == SUCCESS && !beiShuiAsked && weiyan->handCards.size() >= 1);
    btnXuShengSlash->setEnabled(false);
    btnEndTurn->setEnabled(true);
    btnConfirm->setEnabled(false);
    btnCancel->setEnabled(false);

    if (weiyan->currentState == SUCCESS) {
        appendLog(">>> 可发动【背水】");
    }
}

void MainWindow::startXuShengTurn()
{
    isWeiYanTurn = false;

    xusheng->drawCard(deck->draw());
    xusheng->drawCard(deck->draw());
    appendLog("界·徐盛摸2张牌");

    lblPhase->setText("出牌阶段 - 界·徐盛");
    lblPhase->setStyleSheet("font-size: 18px; font-weight: bold; color: #2196F3; text-shadow: 2px 2px 4px #000;");

    btnZhuangShi->setEnabled(false);
    btnSlash->setEnabled(false);
    btnBeiShui->setEnabled(false);
    btnKuangGuHeal->setEnabled(false);
    btnKuangGuDraw->setEnabled(false);
    btnXuShengSlash->setEnabled(true);
    btnEndTurn->setEnabled(true);
    btnConfirm->setEnabled(false);
    btnCancel->setEnabled(false);
}

void MainWindow::onWeiYanZhuangShi()
{
    if (zhuangShiUsed || weiyan->currentState == SUCCESS) return;

    int discard = std::min(2, (int)weiyan->handCards.size());
    int hpLoss = 1;

    if (weiyan->hp <= hpLoss) {
        appendLog("体力不足，无法发动壮誓");
        return;
    }

    weiyan->activateZhuangShi(discard, hpLoss);
    zhuangShiUsed = true;
    appendLog("【壮誓】弃" + QString::number(discard) + "张，失" + QString::number(hpLoss) + "血");
    showSkillAnimation("壮誓", "势·魏延");

    btnZhuangShi->setEnabled(false);
    updateUI();
    checkGameOver();
}

void MainWindow::onWeiYanSlash()
{
    if (!removeCardFromHand(weiyan, "杀")) {
        appendLog("没有【杀】，无法出杀");
        return;
    }
    tableCardLeft->setPixmap(QPixmap(getCardImagePath("杀")).scaled(tableCardLeft->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    tableCardLeft->setVisible(true);

    kuangGuTriggered = false;
    bool xushengDodged = removeCardFromHand(xusheng, "闪");
    if (xushengDodged) {
        tableCardRight->setPixmap(QPixmap(getCardImagePath("闪")).scaled(tableCardRight->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        tableCardRight->setVisible(true);
        appendLog("界·徐盛打出【闪】");
    } else {
        weiyan->useSlash(xusheng);
    }

    if (!xushengDodged && xusheng->hp <= 0) {
        weiyan->checkKill(xusheng);
        appendLog("【忠傲】使命成功！");
    } else if (!xushengDodged && weiyan->currentState == START) {
        appendLog(">>> 触发【狂骨】，请选择：");
        kuangGuTriggered = true;
        btnKuangGuHeal->setEnabled(true);
        btnKuangGuDraw->setEnabled(true);
        btnSlash->setEnabled(false);
        btnEndTurn->setEnabled(false);
    }

    discardPileCount += xushengDodged ? 2 : 1;
    discardPileLabel->setText("弃牌堆：" + QString::number(discardPileCount));
    QTimer::singleShot(1000, this, &MainWindow::clearTableCards);

    updateUI();
    checkGameOver();
}

void MainWindow::onWeiYanKuangGuHeal()
{
    if (!kuangGuTriggered) return;

    weiyan->chooseKuangGuHeal();
    appendLog("【狂骨】选择回血");
    kuangGuTriggered = false;
    btnKuangGuHeal->setEnabled(false);
    btnKuangGuDraw->setEnabled(false);
    btnSlash->setEnabled(true);
    btnEndTurn->setEnabled(true);
    updateUI();
}

void MainWindow::onWeiYanKuangGuDraw()
{
    if (!kuangGuTriggered) return;

    weiyan->chooseKuangGuDraw();
    appendLog("【狂骨】选择摸牌");
    kuangGuTriggered = false;
    btnKuangGuHeal->setEnabled(false);
    btnKuangGuDraw->setEnabled(false);
    btnSlash->setEnabled(true);
    btnEndTurn->setEnabled(true);
    updateUI();
}

void MainWindow::onWeiYanBeiShui()
{
    if (weiyan->currentState != SUCCESS || beiShuiAsked || weiyan->handCards.size() < 1) return;

    weiyan->activateBeiShui(true);
    beiShuiAsked = true;
    appendLog("【背水】发动");
    btnBeiShui->setEnabled(false);
    updateUI();
}

void MainWindow::onXuShengSlash()
{
    xusheng->useSlash(weiyan);
    showSkillAnimation("破军", "界·徐盛");
    updateUI();
    checkGameOver();
}

void MainWindow::onEndTurn()
{
    if (isWeiYanTurn) {
        weiyan->endTurn();

        if (!zhuangShiUsed && weiyan->currentState != SUCCESS) {
            appendLog("【困奋】失1血，摸2牌");
        }

        if (weiyan->currentState == SUCCESS) {
            if (weiyan->slashUsedThisTurn < weiyan->zhuangShi_discarded) {
                appendLog("【背水】摸1牌");
            }
            if (weiyan->slashUsedThisTurn < weiyan->zhuangShi_hpLost) {
                appendLog("【背水】回1血");
            }
        }

        if (weiyan->hp <= 0) {
            checkGameOver();
            return;
        }

        startXuShengTurn();
    } else {
        startWeiYanTurn();
    }

    updateUI();
}

void MainWindow::updateUI()
{
    heroXuSheng->setHp(xusheng->hp);
    heroWeiYan->setHp(weiyan->hp);
    heroSelf->setHp(weiyan->hp);

    heroXuSheng->setSelected(!isWeiYanTurn);
    heroWeiYan->setSelected(isWeiYanTurn);

    refreshHandCards();
    updateConfirmState();
}

void MainWindow::refreshHandCards()
{
    for (auto card : handCardWidgets) {
        delete card;
    }
    handCardWidgets.clear();

    Player* currentPlayer = isWeiYanTurn ? (Player*)weiyan : (Player*)xusheng;
    
    for (size_t i = 0; i < currentPlayer->handCards.size(); ++i) {
        QString cardName = currentPlayer->handCards[i]->name;
        CardWidget* cardWidget = new CardWidget(cardName);
        
        QString imgPath = getCardImagePath(cardName);
        QPixmap pixmap(imgPath);
        if (!pixmap.isNull()) {
            cardWidget->setPixmap(pixmap.scaled(cardWidget->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(cardName);
            cardWidget->setStyleSheet("QLabel { border: 2px solid rgba(176, 139, 85, 0.95); border-radius: 10px; background: rgba(22, 18, 15, 0.85); color: #F5E6C8; font-size: 16px; text-align: center; }");
            cardWidget->setAlignment(Qt::AlignCenter);
        }

        bool playable = isCardPlayable(cardName);
        cardWidget->setEnabled(playable);
        if (!playable) {
            cardWidget->setStyleSheet("QLabel { border: 2px solid rgba(130, 120, 100, 0.45); border-radius: 10px; background: rgba(12, 10, 9, 0.7); color: rgba(220, 210, 190, 0.5); }");
        }

        if (!pendingCardName.isEmpty() && pendingCardName == cardName && selectedHandCard == nullptr && playable) {
            cardWidget->selected = true;
            selectedHandCard = cardWidget;
            cardWidget->setStyleSheet("QLabel { border: 3px solid #F2C66D; border-radius: 10px; background: rgba(22, 18, 15, 0.85); color: #FFF3D0; }");
        }
        
        handCardsLayout->addWidget(cardWidget);
        handCardWidgets.append(cardWidget);
        connect(cardWidget, &CardWidget::clicked, this, &MainWindow::onCardClicked);
    }
}

QString MainWindow::getCardImagePath(const QString& cardName) {
    QString name = cardName;
    if (name == "杀") return imagePath + "杀.jpg";
    if (name == "闪") return imagePath + "闪.jpg";
    if (name == "桃") return imagePath + "桃.jpg";
    if (name == "酒") return imagePath + "酒.jpg";
    if (name == "决斗") return imagePath + "决斗.jpg";
    if (name == "南蛮入侵") return imagePath + "南蛮入侵.jpg";
    if (name == "万箭齐发") return imagePath + "万箭齐发.jpg";
    if (name == "顺手牵羊") return imagePath + "顺手牵羊.jpg";
    if (name == "过河拆桥") return imagePath + "过河拆桥.jpg";
    if (name == "诸葛连弩") return imagePath + "诸葛连弩.jpg";
    if (name == "古锭刀") return imagePath + "古锭刀.jpg";
    if (name == "麒麟弓") return imagePath + "麒麟弓.jpg";
    if (name == "仁王盾") return imagePath + "仁王盾.jpg";
    if (name == "白银狮子") return imagePath + "白银狮子.jpg";
    if (name == "+1马") return imagePath + "加一马.jpg";
    if (name == "-1马") return imagePath + "减一马.jpg";
    return "";
}

void MainWindow::onCardClicked(CardWidget* card) {
    if (!isCardPlayable(card->cardName)) {
        card->selected = false;
        pendingCardName.clear();
        selectedHandCard = nullptr;
        appendLog("当前阶段不可使用【" + card->cardName + "】");
        updateConfirmState();
        return;
    }

    for (auto* w : handCardWidgets) {
        if (w != card) {
            w->selected = false;
            w->setGraphicsEffect(nullptr);
            if (w->isEnabled()) {
                w->setStyleSheet("QLabel { border: 2px solid rgba(176, 139, 85, 0.95); border-radius: 10px; background: rgba(22, 18, 15, 0.85); color: #F5E6C8; }");
            }
        }
    }

    if (card->selected) {
        selectedHandCard = card;
        pendingCardName = card->cardName;
        appendLog("选择：【" + card->cardName + "】");
    } else {
        selectedHandCard = nullptr;
        pendingCardName.clear();
    }
    updateConfirmState();
}

bool MainWindow::isCardPlayable(const QString& cardName) const {
    if (!isWeiYanTurn || kuangGuTriggered) return false;
    if (cardName == "闪") return false;
    if (cardName == "桃") return weiyan && weiyan->hp < weiyan->maxHp;
    if (cardName == "杀") return weiyan && weiyan->slashUsedThisTurn < weiyan->slashCountLimit;
    return true;
}

void MainWindow::updateConfirmState() {
    bool canConfirm = isWeiYanTurn && selectedHandCard && !pendingCardName.isEmpty() && isCardPlayable(pendingCardName);
    btnConfirm->setEnabled(canConfirm);
    btnCancel->setEnabled(selectedHandCard != nullptr);
}

void MainWindow::resolveSelectedCardPlay() {
    if (!selectedHandCard || pendingCardName.isEmpty()) return;
    QString cardToPlay = pendingCardName;
    pendingCardName.clear();
    selectedHandCard = nullptr;

    if (cardToPlay == "杀") {
        onWeiYanSlash();
    } else if (cardToPlay == "桃") {
        if (removeCardFromHand(weiyan, "桃")) {
            weiyan->heal(1);
            tableCardLeft->setPixmap(QPixmap(getCardImagePath("桃")).scaled(tableCardLeft->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            tableCardLeft->setVisible(true);
            appendLog("势·魏延使用【桃】回复1点体力");
            discardPileCount += 1;
            discardPileLabel->setText("弃牌堆：" + QString::number(discardPileCount));
            QTimer::singleShot(800, this, &MainWindow::clearTableCards);
        }
    } else if (removeCardFromHand(weiyan, cardToPlay)) {
        tableCardLeft->setPixmap(QPixmap(getCardImagePath(cardToPlay)).scaled(tableCardLeft->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        tableCardLeft->setVisible(true);
        appendLog("势·魏延使用【" + cardToPlay + "】");
        discardPileCount += 1;
        discardPileLabel->setText("弃牌堆：" + QString::number(discardPileCount));
        QTimer::singleShot(800, this, &MainWindow::clearTableCards);
    }

    updateUI();
    checkGameOver();
}

bool MainWindow::removeCardFromHand(Player* player, const QString& cardName) {
    if (!player) return false;
    for (auto it = player->handCards.begin(); it != player->handCards.end(); ++it) {
        if ((*it)->name == cardName) {
            player->handCards.erase(it);
            return true;
        }
    }
    return false;
}

void MainWindow::clearTableCards() {
    tableCardLeft->clear();
    tableCardRight->clear();
    tableCardLeft->setVisible(false);
    tableCardRight->setVisible(false);
}

void MainWindow::appendLog(const QString& text)
{
    txtLog->append(text);
    txtLog->verticalScrollBar()->setValue(txtLog->verticalScrollBar()->maximum());
}

void MainWindow::checkGameOver()
{
    if (weiyan->hp <= 0) {
        lblPhase->setText("游戏结束 - 界·徐盛 获胜！");
        lblPhase->setStyleSheet("font-size: 20px; font-weight: bold; color: #2196F3; text-shadow: 2px 2px 4px #000;");
        QMessageBox::information(this, "游戏结束", "界·徐盛 获胜！");
        btnStart->setEnabled(true);
        btnZhuangShi->setEnabled(false);
        btnSlash->setEnabled(false);
        btnBeiShui->setEnabled(false);
        btnXuShengSlash->setEnabled(false);
        btnEndTurn->setEnabled(false);
    } else if (xusheng->hp <= 0) {
        lblPhase->setText("游戏结束 - 势·魏延 获胜！");
        lblPhase->setStyleSheet("font-size: 20px; font-weight: bold; color: #F44336; text-shadow: 2px 2px 4px #000;");
        QMessageBox::information(this, "游戏结束", "势·魏延 获胜！");
        btnStart->setEnabled(true);
        btnZhuangShi->setEnabled(false);
        btnSlash->setEnabled(false);
        btnBeiShui->setEnabled(false);
        btnXuShengSlash->setEnabled(false);
        btnEndTurn->setEnabled(false);
    }
}