#ifndef FIREBASEAPP_H
#define FIREBASEAPP_H

#include <QObject>
#include <QJsonDocument>
#include "firebaseauth.h"
#include "firebasedatabase.h"
#include "firebaseuser.h"

class FirebaseApp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(QString authDomain READ authDomain WRITE setAuthDomain NOTIFY authDomainChanged)
    Q_PROPERTY(QString databaseUrl READ databaseUrl WRITE setDatabaseUrl NOTIFY databaseUrlChanged)
    Q_PROPERTY(QString storageBucket READ storageBucket WRITE setStorageBucket NOTIFY storageBucketChanged)

public:
    explicit FirebaseApp(QObject *parent = nullptr);

    QString source() const;
    QString apiKey() const;
    QString authDomain() const;
    QString databaseUrl() const;
    QString storageBucket() const;

    void setSource(const QString &source);
    void setApiKey(const QString &apiKey);
    void setAuthDomain(const QString &authDomain);
    void setDatabaseUrl(const QString &databaseUrl);
    void setStorageBucket(const QString &storageBucket);

signals:
    void apiKeyChanged();
    void authDomainChanged();
    void databaseUrlChanged();
    void storageBucketChanged();
    void sourceChanged();

private:
    void parseFirebaseJson();

    QString m_source;
    QString m_apiKey, m_authDomain, m_databaseUrl, m_storageBucket;
};

#endif // FIREBASEAPP_H
