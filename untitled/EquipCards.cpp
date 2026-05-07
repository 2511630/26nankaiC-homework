#include "EquipCards.h"
#include "Player.h"
#include <QDebug>

void HorseCard::execute(Player* source, Player* target) {
    (void)target;
    auto self = std::static_pointer_cast<Card>(std::make_shared<HorseCard>(name, distMod));
    source->equipCard(self);
    qDebug() << source->name << "装备了【" << name << "】，距离修正:" << distMod;
}

void WeaponCard::execute(Player* source, Player* target) {
    (void)target;
    auto self = std::static_pointer_cast<Card>(std::make_shared<WeaponCard>(name, isCrossbow));
    source->equipCard(self);
    qDebug() << source->name << "装备了武器【" << name << "】";
}

void ArmorCard::execute(Player* source, Player* target) {
    (void)target;
    auto self = std::static_pointer_cast<Card>(std::make_shared<ArmorCard>(name));
    source->equipCard(self);
    qDebug() << source->name << "装备了防具【" << name << "】";
}

void GuDingDaoCard::execute(Player* source, Player* target) {
    WeaponCard::execute(source, target);
}