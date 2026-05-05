#include "CardManager.h"
#include <QDebug>

void CardManager::executeCard(std::shared_ptr<Card> card, Player* source, Player* target) {
    if (!card) return;

    qDebug() << "------------------------------------------------";
    qDebug() << source->name << "使用了【" << card->name << "】";

    // 【多态的核心】：直接执行！
    // 如果 card 是 SlashCard，它就执行 SlashCard::execute
    // 如果 card 是 PeachCard，它就执行 PeachCard::execute
    card->execute(source, target);
}