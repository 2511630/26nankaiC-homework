#include "Player.h"
#include <QDebug>

void Player::takeDamage(int amount) {
    if (hasBaiYinShiZi() && amount > 1) {
        amount = 1;
        qDebug() << name << "装备【白银狮子】，伤害减为1";
    }
    hp -= amount;
    if (hp < 0) hp = 0;
    qDebug() << name << "受到" << amount << "点伤害, 当前体力:" << hp;
}

void Player::heal(int amount) {
    hp += amount;
    if (hp > maxHp) hp = maxHp;
}

void Player::loseHp(int amount) {
    hp -= amount;
    if (hp < 0) hp = 0;
}

void Player::drawCard(std::shared_ptr<Card> card) {
    if(card) handCards.push_back(card);
}

void Player::discardRandom(int count) {
    for(int i=0; i<count && !handCards.empty(); ++i) handCards.pop_back();
}

void Player::equipCard(std::shared_ptr<Card> card) {
    if (card->category == CardCategory::EQUIPMENT) {
        if (card->name.contains("+1马")) equipment.plusHorse = card;
        else if (card->name.contains("-1马")) equipment.minusHorse = card;
        else if (card->name.contains("盾") || card->name.contains("狮子")) equipment.armor = card;
        else equipment.weapon = card;
        qDebug() << name << "装备了【" << card->name << "】";
    }
}

int Player::getCurrentDistance() const {
    int d = distance;
    if (equipment.plusHorse) d += 1;
    if (equipment.minusHorse) d -= 1;
    return d;
}

int Player::getCurrentDistanceTo(Player* target) const {
    int distance = 1;
    if (this->equipment.minusHorse) distance--;
    if (target->equipment.plusHorse) distance++;
    if (distance < 1) distance = 1;
    return distance;
}

bool Player::hasCrossbow() const {
    return equipment.weapon && equipment.weapon->name == "诸葛连弩";
}

bool Player::hasGuDingDao() const {
    return equipment.weapon && equipment.weapon->name == "古锭刀";
}

bool Player::hasQiLinGong() const {
    return equipment.weapon && equipment.weapon->name == "麒麟弓";
}

bool Player::hasRenWangDun() const {
    return equipment.armor && equipment.armor->name == "仁王盾";
}

bool Player::hasBaiYinShiZi() const {
    return equipment.armor && equipment.armor->name == "白银狮子";
}

int Player::getAttackRange() const {
    int range = 1; // 默认攻击范围1
    if (equipment.weapon) {
        if (equipment.weapon->name == "麒麟弓") {
            range = 5;
        } else {
            range = 2; // 其他武器（诸葛连弩、古锭刀等）都是攻击范围2
        }
    }
    if (equipment.minusHorse) {
        range += 1;
    }
    return range;
}

bool Player::hasSlash() const {
    for (const auto& card : handCards) {
        if (card->name == "杀" || card->name == "闪") {
            return true;
        }
    }
    return false;
}

bool Player::playSlash() {
    for (auto it = handCards.begin(); it != handCards.end(); ++it) {
        if ((*it)->name == "杀") {
            handCards.erase(it);
            return true;
        }
    }
    return false;
}