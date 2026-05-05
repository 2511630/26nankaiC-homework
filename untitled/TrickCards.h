#ifndef TRICKCARDS_H
#define TRICKCARDS_H

#include "Card.h"

class DuelCard : public Card {
public:
    DuelCard() : Card(CardCategory::TRICK, "决斗") {}
    void execute(Player* source, Player* target) override;
};

class NanmanCard : public Card {
public:
    NanmanCard() : Card(CardCategory::TRICK, "南蛮入侵") {}
    void execute(Player* source, Player* target) override;
};

class ArrowCard : public Card {
public:
    ArrowCard() : Card(CardCategory::TRICK, "万箭齐发") {}
    void execute(Player* source, Player* target) override;
};

class StealCard : public Card {
public:
    StealCard() : Card(CardCategory::TRICK, "顺手牵羊") {}
    void execute(Player* source, Player* target) override;
};

class BridgeCard : public Card {
public:
    BridgeCard() : Card(CardCategory::TRICK, "过河拆桥") {}
    void execute(Player* source, Player* target) override;
};

#endif