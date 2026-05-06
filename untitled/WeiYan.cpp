#include "WeiYan.h"
#include "Card.h"
#include "BasicCards.h"
#include <QDebug>

void WeiYan::activateZhuangShi(int discard, int hpLoss) {
    zhuangShi_discarded = discard;
    zhuangShi_hpLost = hpLoss;
    this->hp -= hpLoss;
    usedZhuangShiThisTurn = true;
    slashCountLimit += discard;
    qDebug() << "【壮誓】发动：弃" << discard << "张，失" << hpLoss << "血，【杀】次数+" << discard;
}

void WeiYan::discardSelectedCards(const std::vector<int>& indices) {
    std::vector<std::shared_ptr<Card>> tempHand;
    for (size_t i = 0; i < handCards.size(); ++i) {
        bool shouldDiscard = false;
        for (int idx : indices) {
            if (static_cast<size_t>(idx) == i) {
                shouldDiscard = true;
                break;
            }
        }
        if (!shouldDiscard) {
            tempHand.push_back(handCards[i]);
        }
    }
    handCards = tempHand;
}

void WeiYan::useSlash(Player* target, int baseDamage) {
    if (slashUsedThisTurn >= slashCountLimit) {
        qDebug() << "【杀】次数已用完！";
        return;
    }

    int attackRange = 1;
    if (equipment.weapon) {
        QString weaponName = equipment.weapon->name;
        if (weaponName == "诸葛连弩") attackRange = 1;
        else if (weaponName == "古锭刀") attackRange = 2;
        else if (weaponName == "麒麟弓") attackRange = 5;
    }

    int distance = this->getCurrentDistance() - (equipment.minusHorse ? 1 : 0) + (target->equipment.plusHorse ? 1 : 0);
    if (distance > attackRange) {
        qDebug() << target->name << "不在攻击范围内（距离：" << distance << "，攻击范围：" << attackRange << "）";
        return;
    }

    cardsUsedThisTurn++;
    slashUsedThisTurn++;
    qDebug() << "魏延对" << target->name << "使用【杀】！（剩余次数：" << slashCountLimit - slashUsedThisTurn << "）";

    int damage = baseDamage;
    bool hpSat = (this->hp <= target->hp);
    bool handSat = (this->handCards.size() <= target->handCards.size());

    if (hpSat) { damage += 1; qDebug() << "【饮战】体力满足 -> 伤害+1"; }
    if (handSat) {
        target->discardRandom(1);
        qDebug() << "【饮战】手牌满足 -> 对方弃置1张";
    }

    target->takeDamage(damage);
    qDebug() << target->name << "受到" << damage << "点伤害, 当前体力:" << target->hp;

    if (this->hasQiLinGong() && target->equipment.plusHorse && target->hp > 0) {
        target->equipment.plusHorse.reset();
        qDebug() << "【麒麟弓】效果：弃掉" << target->name << "的+1马";
    }

    if (target->hp <= 0) {
        checkKill(target);
    } else if (currentState == START && damage > 0) {
        bool afterHpSat = (this->hp <= target->hp);
        bool afterHandSat = (this->handCards.size() <= target->handCards.size());
        if (afterHpSat && afterHandSat) {
            qDebug() << "【乘势】触发！回1血，摸1牌。";
        } else {
            kuangGuCanTrigger = true;
            qDebug() << ">>> [系统提示]：触发【狂骨】，请玩家选择：1.回血 2.摸牌";
        }
    }
}

void WeiYan::chooseKuangGuHeal() {
    if (!kuangGuCanTrigger) return;
    this->heal(1);
    kuangGuCanTrigger = false;
    qDebug() << "【狂骨】玩家选择 -> 回复1点体力。";
}

void WeiYan::chooseKuangGuDraw() {
    if (!kuangGuCanTrigger) return;
    kuangGuCanTrigger = false;
    qDebug() << "【狂骨】玩家选择 -> 摸一张牌。";
}

void WeiYan::skipKuangGu() {
    kuangGuCanTrigger = false;
    qDebug() << "【狂骨】玩家选择不发动";
}

bool WeiYan::activateBeiShui(bool shouldActivate) {
    if (currentState == SUCCESS && shouldActivate) {
        if (!handCards.empty()) {
            handCards.pop_back();
            slashCountLimit++;
            qDebug() << "【背水】发动：弃1牌，【杀】次数+1。";
            return true;
        }
    }
    return false;
}

void WeiYan::checkKill(Player* target) {
    if (target->hp <= 0 && currentState == START) {
        currentState = SUCCESS;
        qDebug() << "【忠傲】使命成功！获得【背水】技能！";
    }
}

void WeiYan::endTurn() {
    if (currentState == SUCCESS) {
        qDebug() << "--- 结算【背水】收益 ---";
        if (slashUsedThisTurn < zhuangShi_discarded) {
            qDebug() << "收益：【杀】次数 < 弃牌数 -> 摸1张牌。";
        }
        if (slashUsedThisTurn < zhuangShi_hpLost) {
            if (this->hp < this->maxHp) this->heal(1);
            qDebug() << "收益：【杀】次数 < 失血量 -> 回复1体力。";
        }
    } else if (!usedZhuangShiThisTurn) {
        this->hp -= 1;
        qDebug() << "【困奋】触发：失1血，摸2牌。";
    }
    cardsUsedThisTurn = 0;
    slashUsedThisTurn = 0;
    slashCountLimit = 1;
    usedZhuangShiThisTurn = false;
    kuangGuCanTrigger = false;
}

void WeiYan::updateStatus() {
    QString stateStr = (currentState == START) ? "狂骨" : (currentState == SUCCESS ? "背水" : "困奋");
    qDebug() << "当前状态：" << stateStr << " | 体力：" << hp << "/" << maxHp;
}

void WeiYan::startTurn() {
    slashUsedThisTurn = 0;
    slashCountLimit = 1;
    kuangGuCanTrigger = false;

    if (currentState == SUCCESS) {
        qDebug() << ">>> [系统提示]：回合开始，是否发动【背水】？1.是 2.否";
    }
}