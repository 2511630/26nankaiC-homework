#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPalette>
#include <QBrush>
#include <QPixmap>
#include <QPainter>

CardWidget::CardWidget(const QString& cardName, QWidget* parent) : QLabel(parent), cardName(cardName), selected(false) {
    setFixedSize(75, 110);
    setStyleSheet("QLabel { border: 3px solid #5D4037; border-radius: 10px; background: #8D6E63; }");
    setCursor(Qt::PointingHandCursor);
}

void CardWidget::mousePressEvent(QMouseEvent* event) {
    selected = !selected;
    if (selected) {
        setStyleSheet("QLabel { border: 4px solid #FFD700; border-radius: 10px; background: #8D6E63; transform: translateY(-10px); }");
    } else {
        setStyleSheet("QLabel { border: 3px solid #5D4037; border-radius: 10px; background: #8D6E63; }");
    }
    emit clicked(this);
    QLabel::mousePressEvent(event);
}

HeroCardWidget::HeroCardWidget(const QString& heroName, const QString& imgPath, int maxHp, QWidget* parent) 
    : QWidget(parent), heroName(heroName) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->setSpacing(0);

    heroImage = new QLabel();
    heroImage->setFixedSize(70, 90);
    heroImage->setScaledContents(true);
    heroImage->setStyleSheet("QLabel { border: 2px solid #4CAF50; border-radius: 8px; }");
    
    QPixmap pixmap(imgPath);
    if (!pixmap.isNull()) {
        heroImage->setPixmap(pixmap.scaled(70, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    layout->addWidget(heroImage, 0, Qt::AlignCenter);

    hpLayout = new QHBoxLayout();
    hpLayout->setSpacing(1);
    hpLayout->setAlignment(Qt::AlignCenter);

    for (int i = 0; i < maxHp; ++i) {
        QLabel* hpIcon = new QLabel();
        hpIcon->setFixedSize(14, 14);
        QPixmap hpPixmap("D:/OneDrive/Desktop/GitHub/26nankaiC-homework/C++作业图片/red_hp.png");
        if (!hpPixmap.isNull()) {
            hpIcon->setPixmap(hpPixmap.scaled(14, 14, Qt::KeepAspectRatio));
        }
        hpIcons.append(hpIcon);
        hpLayout->addWidget(hpIcon);
    }
    
    layout->addLayout(hpLayout);
    
    hpLabel = new QLabel(heroName);
    hpLabel->setStyleSheet("QLabel { font-size: 11px; color: #FFF; text-align: center; font-weight: bold; }");
    hpLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(hpLabel);

    setLayout(layout);
}

void HeroCardWidget::setHp(int hp) {
    for (int i = 0; i < hpIcons.size(); ++i) {
        QString path = (i < hp) ? "D:/OneDrive/Desktop/GitHub/26nankaiC-homework/C++作业图片/red_hp.png" : "D:/OneDrive/Desktop/GitHub/26nankaiC-homework/C++作业图片/green_hp.png";
        QPixmap pixmap(path);
        if (!pixmap.isNull()) {
            hpIcons[i]->setPixmap(pixmap.scaled(14, 14, Qt::KeepAspectRatio));
        }
    }
}

void HeroCardWidget::setSelected(bool selected) {
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();
    effect->setColor(selected ? Qt::yellow : Qt::transparent);
    effect->setBlurRadius(selected ? 20 : 0);
    heroImage->setGraphicsEffect(effect);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    imagePath = "D:/OneDrive/Desktop/GitHub/26nankaiC-homework/C++作业图片/";
    setWindowTitle("三国杀 - 势·魏延 VS 界·徐盛");
    setFixedSize(1024, 600);
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
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QString bgPath = imagePath + "战局背景.jpg";
    QPalette palette;
    QPixmap bgImage(bgPath);
    if (bgImage.isNull()) {
        palette.setColor(QPalette::Window, QColor(26, 26, 26));
    } else {
        palette.setBrush(QPalette::Window, bgImage.scaled(1024, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    centralWidget->setPalette(palette);
    centralWidget->setAutoFillBackground(true);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setContentsMargins(20, 15, 20, 0);
    topLayout->setSpacing(30);
    topLayout->setAlignment(Qt::AlignCenter);

    heroXuSheng = new HeroCardWidget("界·徐盛", imagePath + "界徐盛.webp", 4);
    heroXuSheng->setHp(4);
    topLayout->addWidget(heroXuSheng);

    QLabel* vsLabel = new QLabel("VS");
    vsLabel->setStyleSheet("font-size: 36px; font-weight: bold; color: #FFD700; text-shadow: 2px 2px 4px #DC143C;");
    topLayout->addWidget(vsLabel);

    heroWeiYan = new HeroCardWidget("势·魏延", imagePath + "4.webp", 5);
    heroWeiYan->setHp(5);
    topLayout->addWidget(heroWeiYan);

    mainLayout->addLayout(topLayout);

    QHBoxLayout* centerLayout = new QHBoxLayout();
    centerLayout->setContentsMargins(10, 10, 10, 10);
    centerLayout->setSpacing(10);

    QVBoxLayout* leftPanel = new QVBoxLayout();
    leftPanel->setSpacing(8);

    QPushButton* btnPaiXu = new QPushButton("牌序");
    btnPaiXu->setFixedSize(60, 35);
    btnPaiXu->setStyleSheet("QPushButton { background: rgba(139, 69, 19, 0.85); color: #FFD700; font-weight: bold; font-size: 12px; border-radius: 4px; border: 1px solid #8B4513; }");
    leftPanel->addWidget(btnPaiXu);

    QPushButton* btnPaiKu = new QPushButton("牌库");
    btnPaiKu->setFixedSize(60, 35);
    btnPaiKu->setStyleSheet("QPushButton { background: rgba(139, 69, 19, 0.85); color: #FFD700; font-weight: bold; font-size: 12px; border-radius: 4px; border: 1px solid #8B4513; }");
    leftPanel->addWidget(btnPaiKu);

    QPushButton* btnJiLu = new QPushButton("记录");
    btnJiLu->setFixedSize(60, 35);
    btnJiLu->setStyleSheet("QPushButton { background: rgba(139, 69, 19, 0.85); color: #FFD700; font-weight: bold; font-size: 12px; border-radius: 4px; border: 1px solid #8B4513; }");
    leftPanel->addWidget(btnJiLu);

    centerLayout->addLayout(leftPanel);

    QVBoxLayout* centerPanel = new QVBoxLayout();
    centerPanel->setSpacing(10);

    lblPhase = new QLabel("点击开始游戏");
    lblPhase->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFD700; text-shadow: 2px 2px 4px #000;");
    lblPhase->setAlignment(Qt::AlignCenter);
    centerPanel->addWidget(lblPhase);

    skillAnimationLabel = new QLabel();
    skillAnimationLabel->setFixedSize(450, 150);
    skillAnimationLabel->setAlignment(Qt::AlignCenter);
    skillAnimationLabel->setVisible(false);
    centerPanel->addWidget(skillAnimationLabel, 0, Qt::AlignCenter);

    QHBoxLayout* actionButtons = new QHBoxLayout();
    actionButtons->setSpacing(25);
    actionButtons->setAlignment(Qt::AlignCenter);

    btnConfirm = new QPushButton("确定");
    btnConfirm->setFixedSize(130, 45);
    btnConfirm->setStyleSheet("QPushButton { background: linear-gradient(#4CAF50, #388E3C); color: white; font-weight: bold; font-size: 15px; border-radius: 8px; border: 2px solid #2E7D32; }");
    btnConfirm->setEnabled(false);
    actionButtons->addWidget(btnConfirm);

    btnCancel = new QPushButton("取消");
    btnCancel->setFixedSize(130, 45);
    btnCancel->setStyleSheet("QPushButton { background: linear-gradient(#F44336, #D32F2F); color: white; font-weight: bold; font-size: 15px; border-radius: 8px; border: 2px solid #C62828; }");
    btnCancel->setEnabled(false);
    actionButtons->addWidget(btnCancel);

    centerPanel->addLayout(actionButtons);

    QHBoxLayout* skillButtons = new QHBoxLayout();
    skillButtons->setSpacing(12);
    skillButtons->setAlignment(Qt::AlignCenter);

    btnStart = new QPushButton("开始游戏");
    btnStart->setFixedSize(100, 40);
    btnStart->setStyleSheet("QPushButton { background: linear-gradient(#4CAF50, #388E3C); color: white; font-weight: bold; font-size: 13px; border-radius: 6px; }");
    connect(btnStart, &QPushButton::clicked, this, &MainWindow::onStartGame);
    skillButtons->addWidget(btnStart);

    btnZhuangShi = new QPushButton("壮誓");
    btnZhuangShi->setFixedSize(80, 40);
    btnZhuangShi->setStyleSheet("QPushButton { background: linear-gradient(#E65100, #BF360C); color: white; font-weight: bold; font-size: 13px; border-radius: 6px; }");
    btnZhuangShi->setEnabled(false);
    connect(btnZhuangShi, &QPushButton::clicked, this, &MainWindow::onWeiYanZhuangShi);
    skillButtons->addWidget(btnZhuangShi);

    btnSlash = new QPushButton("出杀");
    btnSlash->setFixedSize(80, 40);
    btnSlash->setStyleSheet("QPushButton { background: linear-gradient(#E65100, #BF360C); color: white; font-weight: bold; font-size: 13px; border-radius: 6px; }");
    btnSlash->setEnabled(false);
    connect(btnSlash, &QPushButton::clicked, this, &MainWindow::onWeiYanSlash);
    skillButtons->addWidget(btnSlash);

    btnBeiShui = new QPushButton("背水");
    btnBeiShui->setFixedSize(80, 40);
    btnBeiShui->setStyleSheet("QPushButton { background: linear-gradient(#E65100, #BF360C); color: white; font-weight: bold; font-size: 13px; border-radius: 6px; }");
    btnBeiShui->setEnabled(false);
    connect(btnBeiShui, &QPushButton::clicked, this, &MainWindow::onWeiYanBeiShui);
    skillButtons->addWidget(btnBeiShui);

    btnXuShengSlash = new QPushButton("徐盛出杀");
    btnXuShengSlash->setFixedSize(90, 40);
    btnXuShengSlash->setStyleSheet("QPushButton { background: linear-gradient(#1E88E5, #1565C0); color: white; font-weight: bold; font-size: 13px; border-radius: 6px; }");
    btnXuShengSlash->setEnabled(false);
    connect(btnXuShengSlash, &QPushButton::clicked, this, &MainWindow::onXuShengSlash);
    skillButtons->addWidget(btnXuShengSlash);

    btnEndTurn = new QPushButton("结束回合");
    btnEndTurn->setFixedSize(100, 40);
    btnEndTurn->setStyleSheet("QPushButton { background: linear-gradient(#616161, #424242); color: white; font-weight: bold; font-size: 13px; border-radius: 6px; }");
    btnEndTurn->setEnabled(false);
    connect(btnEndTurn, &QPushButton::clicked, this, &MainWindow::onEndTurn);
    skillButtons->addWidget(btnEndTurn);

    centerPanel->addLayout(skillButtons);

    QHBoxLayout* kuangGuButtons = new QHBoxLayout();
    kuangGuButtons->setSpacing(20);
    kuangGuButtons->setAlignment(Qt::AlignCenter);

    btnKuangGuHeal = new QPushButton("回复1点体力");
    btnKuangGuHeal->setFixedSize(120, 38);
    btnKuangGuHeal->setStyleSheet("QPushButton { background: linear-gradient(#388E3C, #2E7D32); color: white; font-weight: bold; font-size: 12px; border-radius: 5px; }");
    btnKuangGuHeal->setEnabled(false);
    connect(btnKuangGuHeal, &QPushButton::clicked, this, &MainWindow::onWeiYanKuangGuHeal);
    kuangGuButtons->addWidget(btnKuangGuHeal);

    btnKuangGuDraw = new QPushButton("摸一张牌");
    btnKuangGuDraw->setFixedSize(120, 38);
    btnKuangGuDraw->setStyleSheet("QPushButton { background: linear-gradient(#FFA000, #FF8F00); color: black; font-weight: bold; font-size: 12px; border-radius: 5px; }");
    btnKuangGuDraw->setEnabled(false);
    connect(btnKuangGuDraw, &QPushButton::clicked, this, &MainWindow::onWeiYanKuangGuDraw);
    kuangGuButtons->addWidget(btnKuangGuDraw);

    centerPanel->addLayout(kuangGuButtons);

    centerLayout->addLayout(centerPanel);

    QVBoxLayout* rightPanel = new QVBoxLayout();
    rightPanel->setSpacing(8);

    heroSelf = new HeroCardWidget("势·魏延", imagePath + "4.webp", 5);
    heroSelf->setHp(5);
    rightPanel->addWidget(heroSelf);

    txtLog = new QTextEdit();
    txtLog->setReadOnly(true);
    txtLog->setFixedSize(140, 180);
    txtLog->setStyleSheet("QTextEdit { background: rgba(0,0,0,0.8); color: #FFF; font-size: 11px; border: 1px solid #4CAF50; border-radius: 5px; }");
    rightPanel->addWidget(txtLog);

    centerLayout->addLayout(rightPanel);
    mainLayout->addLayout(centerLayout);

    handCardsArea = new QWidget();
    handCardsArea->setFixedHeight(130);
    handCardsLayout = new QHBoxLayout(handCardsArea);
    handCardsLayout->setContentsMargins(40, 10, 40, 10);
    handCardsLayout->setSpacing(6);
    handCardsLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(handCardsArea);

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
    weiyan->startTurn();

    lblPhase->setText("出牌阶段 - 势·魏延");
    lblPhase->setStyleSheet("font-size: 18px; font-weight: bold; color: #F44336; text-shadow: 2px 2px 4px #000;");

    btnZhuangShi->setEnabled(!zhuangShiUsed && weiyan->currentState != SUCCESS);
    btnSlash->setEnabled(true);
    btnBeiShui->setEnabled(weiyan->currentState == SUCCESS && !beiShuiAsked && weiyan->handCards.size() >= 1);
    btnXuShengSlash->setEnabled(false);
    btnEndTurn->setEnabled(true);
    btnConfirm->setEnabled(true);
    btnCancel->setEnabled(true);

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
    kuangGuTriggered = false;
    weiyan->useSlash(xusheng);

    if (xusheng->hp <= 0) {
        weiyan->checkKill(xusheng);
        appendLog("【忠傲】使命成功！");
    } else if (weiyan->currentState == START) {
        appendLog(">>> 触发【狂骨】，请选择：");
        kuangGuTriggered = true;
        btnKuangGuHeal->setEnabled(true);
        btnKuangGuDraw->setEnabled(true);
        btnSlash->setEnabled(false);
        btnEndTurn->setEnabled(false);
    }

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
            cardWidget->setPixmap(pixmap.scaled(75, 110, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            cardWidget->setText(cardName);
            cardWidget->setStyleSheet("QLabel { border: 3px solid #5D4037; border-radius: 10px; background: #8D6E63; color: white; font-size: 13px; text-align: center; }");
            cardWidget->setAlignment(Qt::AlignCenter);
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
    appendLog("选择：【" + card->cardName + "】");
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