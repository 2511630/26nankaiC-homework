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
#include <QLineEdit>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <vector>
#include "Player.h"
#include "WeiYan.h"
#include "XuSheng.h"
#include "DeckManager.h"

enum class FlowState {
    START_MENU,
    TURN_DRAW,
    TURN_PLAY_SELECT,
    TURN_PLAY_CONFIRM,
    WAIT_RESPONSE,
    RESOLVE,
    TURN_END,
    DISCARD_PHASE,
    ZHUANGSHI_SELECT,
    ZHUANGSHI_DISCARD,
    NEAR_DEATH,
    SELECT_TRICK_TARGET,
    SELECT_POJUN_TARGET,
    ZHUANGSHI_SELECT_CARDS,
    ZHUANGSHI_SELECT_HP,
    SELECT_YINGZHAN_CARD,
    KUNFEN_END_PHASE,
    POJUN_SELECT,
    POJUN_DODGE_WAIT,
};

class CardWidget : public QLabel {
    Q_OBJECT
public:
    CardWidget(const QString& cardName, int index, QWidget* parent = nullptr);
    void setSelected(bool selected);
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
    void onZhuangShiConfirm();
    void onZhuangShiCancel();
    void clearTableCards();
    void handleNearDeath(Player* player);
    void startDiscardPhase(Player* player);
    void onDiscardPhaseConfirm();
    bool useCardEffect(std::shared_ptr<Card> card, Player* source, Player* target, bool zhuangShiNoLimitActive = false);
    void useSlashEffect(Player* source, Player* target, bool useWeiYanSkill);
    void onTargetCardClicked(CardWidget* card);
    void showTargetCardSelection(Player* target, const QString& cardName);
    void showPojunCardSelection(Player* target);
    void onPojunCardClicked(CardWidget* card);
    void onPojunConfirm();
    void onKunFenEndPhaseConfirm();
    void showPojunDodgeOption(Player* target);
    void resolvePojunSelect();
    void resolvePojunNoDodge(Player* target);
    void resolvePojunDodge();

private:
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
    QString audioPath;

    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;
    Player* nearDeathPlayer;

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

    QLabel* lblDeckCount;
    QLabel* lblDeckCard;

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
    QLineEdit* editZhuangShiHpLoss;
    QPushButton* btnZhuangShiConfirm;
    QPushButton* btnZhuangShiCancel;
    QLabel* lblZhuangShiStep;

    int zhuangShiDiscardCount;
    int discardPhaseCount;
    std::vector<int> selectedDiscardIndices;
    std::vector<int> zhuangShiSelectedCards;
    int kunFenDiscardCount;
    bool inKunFenNearDeath;

    Player* pojunTarget;
    int pojunKouZhiCount;
    QLabel* lblPojunInfo;
    QWidget* pojunSelectPanel;
    QList<CardWidget*> pojunCardWidgets;
    std::vector<int> pojunSelectedIndices;
    
    Player* yingZhanTarget;
    int yingZhanSelectedIndex;

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
    void updateDeckDisplay();
    void resolveSelectedCardPlay();
    void enterFlowState(FlowState state);
    Player* currentPlayer() const;
    Player* enemyPlayer() const;
    void showCardOnTable(QLabel* slot, const QString& cardName);
    void updateSkillPanelVisibility();
    void triggerKuangGu(WeiYan* wy, Player* target, int damage);
    void showYingZhanCardSelection(Player* target);
    void onYingZhanCardSelected();
    void playAudio(const QString& audioName);
};

#endif // MAINWINDOW_H
