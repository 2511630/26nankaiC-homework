#ifndef DECKMANAGER_H
#define DECKMANAGER_H

#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include "Card.h"
#include "BasicCards.h"
#include "TrickCards.h"
#include "EquipCards.h"

class DeckManager {
public:
    std::vector<std::shared_ptr<Card>> deck;
    DeckManager();
    void shuffle();
    std::shared_ptr<Card> draw();
};
#endif