#include <QCoreApplication>
#include <QDebug>
#include "gamelogic.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    WeiYan weiyan;
    Player enemy("敌将", 3);

    qDebug() << "--- 战斗开始 ---";

    // 1. 玩家思考后决定：壮誓弃1张，失1血
    weiyan.activateZhuangShi(1, 1);

    // 2. 魏延出杀，触发饮战
    weiyan.useSlash(&enemy);

    // 3. 此时逻辑层提示：请选择【狂骨】效果
    // 模拟玩家点击了“回复体力”按钮
    weiyan.chooseKuangGuHeal();

    // 4. 再次出杀，击杀敌人
    weiyan.useSlash(&enemy);
    weiyan.useSlash(&enemy);

    // 5. 现在魏延升级到了【背水】状态
    // 模拟玩家在回合中决定发动【背水】
    weiyan.activateBeiShui();

    // 6. 回合结束，结算收益
    weiyan.endTurn();

    qDebug() << "--- 战斗结束 ---";
    return 0;
}