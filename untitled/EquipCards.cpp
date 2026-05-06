#include "EquipCards.h"
#include "Player.h"
#include <QDebug>

void HorseCard::execute(Player* source, Player* target) {
    auto self = std::shared_ptr<HorseCard>(this);
    source->equipCard(self);
    qDebug() << source->name << "装备了【" << name << "】，距离修正:" << distMod;
}

void WeaponCard::execute(Player* source, Player* target) {
    auto self = std::shared_ptr<WeaponCard>(this);
    source->equipCard(self);
    qDebug() << source->name << "装备了武器【" << name << "】";
}

void ArmorCard::execute(Player* source, Player* target) {
    auto self = std::shared_ptr<ArmorCard>(this);
    source->equipCard(self);
    qDebug() << source->name << "装备了防具【" << name << "】";
}