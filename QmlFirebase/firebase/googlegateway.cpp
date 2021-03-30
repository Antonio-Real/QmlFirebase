#include "googlegateway.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>



/*!
    \qmltype GoogleGateway
    \inqmlmodule Firebase
    \ingroup Firebase
    \brief Helper for authenticating using a google account.

    This type allows the user to authenticate on the firebase using a google account. By calling the \l fetchGoogleToken()
    a browser window will be opened and the user can log in. After the login is complete, if successful an \l authToken
    will be retrieved which can be used with \l FirebaseAuth::signInWithOAuthCredential. See example below:

    \code
    FirebaseApp {
        id: fbApp
        apiKey: "insert-key"
        databaseUrl: "insert-url"
    }

    FirebaseAuth {
        id: fbAuth
        apiKey: fbApp.apiKey
        onSignedIn: ...
    }

    GoogleSignInHelper {
        id: googleSignIn

        onAuthTokenChanged: {
            console.log("Token granted:  \n" + authToken)
            fbAuth.signInWithOAuthCredential(authToken, "google.com")
        }
    }

    Button {
        text: "Google login"
        onClicked: googleSignIn.fetchGoogleToken()
    }
    \endcode

    \sa FirebaseAuth, FirebaseApp

 */
GoogleGateway::GoogleGateway(QObject *parent) : QObject(parent)
{
}


/*!
    \qmlproperty string GoogleGateway::authToken

    Token that can be used for authenticating on the firebase.
 */
QString GoogleGateway::authToken() const
{
    return m_authToken;
}

void GoogleGateway::setAuthToken(const QString &authToken)
{
    if(m_authToken == authToken)
        return;

    m_authToken = authToken;
    emit authTokenChanged();
}

/*!
    \qmlmethod void GoogleGateway::fetchGoogleToken()

    Opens a browser window to sign in with google. If successful, updates the \l authToken.
 */
void GoogleGateway::fetchGoogleToken()
{
    m_google->grant();
}

void GoogleGateway::parseClientSecret()
{
   if(!m_google)
        m_google = new QOAuth2AuthorizationCodeFlow(this);

    m_google->setScope("email profile");

    qDebug() << clientSecretPath;

    QByteArray val;
    QFile file;
    file.setFileName(clientSecretPath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        val = file.readAll();
        file.close();
    }
    else {
        qDebug() << "Google login error, no client_secret file found";
    }


    QJsonDocument document = QJsonDocument::fromJson(val);
    QJsonObject object = document.object();
    const auto settingsObject = object["web"].toObject();
    const QUrl authUri(settingsObject["auth_uri"].toString());
    const auto clientId = settingsObject["client_id"].toString();
    const QUrl tokenUri(settingsObject["token_uri"].toString());
    const auto clientSecret(settingsObject["client_secret"].toString());

    const auto redirectUris = settingsObject["redirect_uris"].toArray();

    // Change the index to the one of http://localhost:xxxx in client_secret.json
    const QUrl redirectUri(redirectUris[1].toString());
    const auto port = static_cast<quint16>(redirectUri.port());

    m_google->setAuthorizationUrl(authUri);
    m_google->setClientIdentifier(clientId);
    m_google->setAccessTokenUrl(tokenUri);
    m_google->setClientIdentifierSharedKey(clientSecret);

    if(m_replyhandler)
        delete m_replyhandler;

    m_replyhandler = new QOAuthHttpServerReplyHandler(port, this);
    m_google->setReplyHandler(m_replyhandler);

    m_google->setModifyParametersFunction([](QAbstractOAuth::Stage stage, QVariantMap* parameters) {
        // Percent-decode the "code" parameter so Google can match it
        if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
            QByteArray code = parameters->value("code").toByteArray();
            (*parameters)["code"] = QUrl::fromPercentEncoding(code);
        }
    });

    connect(m_google, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

    connect(m_google, &QOAuth2AuthorizationCodeFlow::granted, [this](){
        qDebug() << "Access Granted!";
        setAuthToken(m_google->token());
    });
}

/*!
    \qmlproperty string GoogleGateway::clientSecretPath

    Sets the path where the class can find the client_secret.json file to parse information such as client ID, token and redirect URIs, etc.
 */
void GoogleGateway::setClientSecretPath(const QString &value)
{
    clientSecretPath = value;
    parseClientSecret();
}
