#ifndef FIREBASEDATABASE_H
#define FIREBASEDATABASE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QJSValue>

class FirebaseDatabase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey REQUIRED)
    Q_PROPERTY(QString databaseUrl READ databaseUrl WRITE setDatabaseUrl REQUIRED)

public:
    explicit FirebaseDatabase(QObject *parent = nullptr);

public slots:
    void listenEvents(QString dbPath, QString idToken, int requestCode, bool ignoreFirstEvent = true, bool recursive = false);
    void pushValueWithUniqueKey(QString dbPath, QString jsonData, QString idToken);
    void writeValue(QString dbPath, QString jsonData, QString idToken);
    void updateValue(QString dbPath, QString jsonData, QString idToken);
    void getValue(QString dbPath, QString idToken, int requestCode);
    void deleteValue(QString dbPath, QString idToken);

signals:
    void dataRetrieved(QByteArray data, int requestCode);
    void dataEvent(QByteArray data, int requestCode);

    // Signals for when operations are finished
    void getValueFinished();
    void updateValueFinished();
    void writeValueFinished();
    void pushValueFinished();
    void deleteValueFinished();

private:
    QString apiKey() const;
    void setApiKey(const QString &apiKey);

    QString databaseUrl() const;
    void setDatabaseUrl(const QString &databaseUrl);


private:
    QString m_apiKey;
    QString m_databaseUrl;
    QNetworkAccessManager m_manager;

};

#endif // FIREBASEDATABASE_H
