#include "DeckManager.h"

DeckManager::DeckManager() {
    for(int i=0; i<20; ++i) deck.push_back(std::make_shared<SlashCard>());
    for(int i=0; i<20; ++i) deck.push_back(std::make_shared<DodgeCard>());
    for(int i=0; i<10; ++i) deck.push_back(std::make_shared<PeachCard>());
    for(int i=0; i<10; ++i) deck.push_back(std::make_shared<WineCard>());
    for(int i=0; i<4; ++i) deck.push_back(std::make_shared<DuelCard>());
    for(int i=0; i<4; ++i) deck.push_back(std::make_shared<NanmanCard>());
    deck.push_back(std::make_shared<ArrowCard>());
    for(int i=0; i<4; ++i) deck.push_back(std::make_shared<StealCard>());
    for(int i=0; i<6; ++i) deck.push_back(std::make_shared<BridgeCard>());
    deck.push_back(std::make_shared<HorseCard>("加一马", 1));
    deck.push_back(std::make_shared<HorseCard>("减一马", -1));
    deck.push_back(std::make_shared<WeaponCard>("诸葛连弩", true));
    for(int i=0; i<2; ++i) deck.push_back(std::make_shared<WeaponCard>("麒麟弓", false));
    deck.push_back(std::make_shared<ArmorCard>("仁王盾"));
    deck.push_back(std::make_shared<ArmorCard>("白银狮子"));
}

void DeckManager::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
}

std::shared_ptr<Card> DeckManager::draw() {
    if (deck.empty()) return nullptr;
    auto c = deck.back();
    deck.pop_back();
    return c;
}