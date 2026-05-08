#ifndef WEIYAN_H
#define WEIYAN_H

#include "Player.h"
#include "Card.h"
#include <memory>
#include <utility>

enum ZhongAoState { MISSION_ACTIVE, MISSION_SUCCESS, MISSION_FAILED };

class WeiYan : public Player {
public:
    WeiYan() : Player("势·魏延", 5), zhongAoState(MISSION_ACTIVE), kuangGuLevel(1),
        zhuangShiDiscarded(0), zhuangShiHpLost(0),
        zhuangShiUsedThisTurn(false), cardsUsedThisTurn(0),
        zhuangShiNoLimitRemaining(0), zhuangShiNoLimitUsed(0),
        slashUsedThisTurn(0), slashCountLimit(1),
        chengShiCanTrigger(false), chengShiDiscardedCard(nullptr) {}

    // 使命状态
    ZhongAoState zhongAoState;
    int kuangGuLevel; // 狂骨等级1或2

    // 壮誓状态
    int zhuangShiDiscarded; // 壮誓弃牌数
    int zhuangShiHpLost;    // 壮誓掉血数
    bool zhuangShiUsedThisTurn;
    int cardsUsedThisTurn;
    int zhuangShiNoLimitRemaining; // 壮誓：无距离限制且不可被响应的剩余次数（对应弃牌N张）
    int zhuangShiNoLimitUsed;      // 壮誓：已使用的无距离限制次数
    int zhuangShiNoCountRemaining; // 壮誓：不计入使用次数的剩余次数（对应掉血M张）
    int zhuangShiNoCountUsed;      // 壮誓：已使用的不计入使用次数次数
    int slashUsedThisTurn;
    int slashCountLimit;
    
    // 乘势状态
    bool chengShiCanTrigger;
    std::shared_ptr<Card> chengShiDiscardedCard;

    // 壮誓：执行
    void activateZhuangShi(int discard, int hpLoss);
    // 壮誓：不执行（使命失败）
    void skipZhuangShi();
    // 造成任意伤害后触发狂骨
    void onDamageDealt(Player* target, int damage);
    // 使用杀时触发饮战（加伤），返回是否需要选择弃置的牌
    std::pair<bool, int> useSlash(Player* target, int baseDamage = 1);
    // 狂骨选项（回血/摸牌）
    void chooseKuangGuHeal();
    void chooseKuangGuDraw();
    void skipKuangGu();
    // 背水（二级狂骨）
    void activateBeiShui();
    // 击杀后
    void onKill(Player* target);
    // 结束回合
    void endTurn();
    // 开始回合
    void startTurn();
    // 失去体力
    void loseHp(int amount);
    // 获取到目标的距离
    int getCurrentDistanceTo(Player* target) const;
};

#endif
