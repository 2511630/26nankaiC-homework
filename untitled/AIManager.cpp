#include "AIManager.h"
#include <QDebug>
#include <QNetworkRequest>

AIManager::AIManager(QObject* parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
    , m_timeoutTimer(new QTimer(this))
{
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(30000);
    connect(m_timeoutTimer, &QTimer::timeout, this, &AIManager::onTimeout);
}

AIManager::~AIManager() {
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
}

void AIManager::setApiKey(const QString& apiKey) {
    m_apiKey = apiKey;
}

void AIManager::requestDecision(const QString& gameState, std::function<void(const QString&)> callback) {
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API key not set");
        callback("");
        return;
    }

    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }

    m_callback = callback;

    QUrl url("https://api.deepseek.com/chat/completions");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());

    QString systemPrompt = R"(
你是一个三国杀游戏AI。当前你是界·徐盛，需要做出最优决策。

游戏规则：
- 你有4点体力
- 你的技能【破军】：当你使用【杀】指定目标后，可以将其至多X张牌扣置（X为目标体力值），若如此做，回合结束后该角色获得这些牌。当你使用【杀】对手牌数与装备数均不大于你的角色造成伤害时，此伤害+1。

当前你只能执行以下操作之一（必须严格返回JSON格式）：
1. 出杀：{"action": "slash"}
2. 出顺手牵羊：{"action": "shunshou"}
3. 出过河拆桥：{"action": "guohe"}
4. 出决斗：{"action": "jiuedou"}
5. 出南蛮入侵：{"action": "nanman"}
6. 出万箭齐发：{"action": "wanjian"}
7. 结束回合：{"action": "end_turn"}
8. 装备武器：{"action": "equip_weapon"}
9. 装备防具：{"action": "equip_armor"}
10. 装备+1马：{"action": "equip_plus_horse"}
11. 装备-1马：{"action": "equip_minus_horse"}
12. 使用桃：{"action": "use_peach"}
13. 使用酒：{"action": "use_wine"}

只返回JSON，不要任何解释。
)";

    QJsonObject json;
    json["model"] = "deepseek-chat";
    json["stream"] = false;

    QJsonArray messages;
    QJsonObject systemMsg;
    systemMsg["role"] = "system";
    systemMsg["content"] = systemPrompt;
    messages.append(systemMsg);

    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = gameState;
    messages.append(userMsg);

    json["messages"] = messages;

    QByteArray data = QJsonDocument(json).toJson();

    m_currentReply = m_networkManager->post(request, data);
    connect(m_currentReply, &QNetworkReply::finished, this, &AIManager::onReplyFinished);
    m_timeoutTimer->start();
}

void AIManager::onReplyFinished() {
    m_timeoutTimer->stop();

    if (!m_currentReply) return;

    if (m_currentReply->error() != QNetworkReply::NoError) {
        QString error = m_currentReply->errorString();
        qDebug() << "AI request error:" << error;
        emit errorOccurred(error);
        if (m_callback) {
            m_callback("");
        }
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    QByteArray response = m_currentReply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);

    if (!doc.isObject()) {
        qDebug() << "Invalid response format";
        emit errorOccurred("Invalid response format");
        if (m_callback) {
            m_callback("");
        }
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    QJsonObject obj = doc.object();
    if (obj.contains("choices") && obj["choices"].isArray()) {
        QJsonArray choices = obj["choices"].toArray();
        if (!choices.isEmpty() && choices[0].isObject()) {
            QJsonObject choice = choices[0].toObject();
            if (choice.contains("message") && choice["message"].isObject()) {
                QJsonObject message = choice["message"].toObject();
                QString content = message["content"].toString();
                qDebug() << "AI response:" << content;
                emit decisionReceived(content);
                if (m_callback) {
                    m_callback(content);
                }
            }
        }
    }

    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void AIManager::onTimeout() {
    qDebug() << "AI request timeout";
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
    emit errorOccurred("Request timeout");
    if (m_callback) {
        m_callback("");
    }
}