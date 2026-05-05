#ifndef CARDMANAGER_H
#define CARDMANAGER_H

#include "Card.h"
#include "WeiYan.h"
#include "Player.h"

class CardManager {
public:
    // 核心方法：处理出一张牌的所有效果
    static void executeCard(Card card, Player* source, Player* target);
};

#endif