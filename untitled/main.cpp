#include <QCoreApplication>
#include <QDebug>
#include "WeiYan.h"
#include "Player.h"
#include "CardManager.h"
#include "Card.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    WeiYan weiyan;
    Player enemy("敌将", 10);

    qDebug() << "--- 综合卡牌测试开始 ---";

    // 【关键修改】：必须写成 CardCategory::... 和 CardType::...
    Card c1 = {CardCategory::BASIC, CardType::SLASH, "杀", 0};
    Card c2 = {CardCategory::BASIC, CardType::PEACH, "桃", 0};
    Card c3 = {CardCategory::TRICK, CardType::LIGHTNING, "闪电", 0};
    Card c4 = {CardCategory::EQUIPMENT, CardType::WEAPON, "雌雄双股剑", 2};

    // 使用 CardManager 来执行卡牌
    CardManager::executeCard(c1, &weiyan, &enemy);
    CardManager::executeCard(c2, &weiyan, &enemy);
    CardManager::executeCard(c3, &weiyan, &enemy);
    CardManager::executeCard(c4, &weiyan, &enemy);

    qDebug() << "--- 测试结束 ---";

    return 0;
}