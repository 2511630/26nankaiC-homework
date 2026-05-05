#ifndef CARD_H
#define CARD_H

#include <QString>
#include <memory>

enum class CardCategory { BASIC, TRICK, EQUIPMENT, OTHER };

class Card {
public:
    CardCategory category;
    QString name;

    Card(CardCategory cat, QString n) : category(cat), name(n) {}
    virtual ~Card() {}

    // 核心：所有子类通过这个函数实现自己的效果
    virtual void execute(class Player* source, class Player* target) = 0;
};

#endif