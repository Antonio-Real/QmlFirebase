QT += network networkauth

INCLUDEPATH += $$PWD

SOURCES += \
	$$PWD/firebase/firebaseapp.cpp \
	$$PWD/firebase/firebaseauth.cpp \
	$$PWD/firebase/firebasedatabase.cpp \
	$$PWD/firebase/firebaseuser.cpp \
        $$PWD/firebase/googlegateway.cpp \
        $$PWD/firebase/firebaseqmltypes.cpp \
		
HEADERS += \
    $$PWD/utils/AuthUtils.h \
    $$PWD/firebase/firebaseapp.h \
    $$PWD/firebase/firebaseauth.h \
    $$PWD/firebase/firebasedatabase.h \
    $$PWD/firebase/firebaseuser.h \
    $$PWD/firebase/googlegateway.h

