#include "BasicCards.h"
#include "Player.h"
#include "WeiYan.h"
#include "XuSheng.h"
#include <QDebug>

void SlashCard::execute(Player* source, Player* target) {
    if (target->hasRenWangDun()) {
        qDebug() << target->name << "装备【仁王盾】，黑色【杀】无效";
        return;
    }

    int damage = 1;

    if (source->hasGuDingDao() && target->handCards.empty()) {
        damage += 1;
        qDebug() << source->name << "装备【古锭刀】，目标无手牌，伤害+1";
    }

    WeiYan* wy = dynamic_cast<WeiYan*>(source);
    if (wy) {
        bool yingZhanJiaShang = false;
        bool chengShiTriggered = false;
        wy->useSlash(target, damage, yingZhanJiaShang, chengShiTriggered);
    } else {
        XuSheng* xs = dynamic_cast<XuSheng*>(source);
        if (xs) {
            xs->useSlash(target);
        } else {
            target->takeDamage(damage);
            if (source->hasQiLinGong() && target->equipment.plusHorse) {
                target->equipment.plusHorse.reset();
                qDebug() << source->name << "装备【麒麟弓】，弃掉" << target->name << "的+1马";
            }
        }
    }
}

void PeachCard::execute(Player* source, Player* target) {
    (void)source;
    target->heal(1);
    qDebug() << target->name << "使用了【桃】，回复1点体力";
}

void WineCard::execute(Player* source, Player* target) {
    (void)target;
    if (source->hp <= 0) {
        // 濒死时使用酒回复体力
        source->heal(1);
        qDebug() << source->name << "在濒死状态使用【酒】，回复1点体力";
    } else {
        qDebug() << source->name << "使用了【酒】，增强下一次攻击";
    }
}
