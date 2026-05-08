#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <vector>
#include <memory>
#include "Card.h"

struct EquipmentArea {
    std::shared_ptr<Card> weapon;
    std::shared_ptr<Card> armor;
    std::shared_ptr<Card> plusHorse;
    std::shared_ptr<Card> minusHorse;
};

class Player {
public:
    QString name;
    int hp;
    int maxHp;
    int distance = 1;
    std::vector<std::shared_ptr<Card>> handCards;
    EquipmentArea equipment;
    std::vector<std::shared_ptr<Card>> kouZhiCards;
    int slashUsedThisTurn = 0;
    int slashCountLimit = 1;

    Player* pojunTarget = nullptr;
    int pojunKouZhiCount = 0;

    Player(QString n, int h) : name(n), hp(h), maxHp(h) {}
    virtual ~Player() {}

    void takeDamage(int amount);
    void heal(int amount);
    void loseHp(int amount);
    void drawCard(std::shared_ptr<Card> card);
    void discardRandom(int count);
    bool isAlive() const { return hp > 0; }
    void equipCard(std::shared_ptr<Card> card);
    int getCurrentDistance() const;
    int getCurrentDistanceTo(Player* target) const;
    bool hasSlash() const;
    bool playSlash();
    bool hasCrossbow() const;
    bool hasGuDingDao() const;
    bool hasQiLinGong() const;
    bool hasRenWangDun() const;
    bool hasBaiYinShiZi() const;
    int getAttackRange() const;
};

#endif