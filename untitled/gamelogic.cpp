#include "gamelogic.h"
#include <QDebug>  // <--- 必须加上这个头文件！
#include <cstdlib>
#include <ctime>

// 【壮誓】启动
void WeiYan::activateZhuangShi(int discardCount, int hpLoss) {
    zhuangShi_unresponsable = discardCount;
    zhuangShi_noLimit = hpLoss;
    this->hp -= hpLoss;
    qDebug() << "发动【壮誓】：弃" << discardCount << "张牌，失" << hpLoss << "点体力。"; // 替换这里
}

// 【饮战】逻辑
void WeiYan::useSlash(Player* target) {
    qDebug() << "魏延对" << target->name << "使用【杀】！"; // 替换这里

    int damage = 1;

    if (this->hp <= target->hp) {
        damage += 1;
        qDebug() << "【饮战】体力满足 -> 伤害+1！"; // 替换这里
    }

    if (this->handCards.size() <= target->handCards.size()) {
        qDebug() << "【饮战】手牌满足 -> 对方弃置一张牌！"; // 替换这里
        if (!target->handCards.empty()) target->handCards.pop_back();
    }

    if (this->hp <= target->hp && this->handCards.size() <= target->handCards.size()) {
        this->heal(1);
        this->drawCard();
        qDebug() << "【乘势】触发！回复1点体力并获得一张牌。"; // 替换这里
    }

    target->takeDamage(damage);
    qDebug() << target->name << "受到" << damage << "点伤害，当前体力:" << target->hp; // 替换这里
}

// 【狂骨】选择
void WeiYan::triggerKuangGu() {
    // 注意：qDebug() 只能输出，不能像 cin 那样接收输入
    // 因为我们要进入 Qt 界面，这里的输入以后会变成点击按钮
    qDebug() << "触发【狂骨】：(模拟选择) 回复1体力";
    this->heal(1);
}