#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QScrollBar>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QSpinBox>
#include <vector>
#include "Player.h"
#include "WeiYan.h"
#include "XuSheng.h"
#include "DeckManager.h"
#include "AIManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum class FlowState {
    START_MENU,
    TURN_DRAW,
    TURN_PLAY_SELECT,
    TURN_PLAY_CONFIRM,
    WAIT_RESPONSE,
    RESOLVE,
    TURN_END,
    DISCARD_PHASE,
    ZHUANGSHI_STEP1,
    ZHUANGSHI_STEP2,
    ZHUANGSHI_DISCARD,
    NEAR_DEATH,
    SELECT_TRICK_TARGET,
    SELECT_POJUN_TARGET,
};

class CardWidget : public QLabel {
    Q_OBJECT
public:
    CardWidget(const QString& cardName, int index, QWidget* parent = nullptr);
    QString cardName;
    int index;
    bool selected;
signals:
    void clicked(CardWidget* card);
protected:
    void mousePressEvent(QMouseEvent* event) override;
};

class HeroCardWidget : public QWidget {
    Q_OBJECT
public:
    HeroCardWidget(const QString& heroName, const QString& imgPath, int maxHp, QWidget* parent = nullptr);
    void setHp(int hp);
    void setSelected(bool selected);
    QString heroName;
private:
    QLabel* heroImage;
    QLabel* hpLabel;
    QHBoxLayout* hpLayout;
    QList<QLabel*> hpIcons;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onStartGame();
    void onWeiYanZhuangShi();
    void onWeiYanKuangGuHeal();
    void onWeiYanKuangGuDraw();
    void onWeiYanKuangGuSkip();
    void onWeiYanBeiShui();
    void onEndTurn();
    void updateUI();
    void appendLog(const QString& text);
    void onCardClicked(CardWidget* card);
    void showSkillAnimation(const QString& skillName, const QString& heroName);
    void onZhuangShiStep1Confirm();
    void onZhuangShiStep2Confirm();
    void onZhuangShiDiscardConfirm();
    void onZhuangShiCancel();
    void clearTableCards();
    void handleNearDeath(Player* player);
    void startDiscardPhase(Player* player);
    void onDiscardPhaseConfirm();
    void useCardEffect(std::shared_ptr<Card> card, Player* source, Player* target);
    void useSlashEffect(Player* source, Player* target, bool useWeiYanSkill);
    void onTargetCardClicked(CardWidget* card);
    void showTargetCardSelection(Player* target, const QString& cardName);
    void showPojunCardSelection(Player* target);
    void onPojunCardClicked(CardWidget* card);
    void onPojunConfirm();
    void startAITurn();
    void onAIDecisionReceived(const QString& decision);
    QString getGameStateForAI() const;

private:
    Ui::MainWindow *ui;

    WeiYan* weiyan;
    XuSheng* xusheng;
    DeckManager* deck;
    bool isWeiYanTurn;
    bool zhuangShiUsed;
    bool beiShuiUsed;
    int selectedHandCardIndex;
    int discardPileCount;
    bool weiyanWineUsedThisTurn;
    bool xushengWineUsedThisTurn;
    bool weiyanWineBuffActive;
    bool xushengWineBuffActive;
    FlowState flowState;
    QString pendingCardName;
    QString pendingResponseCardName;

    QString imagePath;

    QLabel* lblPhase;
    QTextEdit* txtLog;
    QLabel* skillAnimationLabel;
    QTimer* animationTimer;

    HeroCardWidget* heroXuSheng;
    HeroCardWidget* heroWeiYan;

    QLabel* lblHandCardCount;
    QLabel* lblWeiYanWeapon;
    QLabel* lblWeiYanArmor;
    QLabel* lblWeiYanHorse;

    QLabel* lblXuShengHandCardCount;
    QLabel* lblXuShengWeapon;
    QLabel* lblXuShengArmor;
    QLabel* lblXuShengHorse;

    QWidget* targetSelectPanel;
    QLabel* lblTargetSelect;
    QList<CardWidget*> targetCardWidgets;
    QString currentTrickCardName;

    QWidget* handCardsArea;
    QHBoxLayout* handCardsLayout;
    QList<CardWidget*> handCardWidgets;

    QLabel* tableCardLeft;
    QLabel* tableCardRight;
    QLabel* discardPileLabel;

    QPushButton* btnStart;
    QPushButton* btnZhuangShi;
    QPushButton* btnBeiShui;
    QPushButton* btnEndTurn;
    QPushButton* btnConfirm;
    QPushButton* btnCancel;

    QPushButton* btnKuangGuHeal;
    QPushButton* btnKuangGuDraw;
    QPushButton* btnKuangGuSkip;

    QWidget* zhuangShiPanel;
    QSpinBox* spinZhuangShiDiscard;
    QSpinBox* spinZhuangShiHpLoss;
    QPushButton* btnZhuangShiConfirm;
    QPushButton* btnZhuangShiCancel;
    QLabel* lblZhuangShiStep;
    QLabel* lblDiscard;
    QLabel* lblHpLoss;

    int zhuangShiDiscardCount;
    int zhuangShiHpLoss;
    int discardPhaseCount;
    std::vector<int> selectedDiscardIndices;

    Player* pojunTarget;
    int pojunKouZhiCount;
    QLabel* lblPojunInfo;
    QWidget* pojunSelectPanel;
    QList<CardWidget*> pojunCardWidgets;
    std::vector<int> pojunSelectedIndices;

    AIManager* aiManager;
    bool isAIThinking;
    QString aiApiKey;

    void initUI();
    void resetGame();
    void startWeiYanTurn();
    void startXuShengTurn();
    void checkGameOver();
    void refreshHandCards();
    QString getCardImagePath(const QString& cardName);
    bool isCardPlayable(const QString& cardName) const;
    void updateConfirmState();
    void updateEquipmentDisplay();
    void resolveSelectedCardPlay();
    void enterFlowState(FlowState state);
    Player* currentPlayer() const;
    Player* enemyPlayer() const;
    void showCardOnTable(QLabel* slot, const QString& cardName);
    void updateSkillPanelVisibility();
    void startZhuangShiStep1();
    void startZhuangShiStep2();
    void startZhuangShiDiscard();
};

#endif // MAINWINDOW_H