#ifndef FIREBASEAUTH_H
#define FIREBASEAUTH_H

#include <QObject>
#include <QNetworkAccessManager>
#include "firebaseuser.h"


class FirebaseAuth : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey REQUIRED)
    Q_PROPERTY(FirebaseUser* currentUser READ currentUser NOTIFY currentUserChanged)

public:
    explicit FirebaseAuth(QObject *parent = nullptr);

    void setApiKey(const QString &apiKey);
    QString apiKey() const;

    FirebaseUser *currentUser() const;

public slots:
    void signUpWithEmailAndPassword(QString email, QString password, QString name);
    void signInWithEmailAndPassword(QString email, QString password);
    void signInWithOAuthCredential(QString authToken, QString providerId);
    void exchangeRefreshToken(QString refreshToken);

    // Email
    void sendEmailVerification(QString idToken);
    void changeEmail(QString idToken, QString newEmail);
    //void confirmEmailVerification(QString verificationCode); // TODO


    // Password
    void sendPasswordResetEmail(QString email);
    void changePassword(QString idToken, QString newPassword);
    //void verifyPasswordResetCode(QString verificationCode); // TODO
    //void confirmPasswordReset(QString verificationCode, QString newPassword); // TODO


    // User data
    void getUserData(QString idToken);
    void profileUpdate(QString idToken, QString name, QString photoUrl);
    void deleteAccount(QString idToken);

signals:
    void currentUserChanged();
    void signedIn();
    void signedOut();
    void errorOcurred(QString error);

private:
    QString m_apiKey;
    QNetworkAccessManager m_manager;
    FirebaseUser *m_currentUser;
};

#endif // FIREBASEAUTH_H
