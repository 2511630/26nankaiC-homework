#include "TrickCards.h"
#include "Player.h"
#include <QDebug>

void DuelCard::execute(Player* source, Player* target) {
    qDebug() << "【决斗】开始！" << source->name << " VS " << target->name;
    Player* currentAttacker = source;
    Player* currentDefender = target;

    while (true) {
        if (currentDefender->hasSlash()) {
            currentDefender->playSlash();
            qDebug() << currentDefender->name << "打出【杀】";
            std::swap(currentAttacker, currentDefender);
        } else {
            currentDefender->takeDamage(1);
            qDebug() << currentDefender->name << "无法打出【杀】，受到1点伤害";
            break;
        }
    }
}

void NanmanCard::execute(Player* source, Player* target) {
    qDebug() << "【南蛮入侵】" << target->name << "需要出【杀】";
    if (target->hasSlash() && target->playSlash()) {
        qDebug() << target->name << "打出【杀】，抵消伤害";
    } else {
        target->takeDamage(1);
        qDebug() << target->name << "无法打出【杀】，受到1点伤害";
    }
}

void ArrowCard::execute(Player* source, Player* target) {
    qDebug() << "【万箭齐发】" << target->name << "需要出【闪】";
    bool hasDodge = false;
    for (auto it = target->handCards.begin(); it != target->handCards.end(); ++it) {
        if ((*it)->name == "闪") {
            target->handCards.erase(it);
            hasDodge = true;
            break;
        }
    }
    if (hasDodge) {
        qDebug() << target->name << "打出【闪】，抵消伤害";
    } else {
        target->takeDamage(1);
        qDebug() << target->name << "无法打出【闪】，受到1点伤害";
    }
}

void StealCard::execute(Player* source, Player* target) {
    if (source->getCurrentDistance() > 1) {
        qDebug() << "【顺手牵羊】失败：" << source->name << "与" << target->name << "距离超过1";
        return;
    }
    qDebug() << "【顺手牵羊】" << source->name << "抢走" << target->name << "一张牌";
    if (!target->handCards.empty()) {
        source->drawCard(target->handCards.back());
        target->handCards.pop_back();
    }
}

void BridgeCard::execute(Player* source, Player* target) {
    qDebug() << "【过河拆桥】触发，弃置对方一张牌";
    target->discardRandom(1);
}