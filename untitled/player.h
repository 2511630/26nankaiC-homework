#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <vector>
#include "Card.h"

class Player {
public:
    QString name;
    int hp;
    int maxHp;
    int distance = 1;
    std::vector<Card> handCards;

    Player(QString n, int h) : name(n), hp(h), maxHp(h) {}
    virtual ~Player() {} // 虚析构函数，保证继承安全

    void takeDamage(int amount);
    void heal(int amount);
    void drawCard(const Card& card);
    void discardRandom(int count);
    bool isAlive() const { return hp > 0; }
};

#endif