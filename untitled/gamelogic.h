#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <vector>
#include <QString>
#include <QDebug>

enum CardType { SLASH, DODGE, PEACH, OTHER };
struct Card {
    CardType type;
    QString name;
};

enum MissionState { START, SUCCESS, FAILURE };

class Player {
public:
    QString name;
    int hp;
    int maxHp;
    std::vector<Card> handCards;

    Player(QString n, int h) : name(n), hp(h), maxHp(h) {}
    void takeDamage(int amount) {
        hp -= amount;
        if (hp < 0) hp = 0; // <--- 增加这一行：血量地板，最低为 0
    }
    bool isAlive() const {
        return hp > 0;
    }
    void heal(int amount) { if(hp < maxHp) hp += amount; }
    void drawCard() { handCards.push_back({OTHER, "随机牌"}); }
    void discardRandom(int count) {
        for(int i=0; i<count && !handCards.empty(); ++i) handCards.pop_back();
    }
};

class WeiYan : public Player {
public:
    WeiYan() : Player("势·魏延", 5), currentState(START) {}

    MissionState currentState;
    int zhuangShi_discarded = 0;
    int zhuangShi_hpLost = 0;
    int cardsUsedThisTurn = 0;
    bool usedZhuangShiThisTurn = false;

    // --- 【壮誓】: 玩家在出牌阶段开始时手动调用 ---
    void activateZhuangShi(int discard, int hpLoss);

    // --- 【饮战】: 锁定技，自动触发判定 ---
    void useSlash(Player* target);

    // --- 【狂骨】: 触发后，等待玩家选择 ---
    // 逻辑层不再自动执行，而是提供两个接口供界面调用
    void chooseKuangGuHeal(); // 玩家选择：回复1点体力
    void chooseKuangGuDraw(); // 玩家选择：摸1张牌

    // --- 【背水】: 玩家手动选择是否发动 ---
    // 弃1张牌，本回合【杀】次数+1
    bool activateBeiShui();

    // --- 【忠傲】: 状态管理 ---
    void checkKill(Player* target);
    void endTurn();

    void updateStatus();
};

#endif