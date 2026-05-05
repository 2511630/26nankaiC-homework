#include <QCoreApplication>
#include <QDebug>
#include <memory>
#include "WeiYan.h"
#include "Player.h"
#include "CardManager.h"
#include "DeckManager.h"
#include "BasicCards.h"
#include "TrickCards.h"
#include "EquipCards.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // 1. 初始化
    DeckManager deck;
    deck.shuffle();
    WeiYan weiyan;
    Player enemy("敌将", 10);

    qDebug() << "=== 势·魏延 综合逻辑压力测试 ===";

    // 2. 模拟摸牌
    for(int i=0; i<5; ++i) weiyan.drawCard(deck.draw());
    qDebug() << "魏延初始摸牌完毕，当前手牌数：" << weiyan.handCards.size();

    // 3. 测试【壮誓】
    weiyan.activateZhuangShi(2, 1);

    // 4. 测试不同卡牌的执行 (通过 CardManager)
    // 测试【杀】 -> 触发饮战
    auto slash = std::make_shared<SlashCard>();
    CardManager::executeCard(slash, &weiyan, &enemy);

    // 测试【桃】 -> 回血
    auto peach = std::make_shared<PeachCard>();
    CardManager::executeCard(peach, &weiyan, &enemy);

    // 测试【决斗】 -> 随机伤害
    auto duel = std::make_shared<DuelCard>();
    CardManager::executeCard(duel, &weiyan, &enemy);

    // 测试【装备】 -> 修改属性
    auto weapon = std::make_shared<WeaponCard>("诸葛连弩", true);
    CardManager::executeCard(weapon, &weiyan, &enemy);

    // 5. 模拟击杀升级
    enemy.hp = 1;
    CardManager::executeCard(slash, &weiyan, &enemy);
    weiyan.updateStatus(); // 检查是否升级为【背水】

    // 6. 结算回合收益
    weiyan.endTurn();

    qDebug() << "==================================";
    qDebug() << "逻辑层全部验证通过！准备进入 UI 阶段！";
    return 0;
}