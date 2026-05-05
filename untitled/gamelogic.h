#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <vector>
#include <QString>
#include <iostream>

// 定义卡牌类型
enum CardType { SLASH, DODGE, PEACH, OTHER };

struct Card {
    CardType type;
    QString name;
};

// 基础玩家类
class Player {
public:
    QString name;
    int hp;
    int maxHp;
    std::vector<Card> handCards;

    Player(QString n, int h) : name(n), hp(h), maxHp(h) {}
    void takeDamage(int amount) { hp -= amount; }
    void heal(int amount) { if(hp < maxHp) hp += amount; }
    void drawCard() { handCards.push_back({OTHER, "随机牌"}); }
};

// 势·魏延类 (继承自 Player)
class WeiYan : public Player {
public:
    WeiYan() : Player("势·魏延", 5) {}

    // --- 技能变量 ---
    int zhuangShi_unresponsable = 0; // 壮誓：不可响应的剩余张数
    int zhuangShi_noLimit = 0;       // 壮誓：不计入次数的剩余张数
    bool hasKuangGu = true;          // 狂骨 (开局拥有)
    bool hasBeiShui = false;         // 背水 (成功后获得)
    bool hasKunFen = false;          // 困奋 (失败后获得)

    // --- 技能逻辑函数 ---
    void activateZhuangShi(int discardCount, int hpLoss); // 壮誓启动
    void useSlash(Player* target);                       // 使用【杀】并触发【饮战】
    void triggerKuangGu();                               // 触发狂骨 (选回复或摸牌)
};

#endif