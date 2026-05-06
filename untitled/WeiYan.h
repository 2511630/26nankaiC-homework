#ifndef WEIYAN_H
#define WEIYAN_H

#include "Player.h"
#include "Card.h"
#include <memory>

enum MissionState { START, SUCCESS, FAILURE };

class WeiYan : public Player {
public:
    // 初始化：名字 势·魏延, 体力 5, 初始状态 START
    WeiYan() : Player("势·魏延", 5), currentState(START) {}

    // --- 状态变量 ---
    MissionState currentState;
    int zhuangShi_discarded = 0;   // 壮誓投入的弃牌数
    int zhuangShi_hpLost = 0;      // 壮誓投入的失血量
    int cardsUsedThisTurn = 0;     // 本回合实际用牌数
    int slashUsedThisTurn = 0;     // 本回合使用【杀】次数
    int slashCountLimit = 1;       // 本回合【杀】次数上限
    bool usedZhuangShiThisTurn = false;

    // --- 核心技能函数 ---
    void activateZhuangShi(int discard, int hpLoss);
    void useSlash(Player* target, int baseDamage = 1);
    void chooseKuangGuHeal();
    void chooseKuangGuDraw();
    bool activateBeiShui(bool shouldActivate);
    void checkKill(Player* target);
    void endTurn();
    void updateStatus();
    void startTurn();
};

#endif