#include "XuSheng.h"
#include <QDebug>

void XuSheng::useSlash(Player* target) {
    qDebug() << "界·徐盛对" << target->name << "使用【杀】！";

    int damage = 1;

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
        int x = target->hp;
        if (x > 0 && !target->handCards.empty()) {
            int takeCount = std::min(x, (int)target->handCards.size());
            for (int i = 0; i < takeCount; ++i) {
                target->kouZhiCards.push_back(target->handCards.back());
                target->handCards.pop_back();
            }
            qDebug() << "【破军】将" << takeCount << "张牌扣置于" << target->name << "的武将牌旁";
            qDebug() << target->name << "扣置区：" << target->kouZhiCards.size() << "张，手牌：" << target->handCards.size() << "张";
        }
    }
}