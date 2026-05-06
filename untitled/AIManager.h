#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

class AIManager : public QObject {
    Q_OBJECT
public:
    explicit AIManager(QObject* parent = nullptr);
    ~AIManager();

    void setApiKey(const QString& apiKey);
    void requestDecision(const QString& gameState, std::function<void(const QString&)> callback);

signals:
    void decisionReceived(const QString& decision);
    void errorOccurred(const QString& error);

private slots:
    void onReplyFinished();
    void onTimeout();

private:
    QString m_apiKey;
    QNetworkAccessManager* m_networkManager;
    QNetworkReply* m_currentReply;
    std::function<void(const QString&)> m_callback;
    QTimer* m_timeoutTimer;
};

#endif