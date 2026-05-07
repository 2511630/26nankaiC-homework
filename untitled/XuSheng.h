#ifndef XUSHENG_H
#define XUSHENG_H

#include "Player.h"
#include "Card.h"
#include <memory>

class XuSheng : public Player {
public:
    XuSheng() : Player("界·徐盛", 4), pojunExtraDamage(false) {}

    void useSlash(Player* target, int baseDamage = 1);
    bool pojunExtraDamage;
};

#endif
