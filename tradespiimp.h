#ifndef TRADESPIIMP_H
#define TRADESPIIMP_H

#include "loginWin.h"

class CTradeSpiImp : public CThostFtdcTraderSpi
{
public:
    CTradeSpiImp(CThostFtdcTraderApi* pTraderApi);
    ~CTradeSpiImp(void);

    CThostFtdcTraderApi* m_pTradeApi;

    // 登录状态
    bool loginFlags;

    // 记录合约清单
    QMap<QString, CThostFtdcInstrumentField *> tempCons;

    //连接建立通知
    /* 说明：
        1. 此连接为系统连接，即便没有任何用户登录，该连接依然会维持在线状态
    */
    virtual void OnFrontConnected();

    //连接断开通知，用户无需处理，API会自动重连
    /* 说明：
        1. 连接断开后，所有在此连接上登录的用户均处于离线状态，重新连接后均需要重新发送登录请求
    */
    virtual void OnFrontDisconnected();

    /*登录应答*/
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*登出应答*/
    virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*修改密码应答*/
    virtual void OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*订单录入应答*/
    virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*订单通知*/
    virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

    /*撤单应答*/
    virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*市场状态通知*/
//    virtual void OnMarketStatusNty(stBCESMarketStatusNty *pMarketStatusNty);

    /*成交通知*/
    virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

    /*行情查询应答*/
    virtual void OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*订单查询应答*/
    virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*成交查询应答*/
    virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*持仓查询应答*/
    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*资金查询应答*/
    virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*合约查询应答*/
    virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*持仓明细查询应答*/
    virtual void OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    /*资金通知*/
 //   virtual void OnFundNty(CThostFtdcTradingAccountField *pFund);

    /*持仓通知*/
 //   virtual void OnPosiNty(CThostFtdcInvestorPositionField *pPosi);
};

#endif // TRADESPIIMP_H
