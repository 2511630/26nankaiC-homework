#include "gamelogic.h"

void WeiYan::activateZhuangShi(int discard, int hpLoss) {
    zhuangShi_discarded = discard;
    zhuangShi_hpLost = hpLoss;
    this->hp -= hpLoss;
    this->discardRandom(discard);
    usedZhuangShiThisTurn = true;
    qDebug() << "【壮誓】已发动 (玩家选择)：弃" << discard << "张，失" << hpLoss << "血。";
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
        this->drawCard();
        qDebug() << "【乘势】触发！回1血，摸1牌。";
    }

    target->takeDamage(damage);
    qDebug() << target->name << "受到" << damage << "点伤害, 当前体力:" << target->hp;

    if (target->hp <= 0) {
        checkKill(target);
    } else if (currentState == START) {
        // 这里不再自动回血，而是发送一个信号给界面
        qDebug() << ">>> [系统提示]：触发【狂骨】，请玩家选择：1.回血 2.摸牌";
    }
}

// 玩家在界面点击“回复体力”按钮时调用
void WeiYan::chooseKuangGuHeal() {
    this->heal(1);
    qDebug() << "【狂骨】玩家选择 -> 回复1点体力。";
}

// 玩家在界面点击“摸一张牌”按钮时调用
void WeiYan::chooseKuangGuDraw() {
    this->drawCard();
    qDebug() << "【狂骨】玩家选择 -> 摸一张牌。";
}

// 玩家在界面点击“发动背水”按钮时调用
bool WeiYan::activateBeiShui() {
    if (currentState == SUCCESS) {
        // 【正确逻辑】：先摸一张牌，然后再弃一张牌
        this->drawCard();
        qDebug() << "【背水】发动：先摸一张牌...";

        // 此时手里绝对有牌了（至少有刚才摸的那张），直接执行弃牌
        this->discardRandom(1);
        qDebug() << "【背水】随后弃置一张牌，本回合【杀】次数+1。";

        return true;
    }
    qDebug() << "【背水】发动失败：当前状态不是【SUCCESS】。";
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
            this->drawCard();
            qDebug() << "收益：用牌数 < 弃牌数 -> 摸1张牌。";
        }
        if (cardsUsedThisTurn < zhuangShi_hpLost) {
            if (this->hp < this->maxHp) {
                this->heal(1);
                qDebug() << "收益：用牌数 < 失血量 -> 回复1体力。";
            } else {
                this->drawCard();
                qDebug() << "收益：体力满 $\rightarrow$ 摸1张牌。";
            }
        }
    } else if (!usedZhuangShiThisTurn || currentState == FAILURE) {
        currentState = FAILURE;
        this->hp -= 1;
        this->drawCard();
        this->drawCard();
        qDebug() << "【困奋】触发：失1血，摸2牌。";
    }
    cardsUsedThisTurn = 0;
    usedZhuangShiThisTurn = false;
}