#include "firebaseapp.h"
#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkReply>
#include <QDataStream>

/*!
    \qmlmodule Firebase 1.0
 */


/*!
    \qmltype FirebaseApp
    \inqmlmodule Firebase
    \ingroup Firebase
    \brief Type that holds all relevant data related to the Firebase project.

    The FirebaseApp type holds crucial data from the Firebase project such as API Key or database URL which
    will then be passed on to \l FirebaseAuth or \l FirebaseDatabase.
    The properties of the Firebase App can be set by downloading the \a google-services.json file from Firebase
    and store it in the resources for automatic parsing. An alternative is manually setting the properties in qml:

    \code
    FirebaseApp {
        id: fbApp
        apiKey: "insert-key"
        databaseUrl: "insert-url"
    }

    FirebaseDatabase {
        apiKey: fbApp.apiKey
        databaseUrl: fbApp.databaseUrl
    }
    \endcode

    \sa FirebaseAuth, FirebaseDatabase, FirebaseUser

 */
FirebaseApp::FirebaseApp(QObject *parent) : QObject(parent)
{
    connect(this, &FirebaseApp::sourceChanged, this, &FirebaseApp::parseFirebaseJson);
}

/*!
    \qmlproperty string FirebaseApp::apiKey

    This property holds the API Key of the Firebase project, used by the types \l FirebaseDatabase and \l FirebaseAuth.
 */
QString FirebaseApp::apiKey() const
{
    return m_apiKey;
}

/*!
    \qmlproperty string FirebaseApp::authDomain

    Domain of the authentication, currently not used.
 */
QString FirebaseApp::authDomain() const
{
    return m_authDomain;
}

/*!
    \qmlproperty string FirebaseApp::databaseUrl

    Property that holds the URL where the Firebase Realtime Database resides and should be passed
    to \l FirebaseDatabase objects.
 */
QString FirebaseApp::databaseUrl() const
{
    return m_databaseUrl;
}

/*!
    \qmlproperty string FirebaseApp::storageBucket

    This property holds the URL of the Firebase Storage, currently not implemented.
 */
QString FirebaseApp::storageBucket() const
{
    return m_storageBucket;
}

void FirebaseApp::setApiKey(const QString &apiKey)
{
    if(m_apiKey == apiKey)
        return;
    m_apiKey = apiKey;
    emit apiKeyChanged();
}

void FirebaseApp::setAuthDomain(const QString &authDomain)
{
    if(m_authDomain == authDomain)
        return;

    m_authDomain = authDomain;
    emit authDomainChanged();
}

void FirebaseApp::setDatabaseUrl(const QString &databaseUrl)
{
    if(m_databaseUrl == databaseUrl)
        return;

    m_databaseUrl = databaseUrl;
    emit databaseUrlChanged();
}

void FirebaseApp::setStorageBucket(const QString &storageBucket)
{
    if(m_storageBucket == storageBucket)
        return;

    m_storageBucket = storageBucket;
    emit storageBucketChanged();
}

/*!
    \qmlproperty url FirebaseApp::source

    This property holds the URL of the google-services.json that contains all information about a Firebase Project (usually obtained in Firebase Console).
    If this path is set, the file will be parsed and all the other properties will be automatically set.
 */
QString FirebaseApp::source() const
{
    return m_source;
}

void FirebaseApp::setSource(const QString &source)
{
    if(m_source == source)
        return;

    m_source = source;
    emit sourceChanged();
}

// Private function for parsing google-services.json file to scrape API Key, Database URL, etc
void FirebaseApp::parseFirebaseJson()
{
    QFile file(m_source);

    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return;

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError errorPtr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &errorPtr);
    if (errorPtr.error == QJsonParseError::NoError) {
        QJsonObject root = doc.object();
        QJsonObject projectInfo = root["project_info"].toObject();
        QJsonArray client = root["client"].toArray();

        setDatabaseUrl(projectInfo["firebase_url"].toString() + "/");
        setStorageBucket(projectInfo["storage_bucket"].toString());
        setAuthDomain(projectInfo["project_id"].toString() + ".firebase.com");


        if(client.count() > 0) {
            QJsonArray api_key = client.at(0).toObject()["api_key"].toArray();

            if(api_key.count() > 0) {
                setApiKey(api_key.at(0).toObject()["current_key"].toString());
            }
        }
    }
}

