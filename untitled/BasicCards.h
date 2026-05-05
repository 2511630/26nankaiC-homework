#ifndef BASICCARDS_H
#define BASICCARDS_H

#include "Card.h"

class SlashCard : public Card {
public:
    SlashCard() : Card(CardCategory::BASIC, "杀") {}
    void execute(Player* source, Player* target) override;
};

class DodgeCard : public Card {
public:
    DodgeCard() : Card(CardCategory::BASIC, "闪") {}
    void execute(Player* source, Player* target) override {}
};

class PeachCard : public Card {
public:
    PeachCard() : Card(CardCategory::BASIC, "桃") {}
    void execute(Player* source, Player* target) override;
};

class WineCard : public Card {
public:
    WineCard() : Card(CardCategory::BASIC, "酒") {}
    void execute(Player* source, Player* target) override;
};

#endif