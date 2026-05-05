#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <vector>
#include <memory>
#include "Card.h"

struct EquipmentArea {
    std::shared_ptr<Card> weapon;
    std::shared_ptr<Card> armor;
    std::shared_ptr<Card> horse;
};

class Player {
public:
    QString name;
    int hp;
    int maxHp;
    int distance = 1;
    std::vector<std::shared_ptr<Card>> handCards;
    EquipmentArea equipment;

    Player(QString n, int h) : name(n), hp(h), maxHp(h) {}
    virtual ~Player() {}

    void takeDamage(int amount);
    void heal(int amount);
    void drawCard(std::shared_ptr<Card> card);
    void discardRandom(int count);
    bool isAlive() const { return hp > 0; }
    void equipCard(std::shared_ptr<Card> card);
    int getCurrentDistance() const;
};

#endif