#ifndef CARDMANAGER_H
#define CARDMANAGER_H

#include "Card.h"
#include "Player.h"
#include "WeiYan.h"
#include <memory> // 必须包含，为了使用 shared_ptr

class CardManager {
public:
    // 核心方法：执行一张卡牌的效果
    // 参数全部升级为智能指针，确保内存安全且支持多态
    static void executeCard(std::shared_ptr<Card> card, Player* source, Player* target);
};

#endif