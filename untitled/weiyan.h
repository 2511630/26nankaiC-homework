#ifndef WEIYAN_H
#define WEIYAN_H

#include "Player.h"
#include "Card.h" // 显式包含，解决 OTHER 找不到的问题

enum MissionState { START, SUCCESS, FAILURE };

class WeiYan : public Player {
public:
    WeiYan() : Player("势·魏延", 5), currentState(START) {}

    // --- 状态变量 ---
    MissionState currentState;
    int zhuangShi_discarded = 0;
    int zhuangShi_hpLost = 0;
    int cardsUsedThisTurn = 0;
    bool usedZhuangShiThisTurn = false;

    // --- 所有的函数必须在这里声明，main.cpp 才能看到 ---
    void activateZhuangShi(int discard, int hpLoss);
    void useSlash(Player* target);                       // <--- 之前漏掉了
    void chooseKuangGuHeal();                            // <--- 之前漏掉了
    void chooseKuangGuDraw();                            // <--- 之前漏掉了
    bool activateBeiShui();                              // <--- 之前漏掉了
    void checkKill(Player* target);
    void endTurn();
    void updateStatus();
};

#endif