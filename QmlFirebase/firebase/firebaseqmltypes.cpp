#include "firebaseapp.h"
#include "firebaseauth.h"
#include "firebasedatabase.h"
#include "firebaseuser.h"
#include "googlegateway.h"
#include <QCoreApplication>
#include <QQmlEngine>

void registerFirebaseTypes() {
    qmlRegisterType<FirebaseApp>("Firebase", 1,0, "FirebaseApp");
    qmlRegisterType<FirebaseAuth>("Firebase", 1,0, "FirebaseAuth");
    qmlRegisterType<FirebaseUser>("Firebase", 1,0, "FirebaseUser");
    qmlRegisterType<FirebaseDatabase>("Firebase", 1,0, "FirebaseDatabase");
    qmlRegisterType<GoogleGateway>("Firebase", 1,0, "GoogleGateway");
}

Q_COREAPP_STARTUP_FUNCTION(registerFirebaseTypes)
