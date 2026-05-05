#include "Player.h"
#include <QDebug>

void Player::takeDamage(int amount) {
    hp -= amount;
    if (hp < 0) hp = 0;
    qDebug() << name << "受到" << amount << "点伤害, 当前体力:" << hp;
}

void Player::heal(int amount) {
    hp += amount;
    if (hp > maxHp) hp = maxHp;
}

void Player::drawCard(const Card& card) {
    handCards.push_back(card);
}

void Player::discardRandom(int count) {
    for(int i=0; i<count && !handCards.empty(); ++i) {
        handCards.pop_back();
    }
}