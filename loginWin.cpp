#ifdef WIN32
#include <windows.h>
#endif

#include "loginWin.h"
#include "tradewidget.h"
#include <QUrl>
#include <QMovie>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QDesktopWidget>
#include "configWidget.h"
#include "notice.h"
#include <QComboBox>
#include <QKeyEvent>
#include <qlibrary.h>
#include <QSettings>

extern TradeWidget * g_tw;
extern configWidget * cview;

loginWin::loginWin(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    ui.setupUi(this);
    getUserInfos();
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui.centralWidget->setAutoFillBackground(true);
    // setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    // ::SetWindowPos(HWND(winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    QPalette palette;
    QPixmap pixmap(":/image/images/login.png");
    palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(414, 292)));
    ui.centralWidget->setPalette(palette);
    //QMovie *movie = new QMovie(":/timg/timg.gif");
    //ui.label->setMovie(movie);
    //movie->start();

    qRegisterMetaType<TRADE_SIGNAL>("TRADE_SIGNAL");

    ui.statusBar->hide();
    ui.label_2->hide();
    connect(ui.btnLogin, SIGNAL(clicked()),this, SLOT(login_clicked()));
}

loginWin::~loginWin()
{
    exit(0);
}

void loginWin::getUserInfos()
{
    memset(&m_users, 0, sizeof(m_users));
    memcpy(m_users.BrokerID, "9999", sizeof(m_users.BrokerID));
    QSettings cfgini("./config.ini", QSettings::IniFormat);
    cfgini.beginGroup("userInfo");
    QString str = cfgini.value("account").toString();
    memcpy(m_users.name, str.toStdString().c_str(), sizeof(m_users.name));
    ui.lineEditUser->setText(str);

    str = cfgini.value("password").toString();
    memcpy(m_users.pass, str.toStdString().c_str(), sizeof(m_users.pass));
    ui.lineEdit->setText(str);
    cfgini.endGroup();

    ServerAddr addr;
    cfgini.beginGroup("serverAddress1");
    addr.tradeServ = cfgini.value("trade").toString();
    addr.marketServ = cfgini.value("market").toString();
    cfgini.endGroup();
    m_listAddr.append(addr);
    cfgini.beginGroup("serverAddress2");
    addr.tradeServ = cfgini.value("trade").toString();
    addr.marketServ = cfgini.value("market").toString();
    cfgini.endGroup();
    m_listAddr.append(addr);
}

void loginWin::login_clicked()
{
    QString str = ui.lineEditUser->text();
    memcpy(userName, str.toStdString().c_str(), sizeof(userName));
    str = ui.lineEdit->text();
    memcpy(password, str.toStdString().c_str(), sizeof(password));
    serverAddrIndex = ui.servBox->currentIndex();

    if(pTraderApi == NULL)
    {
        pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi("trade");
        pTraderSpi = new CTradeSpiImp(pTraderApi);
        if( pTraderApi == NULL || pTraderSpi == NULL )
        {
            return;
        }
        pTraderApi->RegisterSpi(pTraderSpi);
        pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
        pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);
        pTraderApi->RegisterFront((char*)m_listAddr.at(serverAddrIndex).tradeServ.toStdString().c_str());
        pTraderApi->Init( );
    }
}

void loginWin::keyPressEvent( QKeyEvent * event )
{
    int key = event->key();
    if( key == Qt::Key_Enter || key == Qt::Key_Return) // Qt::Key_Enter
    {
        login_clicked();
    }
}

void loginWin::loginSucceed()
{
    QSettings cfgini("./config.ini", QSettings::IniFormat);
    cfgini.beginGroup("userInfo");
    cfgini.setValue("account", userName);
    cfgini.setValue("password", password);
    memcpy(m_users.name, userName, sizeof(m_users.name));
    memcpy(m_users.pass, password, sizeof(m_users.pass));
    cfgini.endGroup();

    emit loginSec();
}

void loginWin::loginFailed(int code,  QString mess)
{
    emit errShow(code, mess);
}

void loginWin::logutSecced()
{
    emit pushTradeToFrom(LOGOUT);
}
