#ifndef CARD_H
#define CARD_H

#include <QString>

enum CardCategory { BASIC, TRICK, EQUIPMENT };
enum CardType {
    SLASH, DODGE, PEACH,
    LIGHTNING, DUEL, IRON_CHAIN,
    WEAPON, ARMOR, HORSE
};

struct Card {
    CardCategory category;
    CardType type;
    QString name;
    int value;
};

#endif