#include <QUrlQuery>
#include <QNetworkReply>
#include <QJSEngine>
#include "firebasedatabase.h"


/*!
    \qmltype FirebaseDatabase
    \inqmlmodule Firebase
    \ingroup Firebase
    \brief Type that offers methods for adding, updating and deleting data from the Firebase Realtime Database.

    FirebaseDatabase provides methods for adding, updating and deleting data from the Firebase Realtime Database.
    Moreover, it also provides the method \l listenEvents that listens to events in a given database path and fires
    a signal when an event occurs.
*/
FirebaseDatabase::FirebaseDatabase(QObject *parent) : QObject(parent)
{

}

/*!
    \qmlsignal FirebaseDatabase::dataRetrieved(string data, int requestCode)

    Emitted when the response to a call of \l getValue() arrives and sends the corresponding \a data and \a requestCode.

    \sa getValue()
 */

/*!
    \qmlsignal FirebaseDatabase::dataUpdated()

    Emitted when a call to \l updateValue() is successfull.
 */

/*!
    \qmlsignal FirebaseDatabase::dataEvent(string data, int requestCode)

    Emitted when an event from a listener occurs, sends the server response \a data and the corresponding \a requestCode.

    \sa listenEvents()
 */

/*!
    \qmlsignal FirebaseDatabase::getValueFinished()

    Emitted when a call to \l getValue() finishes.
 */
/*!
    \qmlsignal FirebaseDatabase::updateValueFinished()

    Emitted when a call to \l updateValue() finishes.
 */
/*!
    \qmlsignal FirebaseDatabase::writeValueFinished()

    Emitted when a call to \l writeValue() finishes.
 */
/*!
    \qmlsignal FirebaseDatabase::pushValueFinished()

    Emitted when a call to \l pushValueWithUniqueKey() finishes.
 */
/*!
    \qmlsignal FirebaseDatabase::deleteValueFinished()

    Emitted when a call to \l deleteValue() finishes.
 */


/*!
    \qmlmethod void FirebaseDatabase::listenEvents(string dbPath, string idToken, int requestCode, bool ignoreFirstEvent, bool recursive)

    Registers a listener to the database path \a dbPath with \a idToken if the Firebase Database rules require authentication.

    When an event occurs (e.g data was added, updated or deleted) on that path, the signal \l dataEvent() is triggered with the
    contents of the server response atached to the code \a requestCode. The request code
    provides a way to differentiate where the response comes from, as seen in the example:

    \code
    FirebaseDatabase {
        id: fbDb
        ...
        onDataEvent: { // params (data, requestCode)
            if(requestCode == 10)
                console.log(data) // data contains event from Users path
            else if(requestCode == 20)
                console.log(data) // data contains event from Alerts path
        }
        Component.onCompleted: {
            fbDb.listenEvents("/Users/.json", "insert-if-necessary", 10)
            fbDb.listenEvents("/Alerts/.json", "insert-if-necessary", 20)
        }
    }
    \endcode

    When an event listener is first registered, the first event will always return the entire contents of the chosen path.
    To ignore the first event, set \a ignoreFirstEvent to true (if no parameter passed, is true by default).

    If you want the listener to be recursive (i.e it re-registers itself when the server closes connection) set the argument \a recursive to true.

    \sa dataEvent()
 */
void FirebaseDatabase::listenEvents(QString dbPath, QString idToken, int requestCode, bool ignoreFirstEvent, bool recursive)
{
    QUrl url = m_databaseUrl + dbPath;

    // Check if idToken was passed
    if(!idToken.isEmpty()) {
        QUrlQuery query;
        query.addQueryItem("auth", idToken);
        url.setQuery(query);
    }

    // Open connection with server
    QNetworkRequest request(url);
    request.setRawHeader("Accept", "text/event-stream");
    QNetworkReply *reply = m_manager.get(request);

    // Event received lambda
    connect(reply, &QNetworkReply::readyRead, this, [=]() mutable {
        QByteArray data = reply->readAll();

        // Ignore the keep-alive events
        if(!data.contains("keep-alive")) {
            if(!ignoreFirstEvent) {
                dataEvent(data, requestCode);
                qDebug().noquote() << "EVENT\n" << data;
            } else ignoreFirstEvent = false;
        }
    });

    // Event finished lambda
    connect(reply, &QNetworkReply::finished, this, [=](){
        if (reply) {
            QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (!redirectUrl.isEmpty()) {
                // Do something with redirect URL here
            }
            reply->deleteLater();
            qDebug() << "Event finished";
            if(recursive) {
                listenEvents(dbPath, idToken, requestCode, ignoreFirstEvent, recursive);
                qDebug() << "Registering listener again";
            }
        }
    });
}


/*!
    \qmlmethod void FirebaseDatabase::pushValueWithUniqueKey(string dbPath, string jsonData, string idToken)

    Writes to the database path \a dbPath a new entry with a randomized unique key and value \a jsonData.

    \code
    FirebaseDatabase {
        id: fbDb
        ...
    }

    Button {
        text: "Push value"
        onClicked: fbDb.pushValueWithUniqueKey("/random.json", JSON.stringify({"text":"Foo Bar"}), fbAuth.currentUser.idToken)
    }
    \endcode

    In the example above, if the button is repeatedly clicked, multiple entries with random keys will be added to the path.

    \sa writeValue(), updateValue(), deleteValue()
 */
void FirebaseDatabase::pushValueWithUniqueKey(QString dbPath, QString jsonData, QString idToken)
{
    QUrl url = m_databaseUrl + dbPath;

    // Check if idToken was passed
    if(!idToken.isEmpty()) {
        QUrlQuery query;
        query.addQueryItem("auth", idToken);
        url.setQuery(query);
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = m_manager.post(request, jsonData.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=](){
        //qDebug().noquote() << "PUSH RESPONSE: \n" << reply->readAll();
        emit pushValueFinished();
    });
}


/*!
    \qmlmethod void FirebaseDatabase::writeValue(string dbPath, string jsonData, string idToken)

    Writes to the database path \a dbPath a new entry with a randomized unique key and value \a jsonData.

    \warning Careful use of this method is required since the last specifier of the path denotes the key of the new entry
    and all data in that path will be overwritten, for example:

    \code
    FirebaseDatabase {
        id: fbDb
        ...
    }
    Button {
        id: button1
        onClicked: fbDb.writeValue("/random/myText.json", JSON.stringify({"text":"Foo Bar"}), fbAuth.currentUser.idToken)
    }
    Button {
        id: button2
        onClicked: fbDb.writeValue("/random.json", JSON.stringify({"text":"Foo Bar"}), fbAuth.currentUser.idToken)
    }
    \endcode

    In the example above, if button1 is pressed, a new entry with key \a myText and the given value, as expected. If button2 is pressed,
    the entire contents of the path "/random.json" will be replaced by the given value, as opposed to inserting the new entry in the "/random.json" path.

    \sa pushValueWithUniqueKey(), updateValue(), deleteValue()
 */
void FirebaseDatabase::writeValue(QString dbPath, QString jsonData, QString idToken)
{
    QUrl url = m_databaseUrl + dbPath;

    // Check if idToken was passed
    if(!idToken.isEmpty()) {
        QUrlQuery query;
        query.addQueryItem("auth", idToken);
        url.setQuery(query);
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = m_manager.put(request, jsonData.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=](){
        qDebug().noquote() << "WRITE DATA RESPONSE: \n" << reply->readAll();
        emit writeValueFinished();
    });
}

/*!
    \qmlmethod void FirebaseDatabase::updateValue(string dbPath, string jsonData, string idToken)

    Updates the entry in \a dbPath with the new value in \a jsonData. The contents of \a jsonData can be used to update the entire entry
    or just patch one of the fields, for example:

    \code
    FirebaseDatabase {
        id: fbDb
        ...
    }

    Button {
        text: "Push value"
        onClicked: fbDb.pushValueWithUniqueKey("/random/myData.json", JSON.stringify({"text":"Foo Bar", "myInt" : 10}), fbAuth.currentUser.idToken)
    }
    Button {
        text: "Update value"
        onClicked: fbDb.pushValueWithUniqueKey("/random/myData.json", JSON.stringify({"text":"Foo Bar"}), fbAuth.currentUser.idToken)
    }
    \endcode

    In the example above, if the value is pushed and then updated, only the text will be updated and "myInt" will remain the same.

    \sa writeValue()
 */
void FirebaseDatabase::updateValue(QString dbPath, QString jsonData, QString idToken)
{
    QUrl url = m_databaseUrl + dbPath;

    // Check if idToken was passed
    if(!idToken.isEmpty()) {
        QUrlQuery query;
        query.addQueryItem("auth", idToken);
        url.setQuery(query);
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QNetworkReply *reply = m_manager.sendCustomRequest(request, "PATCH", jsonData.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=](){
        //qDebug().noquote() << "PATCH DATA RESPONSE: \n" << reply->readAll();
        emit updateValueFinished();
    });
}


/*!
    \qmlmethod void FirebaseDatabase::getValue(string dbPath, string idToken, int requestCode)

    Requests all the data in path \a dbPath with \a idToken if the Firebase Database rules require authentication.

    When the request is successful the signal \l dataRetrieved() is triggered with the
    contents of the server response atached to the code \a requestCode. The request code
    provides a way to differentiate where the response comes from, as seen in the example:

    \code
    FirebaseDatabase {
        id: fbDb
        ...
        onDataRetrieved: { // params (data, requestCode)
            if(requestCode == 10)
                console.log(data) // data contains event from Users path
            else if(requestCode == 20)
                console.log(data) // data contains event from Alerts path
        }
        Component.onCompleted: {
            fbDb.getValue("/Users/.json", "insert-if-necessary", 10)
            fbDb.getValue("/Alerts/.json", "insert-if-necessary", 20)
        }
    }
    \endcode

    \sa dataRetrieved()
 */
void FirebaseDatabase::getValue(QString dbPath, QString idToken, int requestCode)
{
    QUrl url = m_databaseUrl + dbPath;

    // Check if idToken was passed
    if(!idToken.isEmpty()) {
        QUrlQuery query;
        query.addQueryItem("auth", idToken);
        url.setQuery(query);
    }

    QNetworkRequest request(url);

    QNetworkReply *reply = m_manager.get(request);

    connect(reply, &QNetworkReply::finished, this, [=](){
        //qDebug().noquote() << "GETVALUE RESPONSE: \n" << reply->readAll();
        QByteArray data = reply->readAll();

        if(reply->error() == QNetworkReply::NoError)
            emit dataRetrieved(data, requestCode);

        emit getValueFinished();
    });
}

/*!
    \qmlmethod void FirebaseDatabase::deleteValue(string dbPath, string idToken)

    Deletes all data in path \a dbPath with \a idToken if the Firebase Database rules require authentication.
 */
void FirebaseDatabase::deleteValue(QString dbPath, QString idToken)
{
    QUrl url = m_databaseUrl + dbPath;

    // Check if idToken was passed
    if(!idToken.isEmpty()) {
        QUrlQuery query;
        query.addQueryItem("auth", idToken);
        url.setQuery(query);
    }

    QNetworkRequest request(url);

    QNetworkReply *reply = m_manager.sendCustomRequest(request, "DELETE");

    connect(reply, &QNetworkReply::finished, this, [=](){
        //qDebug().noquote() << "DELETE RESPONSE: \n" << reply->readAll();
        emit deleteValueFinished();
    });
}


/*!
    \qmlproperty string FirebaseDatabase::apiKey

    This property holds the API Key of the Firebase project, obtained from \l FirebaseApp.
 */
QString FirebaseDatabase::apiKey() const
{
    return m_apiKey;
}

/*!
    \qmlproperty string FirebaseDatabase::databaseUrl

    This property holds the database where the Firebase Realtime Database resides, obtained from \l FirebaseApp.
 */
QString FirebaseDatabase::databaseUrl() const
{
    return m_databaseUrl;
}

void FirebaseDatabase::setDatabaseUrl(const QString &databaseUrl)
{
    m_databaseUrl = databaseUrl;
}


void FirebaseDatabase::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}
