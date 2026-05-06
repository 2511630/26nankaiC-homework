#ifndef EQUIPCARDS_H
#define EQUIPCARDS_H

#include "Card.h"

class HorseCard : public Card {
public:
    int distMod;
    HorseCard(QString n, int m) : Card(CardCategory::EQUIPMENT, n), distMod(m) {}
    void execute(Player* source, Player* target) override;
};

class WeaponCard : public Card {
public:
    bool isCrossbow;
    WeaponCard(QString n, bool cb) : Card(CardCategory::EQUIPMENT, n), isCrossbow(cb) {}
    void execute(Player* source, Player* target) override;
};

class ArmorCard : public Card {
public:
    ArmorCard(QString n) : Card(CardCategory::EQUIPMENT, n) {}
    void execute(Player* source, Player* target) override;
};
class GuDingDaoCard : public WeaponCard {
public:
    GuDingDaoCard() : WeaponCard("古锭刀", false) {}
    void execute(Player* source, Player* target) override;
};

#endif