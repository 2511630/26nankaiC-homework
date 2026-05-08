#ifndef WEIYAN_H
#define WEIYAN_H

#include "Player.h"
#include "Card.h"
#include <memory>

enum ZhongAoState { MISSION_ACTIVE, MISSION_SUCCESS, MISSION_FAILED };

class WeiYan : public Player {
public:
    WeiYan() : Player("势·魏延", 5), zhongAoState(MISSION_ACTIVE), kuangGuLevel(1),
        zhuangShiDiscarded(0), zhuangShiHpLost(0),
        zhuangShiUsedThisTurn(false), cardsUsedThisTurn(0),
        zhuangShiNoLimitRemaining(0), zhuangShiNoLimitUsed(0),
        slashUsedThisTurn(0), slashCountLimit(1),
        wineBuffCount(0),
        chengShiCanTrigger(false), chengShiDiscardedCard(nullptr) {}

    // 使命状态
    ZhongAoState zhongAoState;
    int kuangGuLevel; // 狂骨等级1或2

    // 壮誓状态
    int zhuangShiDiscarded; // 壮誓弃牌数
    int zhuangShiHpLost;    // 壮誓掉血数
    bool zhuangShiUsedThisTurn;
    int cardsUsedThisTurn;
    int zhuangShiNoLimitRemaining; // 壮誓：无距离限制且不可被响应的剩余次数
    int zhuangShiNoLimitUsed;      // 壮誓：已使用的无距离限制次数
    int slashUsedThisTurn;
    int slashCountLimit;
    
    int wineBuffCount; // 酒的buff层数
    
    // 乘势状态
    bool chengShiCanTrigger;
    std::shared_ptr<Card> chengShiDiscardedCard;

    // 壮誓：执行
    void activateZhuangShi(int discard, int hpLoss);
    // 壮誓：不执行（使命失败）
    void skipZhuangShi();
    // 造成任意伤害后触发狂骨
    void onDamageDealt(Player* target, int damage);
    // 使用杀时触发饮战（加伤），返回是否需要选择弃置的牌，通过引用参数返回是否加伤和是否触发乘势
    bool useSlash(Player* target, int baseDamage, bool& yingZhanJiaShang, bool& chengShiTriggered);
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
};

#endif
