QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = TickTrader
TEMPLATE = app

DESTDIR = "../Out"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    InstrManage.cpp \
    OrderView.cpp \
    PriceView.cpp \
    changePassword.cpp \
    configWidget.cpp \
    coolsubmit.cpp \
    dealView.cpp \
    help.cpp \
    loginWin.cpp \
    main.cpp \
    notice.cpp \
    orderManage.cpp \
    posiManage.cpp \
    tradewidget.cpp \
    tradespiimp.cpp \
    mdspiimp.cpp

HEADERS += \
    InstrManage.h \
    OrderView.h \
    PriceView.h \
    changePassword.h \
    configWidget.h \
    coolsubmit.h \
    dealView.h \
    help.h \
    interfaceCTP/ThostFtdcTraderApi.h \
    interfaceCTP/ThostFtdcUserApiDataType.h \
    interfaceCTP/ThostFtdcUserApiStruct.h \
    loginWin.h \
    notice.h \
    orderManage.h \
    posiManage.h \
    tradewidget.h \
    tradespiimp.h \
    interfaceCTP/ThostFtdcMdApi.h \
    mdspiimp.h \
    appapi.h

FORMS += \
    changePassword.ui \
    configWidget.ui \
    help.ui \
    loginWin.ui \
    tradewidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    tradewidget.qrc


RC_FILE += version.rc

unix|win32: LIBS += -L$$PWD/interfaceCTP/ -lthostmduserapi_se
unix|win32: LIBS += -L$$PWD/interfaceCTP/ -lthosttraderapi_se

INCLUDEPATH += $$PWD/interfaceCTP
DEPENDPATH += $$PWD/interfaceCTP
