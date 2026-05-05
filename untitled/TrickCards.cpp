#include "TrickCards.h"
#include "Player.h"
#include <QDebug>

void DuelCard::execute(Player* source, Player* target) {
    qDebug() << "【决斗】开始！";
    (std::rand() % 2 == 0) ? target->takeDamage(1) : source->takeDamage(1);
}

void NanmanCard::execute(Player* source, Player* target) {
    qDebug() << "【南蛮入侵】触发，对方受1伤";
    target->takeDamage(1);
}

void ArrowCard::execute(Player* source, Player* target) {
    qDebug() << "【万箭齐发】触发，对方受1伤";
    target->takeDamage(1);
}

void StealCard::execute(Player* source, Player* target) {
    qDebug() << "【顺手牵羊】触发，抢走对方一张牌";
    if (!target->handCards.empty()) {
        source->drawCard(target->handCards.back());
        target->handCards.pop_back();
    }
}

void BridgeCard::execute(Player* source, Player* target) {
    qDebug() << "【过河拆桥】触发，弃置对方一张牌";
    target->discardRandom(1);
}