#include <QCoreApplication>
#include <QDebug>
#include <memory>
#include "WeiYan.h"
#include "XuSheng.h"
#include "Player.h"
#include "CardManager.h"
#include "DeckManager.h"
#include "BasicCards.h"
#include "TrickCards.h"
#include "EquipCards.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    qDebug() << "=== 势·魏延 VS 界·徐盛 - 单挑对决 ===";
    qDebug() << "";

    // 1. 初始化角色
    DeckManager deck;
    deck.shuffle();

    WeiYan weiyan;
    XuSheng xusheng;

    // 2. 初始摸牌 (每人4张)
    qDebug() << "--- 初始摸牌阶段 ---";
    for(int i=0; i<4; ++i) {
        weiyan.drawCard(deck.draw());
        xusheng.drawCard(deck.draw());
    }
    qDebug() << "势·魏延 初始手牌：" << weiyan.handCards.size() << "张";
    qDebug() << "界·徐盛 初始手牌：" << xusheng.handCards.size() << "张";
    qDebug() << "";

    // 3. 魏延回合
    qDebug() << "--- 势·魏延 回合 ---";
    weiyan.startTurn();

    qDebug() << "魏延选择发动【壮誓】：弃2张牌，失1血";
    weiyan.activateZhuangShi(2, 1);
    qDebug() << "魏延当前体力：" << weiyan.hp << "/" << weiyan.maxHp;
    qDebug() << "魏延当前手牌：" << weiyan.handCards.size() << "张";

    auto slash1 = std::make_shared<SlashCard>();
    qDebug() << "";
    qDebug() << "魏延对徐盛使用【杀】！";
    CardManager::executeCard(slash1, &weiyan, &xusheng);

    qDebug() << "";
    qDebug() << "徐盛当前体力：" << xusheng.hp << "/" << xusheng.maxHp;
    qDebug() << "徐盛当前手牌：" << xusheng.handCards.size() << "张";

    weiyan.endTurn();
    qDebug() << "";

    // 4. 徐盛回合
    qDebug() << "--- 界·徐盛 回合 ---";
    xusheng.drawCard(deck.draw());
    xusheng.drawCard(deck.draw());
    qDebug() << "徐盛摸牌后手牌：" << xusheng.handCards.size() << "张";

    auto slash2 = std::make_shared<SlashCard>();
    qDebug() << "";
    qDebug() << "徐盛对魏延使用【杀】！";
    CardManager::executeCard(slash2, &xusheng, &weiyan);

    qDebug() << "";
    qDebug() << "魏延当前体力：" << weiyan.hp << "/" << weiyan.maxHp;
    qDebug() << "魏延当前手牌：" << weiyan.handCards.size() << "张";
    qDebug() << "";

    // 5. 结算
    qDebug() << "=== 回合结束 ===";
    weiyan.updateStatus();
    qDebug() << "界·徐盛 | 体力：" << xusheng.hp << "/" << xusheng.maxHp;

    qDebug() << "";
    qDebug() << "==================================";
    qDebug() << "势·魏延 VS 界·徐盛 单挑测试完成！";
    return 0;
}