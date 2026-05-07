#include "XuSheng.h"
#include <QDebug>

void XuSheng::useSlash(Player* target, int baseDamage) {
    if (slashUsedThisTurn >= slashCountLimit) {
        qDebug() << "【杀】次数已用完！";
        return;
    }

    slashUsedThisTurn++;
    pojunExtraDamage = false;
    qDebug() << "界·徐盛对" << target->name << "使用【杀】！（剩余次数：" << slashCountLimit - slashUsedThisTurn << "）";

    int damage = baseDamage;

    target->takeDamage(damage);
    qDebug() << target->name << "受到" << damage << "点伤害, 当前体力:" << target->hp;

    if (damage > 0 && target->isAlive()) {
        pojunTarget = target;
        pojunKouZhiCount = target->hp;
        qDebug() << "【破军】触发：对方当前体力为" << target->hp << "，可扣置最多" << pojunKouZhiCount << "张牌";
    }
}
