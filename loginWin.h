#ifndef loginWin_H
#define loginWin_H

#include <QWidget>
#include "ui_loginWin.h"
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "interfaceCTP/ThostFtdcUserApiDataType.h"
#include "interfaceCTP/ThostFtdcUserApiStruct.h"
extern "C"{
#include "interfaceCTP/ThostFtdcTraderApi.h"
#include "interfaceCTP/ThostFtdcMdApi.h"
}
#include "tradespiimp.h"
#include "mdspiimp.h"
#include <QDebug>

enum TRADE_SIGNAL{LOGIN,GOT_M1,GOT_M5,GOT_M15,GOT_D1,LOGOUT};


struct DBServerInfo {
    char name[32];
    char dll[32];
    char params[3000];
    int  length;
};

struct UserInfo
{
    char name[16];
    char pass[16];
    char BrokerID[16];
};

struct ServerAddr
{
    QString tradeServ;
    QString marketServ;
};

struct PrivateIns
{
    TThostFtdcInstrumentIDType InstrumentID[100];
    int clickmode;
    int notice;
    int fPoints;
    char versionUrl[64];
    char exePath[64];
};

class loginWin : public QWidget
{
    Q_OBJECT

public:
    loginWin(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~loginWin();

    char userName[16];
    char password[16];
    DBServerInfo server;
    UserInfo m_users;
    QList<ServerAddr> m_listAddr;
    int  serverAddrIndex = 0;
    QTimer * timeLimiter;
    void getUserInfos();
    void loginSucceed();
    void loginFailed(int code, QString mess);
    void logutSecced();

    // 登录用户信息答复
    CThostFtdcRspUserLoginField loginRes;

private:
    Ui::loginWinClass ui;

public slots:
    void login_clicked();

signals:
    void pushTrade();
    void errShow(int code, QString mess);
    void loginSec();
    void pushTradeToFrom(TRADE_SIGNAL);
protected:
    void keyPressEvent ( QKeyEvent * event );
};

#endif // loginWin_H
