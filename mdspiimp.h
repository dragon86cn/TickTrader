#ifndef MDSPIIMP_H
#define MDSPIIMP_H

#include "loginWin.h"
#include "interfaceCTP/ThostFtdcMdApi.h"

class CMdSpiImp : public CThostFtdcMdSpi
{
public:
    CMdSpiImp(CThostFtdcMdApi* pMdApi);

    ~CMdSpiImp(void);

    // 登录状态
    bool loginFlags;

     //连接建立通知
    /* 说明：
        1. 此连接为系统连接，即便没有任何用户登录，该连接依然会维持在线状态
    */
    virtual void OnFrontConnected();

    //连接断开通知，用户无需处理，API会自动重连
    /* 说明：
        1. 连接断开后，所有在此连接上登录的用户均处于离线状态，重新连接后均需要重新发送登录请求
    */
    virtual void OnFrontDisconnected(int nReason);

    /*登录应答*/
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*登出应答*/
    virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*行情订阅应答*/
    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*行情退订应答*/
//    virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*行情通知*/
    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

    // 请求实例
    CThostFtdcMdApi* m_pQuotApi;
};

#endif // MDSPIIMP_H
