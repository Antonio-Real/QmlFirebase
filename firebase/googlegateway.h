#ifndef GOOGLEGATEWAY_H
#define GOOGLEGATEWAY_H

#include <QObject>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QNetworkReply>

class GoogleGateway : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString authToken READ authToken WRITE setAuthToken NOTIFY authTokenChanged)
    Q_PROPERTY(QString clientSecretPath WRITE setClientSecretPath NOTIFY clientSecretPathChanged)

public:
    explicit GoogleGateway(QObject *parent = nullptr);

    QString authToken() const;
    void setAuthToken(const QString &authToken);
    void setClientSecretPath(const QString &value);

public slots:
    void fetchGoogleToken();

signals:
    void authTokenChanged();
    void clientSecretPathChanged();

private:
    void parseClientSecret();

    QOAuth2AuthorizationCodeFlow *m_google = nullptr;
    QString m_authToken;
    QString clientSecretPath;
    QOAuthHttpServerReplyHandler *m_replyhandler = nullptr;
};

#endif // GOOGLEGATEWAY_H
