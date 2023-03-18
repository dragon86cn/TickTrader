#include "mdspiimp.h"
#include "tradewidget.h"
#include <QDebug>

extern loginWin *loginW;
extern TradeWidget *g_tw;

extern quint32 CreateNewRequestID();

CMdSpiImp::CMdSpiImp(CThostFtdcMdApi* pMdApi)
    :m_pQuotApi(pMdApi)
{
    loginFlags = false;
}

CMdSpiImp::~CMdSpiImp()
{

}

void CMdSpiImp::OnFrontConnected()
{
    qInfo() << "CMdSpiImp::OnFrontConnected";
    if(!loginFlags)
    {
        CThostFtdcReqUserLoginField login = {0};
        strncpy( login.BrokerID, loginW->m_users.BrokerID, sizeof(login.BrokerID));
        strncpy( login.UserID, loginW->userName, sizeof(login.UserID));
        strncpy( login.Password, loginW->password, sizeof(login.Password));
        strcpy( login.UserProductInfo, "TICKTRADER" );
        strcpy( login.MacAddress, "fe80::75ef:97de:2366:e490" );
        strcpy( login.ClientIPAddress, "10.150.1.23" );

        quint32 nRequestID = CreateNewRequestID();
        int ret = m_pQuotApi->ReqUserLogin(&login, nRequestID);
        qInfo() << "ReqUserLogin ret: " << ret;
    }
}

void CMdSpiImp::OnFrontDisconnected(int nReason)
{

}

void CMdSpiImp::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if(pRspInfo && pRspInfo->ErrorID!=0){
        return;
    }
    if(!pRspUserLogin)
        return;
    loginFlags = true;
    // 遍历一次订阅列表
    int count = 1;
    char **InstrumentID = new char*[count];
    QMapIterator<QString, CThostFtdcInstrumentField *> s(g_tw->insMap_dy);
    while (s.hasNext())
    {
        QString key = s.next().key();
        CThostFtdcInstrumentField * si = g_tw->insMap_dy[key];
        if(!si)
            continue;
        InstrumentID[0] = si->InstrumentID;
        m_pQuotApi->SubscribeMarketData(InstrumentID, 1); // 订阅行情
    }
    delete[] InstrumentID;
}

void CMdSpiImp::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

}

void CMdSpiImp::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

//void CMdSpiImp::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{

//}

void CMdSpiImp::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    if(pDepthMarketData)
    {
        g_tw->priceEmit(pDepthMarketData);
    }
}
