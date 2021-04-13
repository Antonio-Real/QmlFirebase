#ifndef FIREBASEUSER_H
#define FIREBASEUSER_H

#include <QObject>
#include <QNetworkAccessManager>

class FirebaseUser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(bool emailVerified READ emailVerified WRITE setEmailVerified NOTIFY emailVerifiedChanged)
    Q_PROPERTY(QString idToken READ idToken WRITE setIdToken NOTIFY idTokenChanged)
    Q_PROPERTY(QString refreshToken READ refreshToken WRITE setRefreshToken NOTIFY refreshTokenChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString photoUrl READ photoUrl WRITE setPhotoUrl NOTIFY photoUrlChanged)

public:
    explicit FirebaseUser(QObject *parent = nullptr);

    QString email() const;
    void setEmail(const QString &email);

    QString idToken() const;
    void setIdToken(const QString &idToken);

    QString refreshToken() const;
    void setRefreshToken(const QString &refreshToken);

    QString userId() const;
    void setUserId(const QString &userId);

    QString name() const;
    void setName(const QString &name);

    bool emailVerified() const;
    void setEmailVerified(bool emailVerified);

    QString photoUrl() const;
    void setPhotoUrl(const QString &photoUrl);

signals:
    void nameChanged();
    void emailChanged();
    void emailVerifiedChanged();
    void idTokenChanged();
    void refreshTokenChanged();
    void userIdChanged();
    void photoUrlChanged();

private:
    QString m_name, m_email, m_idToken, m_refreshToken, m_userId, m_photoUrl;
    bool m_emailVerified = false;
    QNetworkAccessManager m_manager;
};

#endif // FIREBASEUSER_H
