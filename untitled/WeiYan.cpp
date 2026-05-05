#include "WeiYan.h"
#include "Card.h"
#include "BasicCards.h" // 必须包含，为了使用具体子类
#include <QDebug>

// 为了方便，我们定义一个简单的通用卡牌类，用于处理“随机牌”
class GenericCard : public Card {
public:
    GenericCard(QString n) : Card(CardCategory::OTHER, n) {}
    void execute(Player* s, Player* t) override { qDebug() << "这是一张通用牌"; }
};

void WeiYan::activateZhuangShi(int discard, int hpLoss) {
    zhuangShi_discarded = discard;
    zhuangShi_hpLost = hpLoss;
    this->hp -= hpLoss;
    this->discardRandom(discard);
    usedZhuangShiThisTurn = true;
    qDebug() << "【壮誓】发动：弃" << discard << "张，失" << hpLoss << "血。";
}

void WeiYan::useSlash(Player* target) {
    cardsUsedThisTurn++;
    qDebug() << "魏延对" << target->name << "使用【杀】！";

    int damage = 1;
    bool hpSat = (this->hp <= target->hp);
    bool handSat = (this->handCards.size() <= target->handCards.size());

    if (hpSat) { damage += 1; qDebug() << "【饮战】体力满足 -> 伤害+1"; }
    if (handSat) {
        target->discardRandom(1);
        qDebug() << "【饮战】手牌满足 -> 对方弃置1张";
    }
    if (hpSat && handSat) {
        this->heal(1);
        // 【修复】：不能用 make_shared<Card>，改用具体类
        this->drawCard(std::make_shared<GenericCard>("随机牌"));
        qDebug() << "【乘势】触发！回1血，摸1牌。";
    }

    target->takeDamage(damage);
    qDebug() << target->name << "受到" << damage << "点伤害, 当前体力:" << target->hp;

    if (target->hp <= 0) {
        checkKill(target);
    } else if (currentState == START) {
        qDebug() << ">>> [系统提示]：触发【狂骨】，请玩家选择：1.回血 2.摸牌";
    }
}

void WeiYan::chooseKuangGuHeal() {
    this->heal(1);
    qDebug() << "【狂骨】玩家选择 -> 回复1点体力。";
}

void WeiYan::chooseKuangGuDraw() {
    this->drawCard(std::make_shared<GenericCard>("随机牌"));
    qDebug() << "【狂骨】玩家选择 -> 摸一张牌。";
}

bool WeiYan::activateBeiShui() {
    if (currentState == SUCCESS) {
        this->drawCard(std::make_shared<GenericCard>("随机牌"));
        this->discardRandom(1);
        qDebug() << "【背水】发动：先摸后弃，【杀】次数+1。";
        return true;
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
        if (cardsUsedThisTurn < zhuangShi_discarded) {
            this->drawCard(std::make_shared<GenericCard>("随机牌"));
            qDebug() << "收益：用牌数 < 弃牌数 -> 摸1张牌。";
        }
        if (cardsUsedThisTurn < zhuangShi_hpLost) {
            if (this->hp < this->maxHp) this->heal(1);
            else this->drawCard(std::make_shared<GenericCard>("随机牌"));
            qDebug() << "收益：用牌数 < 失血量 -> 回复1体力。";
        }
    } else if (!usedZhuangShiThisTurn || currentState == FAILURE) {
        currentState = FAILURE;
        this->hp -= 1;
        this->drawCard(std::make_shared<GenericCard>("随机牌"));
        this->drawCard(std::make_shared<GenericCard>("随机牌"));
        qDebug() << "【困奋】触发：失1血，摸2牌。";
    }
    cardsUsedThisTurn = 0;
    usedZhuangShiThisTurn = false;
}

void WeiYan::updateStatus() {
    QString stateStr = (currentState == START) ? "狂骨" : (currentState == SUCCESS ? "背水" : "困奋");
    qDebug() << "当前状态：" << stateStr << " | 体力：" << hp << "/" << maxHp;
}