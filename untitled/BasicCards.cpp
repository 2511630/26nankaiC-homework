#include "BasicCards.h"
#include "Player.h"
#include "WeiYan.h"
#include <QDebug>

void SlashCard::execute(Player* source, Player* target) {
    WeiYan* wy = dynamic_cast<WeiYan*>(source);
    if (wy) wy->useSlash(target);
    else target->takeDamage(1);
}

void PeachCard::execute(Player* source, Player* target) {
    target->heal(1);
    qDebug() << target->name << "使用了【桃】，回复1点体力";
}

void WineCard::execute(Player* source, Player* target) {
    qDebug() << source->name << "使用了【酒】，增强下一次攻击";
}