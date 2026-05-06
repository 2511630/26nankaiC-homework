#ifndef WEIYAN_H
#define WEIYAN_H

#include "Player.h"
#include "Card.h"
#include <memory>

enum State { START, SUCCESS, FAILURE };

class WeiYan : public Player {
public:
    WeiYan() : Player("势·魏延", 5), currentState(START), kuangGuCanTrigger(false), chengShiCanTrigger(false), zhuangShiFailed(false) {}

    State currentState;
    int zhuangShi_discarded = 0;
    int zhuangShi_hpLost = 0;
    int cardsUsedThisTurn = 0;
    int slashUsedThisTurn = 0;
    int slashCountLimit = 1;
    bool usedZhuangShiThisTurn = false;
    bool kuangGuCanTrigger = false;
    bool chengShiCanTrigger = false;
    std::shared_ptr<Card> chengShiDiscardedCard = nullptr;
    bool zhuangShiFailed = false;

    void activateZhuangShi(int discard, int hpLoss);
    void discardSelectedCards(const std::vector<int>& indices);
    void useSlash(Player* target, int baseDamage = 1);
    void chooseKuangGuHeal();
    void chooseKuangGuDraw();
    void skipKuangGu();
    bool activateBeiShui(bool shouldActivate);
    void checkKill(Player* target);
    void endTurn();
    void updateStatus();
    void startTurn();
};

#endif