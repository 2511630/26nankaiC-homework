#include "EquipCards.h"
#include "Player.h"
#include <QDebug>

void HorseCard::execute(Player* source, Player* target) {
    source->equipCard(std::shared_ptr<Card>(this, [](Card*){}));
    qDebug() << source->name << "装备了马，距离修正:" << distMod;
}

void WeaponCard::execute(Player* source, Player* target) {
    source->equipCard(std::shared_ptr<Card>(this, [](Card*){}));
    qDebug() << source->name << "装备了武器" << name;
}

void ArmorCard::execute(Player* source, Player* target) {
    source->equipCard(std::shared_ptr<Card>(this, [](Card*){}));
    qDebug() << source->name << "装备了防具" << name;
}