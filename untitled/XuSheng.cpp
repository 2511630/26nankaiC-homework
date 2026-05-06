#include "XuSheng.h"
#include <QDebug>

void XuSheng::useSlash(Player* target, int baseDamage) {
    if (slashUsedThisTurn >= slashCountLimit) {
        qDebug() << "【杀】次数已用完！";
        return;
    }

    slashUsedThisTurn++;
    qDebug() << "界·徐盛对" << target->name << "使用【杀】！（剩余次数：" << slashCountLimit - slashUsedThisTurn << "）";

    int damage = baseDamage;

    int myCards = handCards.size();
    int myEquips = 0;
    if (equipment.weapon) myEquips++;
    if (equipment.armor) myEquips++;
    if (equipment.plusHorse) myEquips++;
    if (equipment.minusHorse) myEquips++;

    int targetCards = target->handCards.size();
    int targetEquips = 0;
    if (target->equipment.weapon) targetEquips++;
    if (target->equipment.armor) targetEquips++;
    if (target->equipment.plusHorse) targetEquips++;
    if (target->equipment.minusHorse) targetEquips++;

    if (targetCards <= myCards && targetEquips <= myEquips) {
        damage += 1;
        qDebug() << "【破军】伤害+1：对手牌数(" << targetCards << ")与装备数(" << targetEquips << ")均不大于自己";
    }

    target->takeDamage(damage);
    qDebug() << target->name << "受到" << damage << "点伤害, 当前体力:" << target->hp;

    if (damage > 0 && target->isAlive()) {
        pojunTarget = target;
        pojunKouZhiCount = target->hp;
        qDebug() << "【破军】触发：对方当前体力为" << target->hp << "，可扣置最多" << pojunKouZhiCount << "张牌";
    }
}