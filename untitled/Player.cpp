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

void Player::drawCard(std::shared_ptr<Card> card) {
    if(card) handCards.push_back(card);
}

void Player::discardRandom(int count) {
    for(int i=0; i<count && !handCards.empty(); ++i) handCards.pop_back();
}

void Player::equipCard(std::shared_ptr<Card> card) {
    if (card->category == CardCategory::EQUIPMENT) {
        if (card->name.contains("马")) equipment.horse = card;
        else if (card->name.contains("盾") || card->name.contains("狮子")) equipment.armor = card;
        else equipment.weapon = card;
        qDebug() << name << "装备了【" << card->name << "】";
    }
}

int Player::getCurrentDistance() const {
    int d = distance;
    if (equipment.horse) {
        if (equipment.horse->name.contains("+1")) d += 1;
        else if (equipment.horse->name.contains("-1")) d -= 1;
    }
    return d;
}