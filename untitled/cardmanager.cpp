#include "CardManager.h"
#include "WeiYan.h"
#include <QDebug>

void CardManager::executeCard(Card card, Player* source, Player* target) {
    qDebug() << source->name << "使用了【" << card.name << "】";

    // 【关键修改】：全部加上 CardCategory:: 或 CardType::
    if (card.category == CardCategory::BASIC) {
        if (card.type == CardType::SLASH) {
            // 尝试转换为魏延以触发饮战
            WeiYan* wy = dynamic_cast<WeiYan*>(source);
            if (wy) {
                wy->useSlash(target);
            } else {
                target->takeDamage(1);
                qDebug() << "普通攻击造成 1 点伤害";
            }
        }
        else if (card.type == CardType::PEACH) {
            source->heal(1);
            qDebug() << "使用了【桃】，回复 1 点体力";
        }
    }
    else if (card.category == CardCategory::TRICK) {
        if (card.type == CardType::LIGHTNING) {
            qDebug() << "使用【闪电】...";
            if (std::rand() % 2 == 0) target->takeDamage(1);
            else source->takeDamage(1);
        }
    }
    else if (card.category == CardCategory::EQUIPMENT) {
        qDebug() << "装备【" << card.name << "】";
        if (card.type == CardType::WEAPON) {
            source->distance = card.value;
        }
    }
}