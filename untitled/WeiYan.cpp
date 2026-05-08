#include "WeiYan.h"
#include <QDebug>
#include <QtGlobal>
#include "DeckManager.h"

extern DeckManager* gDeck;

void WeiYan::startTurn() {
    zhuangShiDiscarded = 0;
    zhuangShiHpLost = 0;
    zhuangShiUsedThisTurn = false;
    cardsUsedThisTurn = 0;
    slashUsedThisTurn = 0;
    slashCountLimit = 1;
    wineBuffCount = 0;
    chengShiCanTrigger = false;
    chengShiDiscardedCard = nullptr;
}

void WeiYan::activateZhuangShi(int discard, int hpLoss) {
    if (zhongAoState == MISSION_FAILED) return;
    
    zhuangShiUsedThisTurn = true;
    
    if (discard > 0) {
        zhuangShiDiscarded = discard;
        qDebug() << "【壮誓】弃置" << discard << "张牌";
    }
    if (hpLoss > 0) {
        zhuangShiHpLost = hpLoss;
        zhuangShiNoLimitRemaining = hpLoss; // 前等量张牌不计入次数
        zhuangShiNoLimitUsed = 0;
        this->loseHp(hpLoss);
        qDebug() << "【壮誓】失去" << hpLoss << "点体力，" << hpLoss << "张牌不计入次数";
    }
}

void WeiYan::skipZhuangShi() {
    if (zhongAoState != MISSION_ACTIVE) return;
    zhongAoState = MISSION_FAILED;
    qDebug() << "【忠傲】使命失败！失去壮誓，获得困奋";
}

bool WeiYan::useSlash(Player* target, int baseDamage, bool& yingZhanJiaShang, bool& chengShiTriggered) {
    int damage = baseDamage + wineBuffCount; // 酒的buff层数
    yingZhanJiaShang = false;
    bool yingZhanQiPai = false;
    chengShiTriggered = false;
    chengShiDiscardedCard = nullptr;
    chengShiCanTrigger = false;

    if (this->hp <= target->hp) {
        damage += 1;
        yingZhanJiaShang = true;
        qDebug() << "【饮战】势魏延体力≤目标，伤害+1";
    }
    if ((int)this->handCards.size() <= (int)target->handCards.size()) {
        yingZhanQiPai = true;
        qDebug() << "【饮战】势魏延手牌≤目标，结算后弃置目标一张牌";
    }
    if (yingZhanJiaShang && yingZhanQiPai) {
        chengShiTriggered = true;
        qDebug() << "【乘势】两个条件均满足！";
    }

    target->takeDamage(damage);
    
    // 如果是壮誓不计入次数的牌，不增加使用次数
    if (zhuangShiNoLimitRemaining > 0) {
        zhuangShiNoLimitRemaining--;
        zhuangShiNoLimitUsed++;
    } else {
        slashUsedThisTurn++;
    }
    wineBuffCount = 0; // 使用杀后清空酒buff

    if (yingZhanQiPai && !target->handCards.empty()) {
        chengShiCanTrigger = chengShiTriggered;
        return true;
    } else {
        chengShiCanTrigger = false;
        return false;
    }
}

void WeiYan::onDamageDealt(Player* target, int damage) {
    Q_UNUSED(target);
    Q_UNUSED(damage);
    qDebug() << "【狂骨】造成伤害触发判定中";
}

void WeiYan::chooseKuangGuHeal() {
    if (this->hp < this->maxHp) {
        this->heal(1);
        qDebug() << "【狂骨】回复1点体力";
    } else {
        if (gDeck && !gDeck->deck.empty()) {
            this->drawCard(gDeck->draw());
            qDebug() << "【狂骨】体力已满，摸1张牌";
        }
    }
}

void WeiYan::chooseKuangGuDraw() {
    if (gDeck && !gDeck->deck.empty()) {
        this->drawCard(gDeck->draw());
        qDebug() << "【狂骨】摸1张牌";
    }
}

void WeiYan::skipKuangGu() {
    qDebug() << "【狂骨】不发动";
}

void WeiYan::activateBeiShui() {
    if (kuangGuLevel < 2) return;
    if (handCards.empty()) return;
    
    auto card = handCards.back();
    handCards.pop_back();
    qDebug() << "【背水】弃置一张牌：" << card->name;
    slashCountLimit += 1;
    qDebug() << "【背水】杀次数+1";
}

void WeiYan::onKill(Player* target) {
    Q_UNUSED(target);
    if (zhongAoState != MISSION_ACTIVE) return;
    
    qDebug() << "【忠傲】杀死目标，狂骨升级！";
    
    kuangGuLevel = 2;
    zhongAoState = MISSION_SUCCESS;
    
    if (cardsUsedThisTurn < zhuangShiDiscarded) {
        if (gDeck && !gDeck->deck.empty()) {
            this->drawCard(gDeck->draw());
            qDebug() << "【忠傲】使用牌数 < 弃牌数，摸1张";
        }
    }
    if (cardsUsedThisTurn < zhuangShiHpLost) {
        if (this->hp < this->maxHp) {
            this->heal(1);
            qDebug() << "【忠傲】使用牌数 < 掉血数，回复1点";
        } else if (gDeck && !gDeck->deck.empty()) {
            this->drawCard(gDeck->draw());
            qDebug() << "【忠傲】体力已满，摸1张";
        }
    }
}

void WeiYan::endTurn() {
    if (zhongAoState == MISSION_FAILED) {
        qDebug() << "【困奋】结束阶段：失去1点体力，摸2张牌";
    }
}

void WeiYan::loseHp(int amount) {
    this->hp -= amount;
    qDebug() << "失去" << amount << "点体力，当前：" << this->hp;
}
