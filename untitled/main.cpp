#include <QCoreApplication>
#include <QDebug>      // <--- 必须加上这个头文件！
#include "gamelogic.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    WeiYan weiyan;
    Player enemy("敌人", 6);

    qDebug() << "--- 战斗开始 ---"; // 替换这里

    weiyan.activateZhuangShi(2, 1);
    weiyan.useSlash(&enemy);
    weiyan.triggerKuangGu();

    qDebug() << "--- 战斗结束 ---"; // 替换这里
    return 0;
}