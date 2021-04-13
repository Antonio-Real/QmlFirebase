#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrlQuery>
#include "firebaseauth.h"
#include "firebaseuser.h"
#include "utils/AuthUtils.h"


/*!
    \qmltype FirebaseAuth
    \inqmlmodule Firebase
    \ingroup Firebase
    \brief Type that offers methods for managing authentication and users of the Firebase.

    FirebaseAuth provides methods for authenticating on the Firebase and also managing the accounts such as changing
    password or email, sending password reset requests and updating profile data. Authentication is necessary if the rules
    on the Firebase project are set in such a way that authentication is required for writing to the database.
    After logging in, the property \l currentUser holds a reference to the authenticated user of type \l FirebaseUser.
*/
FirebaseAuth::FirebaseAuth(QObject *parent) : QObject(parent), m_currentUser(new FirebaseUser)
{
}

/*!
    \qmlsignal FirebaseAuth::errorOcurred(string error)

    Emitted when an error ocurred with the corresponding error in human readable format.
 */

/*!
    \qmlsignal FirebaseAuth::signedIn()

    Emitted when a user successfully authenticates.

    \sa currentUser
 */

void FirebaseAuth::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

/*!
    \qmlproperty string FirebaseAuth::apiKey

    This property holds the API Key of the Firebase project, passed from \l FirebaseApp.
 */
QString FirebaseAuth::apiKey() const
{
    return m_apiKey;
}

/*!
    \qmlproperty FirebaseUser FirebaseAuth::currentUser

    Holds a reference to the user that is currently logged in. Typically used to access the idToken of the user for actions that require authentication, for example:

    \code
    FirebaseApp { id: fbApp }
    FirebaseAuth {
        id: fbAuth
        apiKey: fbApp.apiKey

        onSignedIn: {
            fbAuth.getUserData(currentUser.idToken)

            // Fetches audio data from database
            fbDb.getValue("/Audio/.json", fbAuth.currentUser.idToken, codes.audioDataCode)

            // Adds the listeners to these locations of the database
            fbDb.listenEvents("/Audio/.json", fbAuth.currentUser.idToken, codes.audioDataCode)
            fbDb.listenEvents("/Users/.json", fbAuth.currentUser.idToken, codes.userDataCode)
        }
    }

    FirebaseDatabase {
        id: fbDb
        ...
    }
    \endcode

    \sa FirebaseUser
 */
FirebaseUser *FirebaseAuth::currentUser() const
{
    return m_currentUser;
}

/*!
    \qmlmethod void FirebaseAuth::signInWithEmailAndPassword(string email, string password)

    Calls a login request with \a email and \a password. If successful emits signals \l signedIn() and updates \l currentUser to the user, otherwise
    if an error occurs (e.g incorrect email/password, no network access) the signal \l errorOcurred is emited with the error string.
 */
void FirebaseAuth::signInWithEmailAndPassword(QString email, QString password)
{
    QUrl endpoint = AuthUtils::endpoint_signIn + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"email\":\"%1\",\"password\":\"%2\",\"returnSecureToken\":true}").arg(email).arg(password);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                m_currentUser->setEmail(doc["email"].toString());
                m_currentUser->setIdToken(doc["idToken"].toString());
                m_currentUser->setRefreshToken(doc["refreshToken"].toString());
                m_currentUser->setUserId(doc["localId"].toString());

                emit currentUserChanged();
                emit signedIn();
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

/*!
    \qmlmethod void FirebaseAuth::signUpWithEmailAndPassword(string email, string password, string name)

    Registers a new account in the Firebase of user with \a name, \a email and \a password. If successful emits signals \l signedIn() otherwise
    if an error occurs (e.g invalid email/password, no network access) then the signal \l errorOcurred() is emited with the error string.

    \note password must be a minimum of 6 characters.
 */
void FirebaseAuth::signUpWithEmailAndPassword(QString email, QString password, QString name)
{
    QUrl endpoint = AuthUtils::endpoint_signUp + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"email\":\"%1\",\"password\":\"%2\",\"returnSecureToken\":true,\"displayName\":\"%3\"}").arg(email).arg(password).arg(name);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                m_currentUser->setEmail(doc["email"].toString());
                m_currentUser->setIdToken(doc["idToken"].toString());
                m_currentUser->setRefreshToken(doc["refreshToken"].toString());
                m_currentUser->setUserId(doc["localId"].toString());

                emit currentUserChanged();
                emit signedIn();
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

/*!
    \qmlmethod void FirebaseAuth::signInWithOAuthCredential(string authToken, string providerId)

    Authenticates user with Google's \a authToken and \a providerId.
 */
void FirebaseAuth::signInWithOAuthCredential(QString authToken, QString providerId)
{
    QUrl endpoint = AuthUtils::endpoint_signInOAuth + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"postBody\":\"access_token=%1&providerId=%2\",\"requestUri\":\"http://127.0.0.1:8080\",\"returnIdpCredential\":true,\"returnSecureToken\":true}").arg(authToken).arg(providerId);

    qDebug().noquote() << "URL:  " << data;

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                m_currentUser->setName(doc["displayName"].toString());
                m_currentUser->setEmail(doc["email"].toString());
                m_currentUser->setIdToken(doc["idToken"].toString());
                m_currentUser->setRefreshToken(doc["refreshToken"].toString());
                m_currentUser->setUserId(doc["localId"].toString());
                m_currentUser->setEmailVerified(doc["emailVerified"].toBool());
                m_currentUser->setPhotoUrl(doc["photoUrl"].toString());

                emit currentUserChanged();
                emit signedIn();
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}


/*!
    \qmlmethod void FirebaseAuth::exchangeRefreshToken(string refreshToken)

    Exchanges a \a refreshToken for an ID token. When the user authenticates, both of these tokens are received. The ID token is necessary for all requests
    to the Firebase but has a short lifetime of 1 hour and needs to be refreshed with the refresh token by calling this method. When the response is received,
    the ID token is automatically updated on the \l currentUser.
 */
void FirebaseAuth::exchangeRefreshToken(QString refreshToken)
{
    QUrl endpoint = AuthUtils::endpoint_refreshToken + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QString data = QString("grant_type=refresh_token&refresh_token=%1").arg(refreshToken);
    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                m_currentUser->setIdToken(doc["id_token"].toString());
                m_currentUser->setRefreshToken(doc["refresh_token"].toString());
                m_currentUser->setUserId(doc["user_id"].toString());
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}


/*!
    \qmlmethod void FirebaseAuth::sendEmailVerification(string idToken)

    Sends verification email to the user with \a idToken.

    \sa sendPasswordResetEmail()
 */
void FirebaseAuth::sendEmailVerification(QString idToken)
{
    QUrl endpoint = AuthUtils::endpoint_sendEmailVerification + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"requestType\":\"VERIFY_EMAIL\",\"idToken\":\"%1\"}").arg(idToken);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                // Nothing to do here, maybe send success message
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

/*!
    \qmlmethod void FirebaseAuth::changeEmail(string idToken, string newEmail)

    Changes the email of the user with \a idToken to the new email \a newEmail.

    \sa changePassword()
 */
void FirebaseAuth::changeEmail(QString idToken, QString newEmail)
{
    QUrl endpoint = AuthUtils::endpoint_changeEmail + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"idToken\":\"%1\",\"email\":\"%2\",\"returnSecureToken\":true}").arg(idToken).arg(newEmail);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                m_currentUser->setEmail(doc["email"].toString());
                m_currentUser->setIdToken(doc["idToken"].toString());
                m_currentUser->setRefreshToken(doc["refreshToken"].toString());
                m_currentUser->setUserId(doc["localId"].toString());

                emit currentUserChanged();
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

void FirebaseAuth::confirmEmailVerification(QString verificationCode)
{
    QUrl endpoint = AuthUtils::endpoint_confirmEmailVerification + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"oobCode\":\"%1\"}").arg(verificationCode);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                // Nothing to do here, maybe send success message
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

/*!
    \qmlmethod void FirebaseAuth::sendPasswordResetEmail(string email)

    Sends request for password reset to \a email.
 */
void FirebaseAuth::sendPasswordResetEmail(QString email)
{
    QUrl endpoint = AuthUtils::endpoint_sendPasswordReset + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"requestType\":\"PASSWORD_RESET\",\"email\":\"%1\"}").arg(email);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                // Nothing to do here, maybe send success message
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

/*!
    \qmlmethod void FirebaseAuth::changePassword(string idToken, string newPassword)

    Changes the password of the user with \a idToken to the new password \a newPassword.

    \sa changeEmail()
 */
void FirebaseAuth::changePassword(QString idToken, QString newPassword)
{
    QUrl endpoint = AuthUtils::endpoint_changePassword + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"idToken\":\"%1\",\"password\":\"%2\",\"returnSecureToken\":true}").arg(idToken).arg(newPassword);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                m_currentUser->setEmail(doc["email"].toString());
                m_currentUser->setIdToken(doc["idToken"].toString());
                m_currentUser->setRefreshToken(doc["refreshToken"].toString());
                m_currentUser->setUserId(doc["localId"].toString());

                emit currentUserChanged();
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

void FirebaseAuth::verifyPasswordResetCode(QString verificationCode)
{
    QUrl endpoint = AuthUtils::endpoint_verifyPasswordReset + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"oobCode\":\"%1\"}").arg(verificationCode);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                // Nothing to do here, maybe send success message
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

void FirebaseAuth::confirmPasswordReset(QString verificationCode, QString newPassword)
{
    QUrl endpoint = AuthUtils::endpoint_confirmPasswordReset + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"oobCode\":\"%1\",\"newPassword\":\"%2\"}").arg(verificationCode).arg(newPassword);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                // Nothing to do here, maybe send success message
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

/*!
    \qmlmethod void FirebaseAuth::getUserData(string idToken)

    Retrieves information about the user with \a idToken, tipically the \l currentUser and automatically updates its properties.
 */
void FirebaseAuth::getUserData(QString idToken)
{
    QUrl endpoint = AuthUtils::endpoint_getUserData + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"idToken\":\"%1\"}").arg(idToken);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                QJsonObject user = doc["users"].toArray().at(0).toObject();

                if(!user["displayName"].toString().isEmpty())
                    m_currentUser->setName(user["displayName"].toString());
                m_currentUser->setEmailVerified(user["emailVerified"].toBool());
                m_currentUser->setPhotoUrl(user["photoUrl"].toString());
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

/*!
    \qmlmethod void FirebaseAuth::profileUpdate(string idToken, string name, string photoUrl)

    Updates the fields \a name and \a photoUrl of the user with \a idToken.
 */
void FirebaseAuth::profileUpdate(QString idToken, QString name, QString photoUrl)
{
    QUrl endpoint = AuthUtils::endpoint_profileUpdate + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"idToken\":\"%1\",\"displayName\":\"%2\",\"photoUrl\":\"%3\",\"returnSecureToken\":true}").arg(idToken).arg(name).arg(photoUrl);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                m_currentUser->setName(doc["displayName"].toString());
                m_currentUser->setPhotoUrl(doc["photoUrl"].toString());
                m_currentUser->setIdToken(doc["idToken"].toString());
                m_currentUser->setRefreshToken(doc["refreshToken"].toString());
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}

/*!
    \qmlmethod void FirebaseAuth::deleteAccount(string idToken)

    Permanently deletes the profile with \a idToken from the Firebase.
 */
void FirebaseAuth::deleteAccount(QString idToken)
{
    QUrl endpoint = AuthUtils::endpoint_deleteAccount + m_apiKey;
    QNetworkRequest request(endpoint);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString data = QString("{\"idToken\":\"%1\"}").arg(idToken);

    QNetworkReply *reply =  m_manager.post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray dat = reply->readAll();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(dat, &err);

        // Check for parsing errors
        if(err.error == QJsonParseError::NoError) {
            qDebug().noquote() << "\n RESPONSE: \n" << doc.toJson(QJsonDocument::Indented);

            // Check for reply errors
            QJsonObject error = doc["error"].toObject();
            if(!error.isEmpty()){
                emit errorOcurred(AuthUtils::parseJSONerror(error["message"].toString()));
            }
            else {
                emit signedOut();
            }
        } else {
            // If JSON did not parse, likely means a network error
            emit errorOcurred(reply->errorString());
        }

        reply->deleteLater();
    });
}
