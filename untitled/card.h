#ifndef CARD_H
#define CARD_H

#include <QString>

// 1. 大类也必须有 OTHER
enum class CardCategory {
    BASIC,
    TRICK,
    EQUIPMENT,
    OTHER  // <--- 补齐这里！
};

// 2. 具体类型有 OTHER
enum class CardType {
    SLASH, DODGE, PEACH,
    LIGHTNING, DUEL, IRON_CHAIN,
    WEAPON, ARMOR, HORSE,
    OTHER  // <--- 已经有了
};

struct Card {
    CardCategory category;
    CardType type;
    QString name;
    int value;
};

#endif