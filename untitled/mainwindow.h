#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTimer>
#include <QScrollBar>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include "Player.h"
#include "WeiYan.h"
#include "XuSheng.h"
#include "DeckManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class CardWidget : public QLabel {
    Q_OBJECT
public:
    CardWidget(const QString& cardName, QWidget* parent = nullptr);
    QString cardName;
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
    void onWeiYanSlash();
    void onWeiYanZhuangShi();
    void onWeiYanKuangGuHeal();
    void onWeiYanKuangGuDraw();
    void onWeiYanBeiShui();
    void onXuShengSlash();
    void onEndTurn();
    void updateUI();
    void appendLog(const QString& text);
    void onCardClicked(CardWidget* card);
    void showSkillAnimation(const QString& skillName, const QString& heroName);

private:
    Ui::MainWindow *ui;

    WeiYan* weiyan;
    XuSheng* xusheng;
    DeckManager* deck;
    bool isWeiYanTurn;
    bool zhuangShiUsed;
    bool kuangGuTriggered;
    bool beiShuiAsked;

    QString imagePath;

    QLabel* lblPhase;
    QTextEdit* txtLog;
    QLabel* skillAnimationLabel;
    QTimer* animationTimer;

    HeroCardWidget* heroXuSheng;
    HeroCardWidget* heroWeiYan;
    HeroCardWidget* heroSelf;

    QWidget* handCardsArea;
    QHBoxLayout* handCardsLayout;
    QList<CardWidget*> handCardWidgets;

    QPushButton* btnStart;
    QPushButton* btnZhuangShi;
    QPushButton* btnSlash;
    QPushButton* btnBeiShui;
    QPushButton* btnXuShengSlash;
    QPushButton* btnEndTurn;
    QPushButton* btnKuangGuHeal;
    QPushButton* btnKuangGuDraw;
    QPushButton* btnConfirm;
    QPushButton* btnCancel;

    void initUI();
    void resetGame();
    void startWeiYanTurn();
    void startXuShengTurn();
    void checkGameOver();
    void refreshHandCards();
    QString getCardImagePath(const QString& cardName);
};

#endif // MAINWINDOW_H