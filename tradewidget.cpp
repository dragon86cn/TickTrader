#ifdef WIN32
#include "windows.h"
#endif

#include "tradewidget.h"
#include <QContextMenuEvent>
#include <string>
#include <QDateTime>
#include <qlibrary.h>
#include "coolsubmit.h"
#include "orderManage.h"
#include "posiManage.h"
#include "help.h"
#include <QSettings>
#include <QFile>
#include <QUrl>
#include <QTextStream>
#include "PriceView.h"
#include "configWidget.h"
#include "changePassword.h"
#include "dealView.h"
#include "notice.h"
#include <time.h>
#include <QThread>

extern configWidget * cview;
extern changePassword *cpView;
extern loginWin * loginW;
extern TradeWidget * g_tw;

clock_t a;

typedef struct
{
    QString name;
    QString dll;
    QString params;
}RouteInfo;

#define BCESMacroFlowResumeMethodResume 2
#define ORDERWIDGETW 380
#define GRAPHH 300

using namespace std;

static quint32 g_RequestID = 0;

// 线程锁
//QMutex mutex;

quint32 CreateNewRequestID()
{
    ++g_RequestID;
    return g_RequestID;
}

static int virtualOrderId = 100000;

quint32 CreateVirtualOrderId()
{
    ++virtualOrderId;
    return virtualOrderId;
}

CThostFtdcTraderApi* pTraderApi = NULL;
CTradeSpiImp *pTraderSpi = NULL;

CThostFtdcMdApi* pQuotApi = NULL;
CMdSpiImp * quotSpi = NULL;

QMap<QString, TradeInfo> tradeInfoLst;

// 交易所号 文字转换
QString convertExchangeID(TThostFtdcExchangeIDType ID)
{
    int x = strlen(ID);
    QString str = QString::fromLocal8Bit(ID);
    if(::strcmp(ID, "SHFE") == 0)
    {
        str = QString::fromLocal8Bit("上期所");
    }
    else if(::strcmp(ID, "DCE") == 0)
    {
        str = QString::fromLocal8Bit("大商所");
    }
    else if(::strcmp(ID, "CZCE") == 0)
    {
        str = QString::fromLocal8Bit("郑商所");
    }
    else if(::strcmp(ID, "CFFEX") == 0)
    {
        str = QString::fromLocal8Bit("中金所");
    }
    else if(::strcmp(ID, "SZ") == 0)
    {
        str = QString::fromLocal8Bit("深市");
    }
    else if(::strcmp(ID, "SH") == 0)
    {
        str = QString::fromLocal8Bit("沪市");
    }
    else if(::strcmp(ID, "INE") == 0)
    {
        str = QString::fromLocal8Bit("能源中心");
    }
    else if(::strcmp(ID, "NULL") == 0)
    {
        str = QString::fromLocal8Bit("TickTrader");
    }
    return str;
}

// 价格类型 文字转换
QString convertPriceType(TThostFtdcOrderPriceTypeType flag)
{
    QString str = "";
    switch(flag)
    {
    case THOST_FTDC_OPT_LimitPrice:
        str = QString::fromLocal8Bit("限价");
        break;
    case THOST_FTDC_OPT_AnyPrice:
        str = QString::fromLocal8Bit("市价");
        break;
//	case BCESConstPriceTypeStop:
//		str = QString::fromLocal8Bit(BCESConstCommentPriceTypeStop);
//		break;
//	case BCESConstPriceTypeLimit:
//		str = QString::fromLocal8Bit(BCESConstCommentPriceTypeLimit);
//		break;
    }
    return str;
}

// 开平 文字转换
QString convertOcFlag(TThostFtdcOffsetFlagType flag)
{
    QString str = "";
    switch(flag)
    {
    case THOST_FTDC_OF_Open:
        str = QString::fromLocal8Bit("开");
        break;
    case THOST_FTDC_OF_Close:
        str = QString::fromLocal8Bit("平");
        break;
    case THOST_FTDC_OF_CloseToday:
        str = QString::fromLocal8Bit("平今");
        break;
    case THOST_FTDC_OF_ForceClose:
        str = QString::fromLocal8Bit("强平");
        break;
    }
    return str;
}


// 买卖 文字转换
QString convertBsFlag(TThostFtdcDirectionType flag)
{
    QString str = "";
    switch(flag)
    {
    case THOST_FTDC_D_Buy:
        str = QString::fromLocal8Bit("买");
        break;
    case THOST_FTDC_D_Sell:
        str = QString::fromLocal8Bit("卖");
        break;
//	case BCESConstBSFlagExecute:
//		str = QString::fromLocal8Bit(BCESConstCommentBSFlagExecute);
//		break;
    }
    return str;
}

// 订单状态文字转换
QString convertOrderStatus(TThostFtdcOrderStatusType flag)
{
    QString str = "";
    switch(flag)
    {
    case THOST_FTDC_OST_NoTradeNotQueueing:
        str = QString::fromLocal8Bit("申报中");
        break;
    case THOST_FTDC_OST_NoTradeQueueing:
        str = QString::fromLocal8Bit("已申报");
        break;
    case THOST_FTDC_OST_AllTraded:
        str = QString::fromLocal8Bit("全部成交");
        break;
    case THOST_FTDC_OST_PartTradedQueueing:
        str = QString::fromLocal8Bit("部分成交");
        break;
    case THOST_FTDC_OST_Canceled:
        str = QString::fromLocal8Bit("已撤消");
        break;
    case THOST_FTDC_OST_NotTouched:
        str = QString::fromLocal8Bit("未触发");
        break;
    }
    return str;
}

// 条件类型文字转换
QString convertConditionMethod(TThostFtdcDirectionType flag)
{
    QString str = "";
    switch(flag)
    {
//	case THOST_FTDC_CC_Immediately:
//		str = QString::fromLocal8Bit(BCESConstCommentConditionMethodNone);
//		break;
//	case BCESConstConditionMethodEqual:
//		str = QString::fromLocal8Bit(BCESConstCommentConditionMethodEqual);
//		break;
//	case BCESConstConditionMethodMore:
//		str = QString::fromLocal8Bit(BCESConstCommentConditionMethodMore);
//		break;
//	case BCESConstConditionMethodMoreEqual:
//		str = QString::fromLocal8Bit(BCESConstCommentConditionMethodMoreEqual);
//		break;
//	case BCESConstConditionMethodLess:
//		str = QString::fromLocal8Bit(BCESConstCommentConditionMethodLess);
//		break;
//	case BCESConstConditionMethodLessEqual:
//		str = QString::fromLocal8Bit(BCESConstCommentConditionMethodLessEqual);
//		break;
    }
    return str;
}

int TradeWidget::getScale(QString InstrumentID)
{
    int pScale = 2;
    CThostFtdcInstrumentField * sbInstr = insMap[InstrumentID];
    if(!sbInstr)
        return pScale;
    if(sbInstr->PriceTick > 0.95)
        pScale = 0;
    else if(sbInstr->PriceTick > 0.095 && sbInstr->PriceTick < 0.95)
        pScale = 1;
    else if(sbInstr->PriceTick > 0.0095 && sbInstr->PriceTick < 0.095)
        pScale = 2;
    else if(sbInstr->PriceTick > 0.00095 && sbInstr->PriceTick < 0.0095)
        pScale = 3;
    return pScale;
}

TradeWidget::TradeWidget(QWidget *parent, Qt::WindowFlags flags)
    :QMainWindow(parent, flags)
{
    initTradeWin();
    CSubmit = NULL;
    omWidget = NULL;
    cmWidget = NULL;
    pview = NULL;
    //oview = NULL;
    dview = NULL;
    loginStatusLbl = new QLabel(this);
    this->ui.statusBar->addWidget(loginStatusLbl);
    //this->ui.statusBar->setStyleSheet("font-size:7px;");
    this->setMaximumWidth(320);
    // 关闭时自动释放
    setAttribute(Qt::WA_DeleteOnClose, true);
    setGeometry(DW/2-160,DH/2-250,320,500);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimerReqPos()));
}

void TradeWidget::closeEvent (QCloseEvent * event)
{
    Notice info(Notice::NOTICE_TYPE_WARNING, QString::fromLocal8Bit("退出TickTrader？"), false, QString::fromLocal8Bit("提示"), NULL, 0);
    info.exec();
    if(info.pushButton) {
        exit(0);
    }
    event->ignore();
}


void TradeWidget::doCancelOrder(CThostFtdcInputOrderActionField *pOrderCancelRsp)
{
    for(int i=0;i<o2upLst.length();i++)
    {
        NEWORDERINF & iti = o2upLst[i];
        if(::strcmp(pOrderCancelRsp->OrderRef, iti.OrderRef) == 0)
        {
            int nRequestID = CreateNewRequestID();
            CThostFtdcInputOrderField pInputOrder;
            ::memset(&pInputOrder,0,sizeof(CThostFtdcInputOrderField));
            strncpy(pInputOrder.BrokerID, loginW->m_users.BrokerID, sizeof(pInputOrder.BrokerID));
            strncpy(pInputOrder.InvestorID,iti.InvestorID, sizeof(pInputOrder.InvestorID)); /* 投资者号 */
            strncpy(pInputOrder.InstrumentID, iti.InstrumentID, sizeof(pInputOrder.InstrumentID));
            pInputOrder.Direction = iti.Direction; /* 买卖标志 */
            pInputOrder.CombOffsetFlag[0] = iti.CombOffsetFlag[0]; /* 开平标志 */
            pInputOrder.CombHedgeFlag[0] = THOST_FTDC_BHF_Speculation; /* 投机套保标志 */
            pInputOrder.TimeCondition = THOST_FTDC_TC_GFD;
            pInputOrder.VolumeCondition = THOST_FTDC_VC_AV;
            pInputOrder.MinVolume = 1;
            pInputOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
            pInputOrder.OrderPriceType = iti.PriceType;	/* 价格类型 */
            pInputOrder.LimitPrice = iti.Price;	/* 价格 */
            pInputOrder.TimeCondition = iti.ConditionMethod;	/* 条件方法 */
            pInputOrder.VolumeTotalOriginal = iti.Qty;	/* 数量 */
            strncpy(pInputOrder.ExchangeID, iti.ExchangeID, sizeof(pInputOrder.ExchangeID));
            tradeInfoLst[iti.tif].api->ReqOrderInsert(&pInputOrder, nRequestID); // 录入订单
            o2upLst.removeAt(i);
            break;
        }
    }
// delete pOrderCancelRsp;
}

// 撤单成功后检查待下的新单列表
void TradeWidget::checkCancelOrder(CThostFtdcInputOrderActionField *pOrderCancelRsp)
{
    CThostFtdcInputOrderActionField * cr = new CThostFtdcInputOrderActionField();
    if(pOrderCancelRsp)
        ::memcpy(cr, pOrderCancelRsp,sizeof(CThostFtdcInputOrderActionField));
    emit cancelOrder(cr);
}

void TradeWidget::tradingAftorLogin()
{
    loginW->hide();
    this->show();
    UserInfo product;
    strncpy(product.name,loginW->userName,sizeof(product.name));
    strncpy(product.pass,loginW->password,sizeof(product.pass));
//	strncpy(product.serv,loginW->server.name,sizeof(product.serv));
    runTrade(product);
    //QLabel * noticeLbl = new QLabel(QString::fromLocal8Bit(" 正在接收行情，请等待..."));
    //QFont font;
 //   font.setFamily(QString::fromUtf8("Cambria Math"));
 //   font.setPointSize(24);
 //   noticeLbl->setFont(font);
 //   noticeLbl->setAlignment(Qt::AlignCenter);
    //setCentralWidget(noticeLbl);

    CSubmit = new CoolSubmit(g_tw->insMap.begin().key(), NULL, g_tw);
    setCentralWidget(CSubmit);
    orderMessageEmit(loginW->userName + QString::fromLocal8Bit(" \r\n登录成功！"));
    loginStatusLbl->setText(QString::fromLocal8Bit("已登录"));
    a = clock();
}

void TradeWidget::runTrade(UserInfo & element)
{
    TradeInfo temp;
    temp.name = QString(element.name);
    TradeInfo & ti = tradeInfoLst[temp.name];
    if(ti.name != "" && ti.updated == false)
        return;

    temp.accountName= QString(element.name);
    temp.accountPass = QString(element.pass);
    temp.updated = false;

    temp.fund = new CThostFtdcTradingAccountField();
    temp.orderLst.clear();
    temp.tradeLst.clear();
    temp.posiLst.clear();
    ::memset(temp.fund,0,sizeof(CThostFtdcTradingAccountField));
    strncpy(temp.fund->AccountID,temp.accountName.toLatin1().data(),sizeof(temp.fund->AccountID));
    temp.api=pTraderApi;
    temp.spi=pTraderSpi;
    tradeInfoLst[temp.name] = temp;

}

// 更新状态栏
void TradeWidget::updateStatusBar(CThostFtdcTradingAccountField * fund)
{
    //double dqqy = fund->FreeBalance+fund->Margin+fund->FeeFrozen+fund->FrozenMargin+fund->FloatingPL; // 当前权益
    //QString lsb = QString::fromLocal8Bit("当前权益:");
    //lsb.append(QString::number(dqqy,'f',0));
    //lsb.append(QString::fromLocal8Bit("，可用资金:"));
    //lsb.append(QString::number(fund->FreeBalance,'f',0));
    //lsb.append(QString::fromLocal8Bit("，浮动盈亏:"));
    //lsb.append(QString::number(fund->FloatingPL,'f',0));
    //loginStatusLbl->setText(lsb);
}

// 交易账户连接成功
void TradeWidget::TradeConnect(CTradeSpiImp * t)
{
    emit tradeConnSec(t);
}

// 交易账户连接成功
void TradeWidget::doTradeConnSec(CTradeSpiImp * t)
{
    QMapIterator<QString, TradeInfo> i(tradeInfoLst);
    while (i.hasNext())
    {
        TradeInfo & ti= tradeInfoLst[i.next().key()];
        if(ti.spi == t)
        {
            CThostFtdcReqUserLoginField LogonReq;
            memset(&LogonReq,0x00,sizeof(LogonReq));
            QString userName =ti.accountName;
            QString pwd = ti.accountPass;
            strncpy(LogonReq.UserID,userName.toLatin1().data(),sizeof(LogonReq.UserID));
            strncpy(LogonReq.Password,pwd.toLatin1().data(),sizeof(LogonReq.Password));
            memcpy(LogonReq.UserProductInfo, "openctp", sizeof(LogonReq.UserProductInfo));
//			LogonReq.ProductVersion = PRODUCT_VERSION;
            ti.api->ReqUserLogin(&LogonReq,0);
            break;
        }
    }
}

// 行情推送
void TradeWidget::priceEmit(CThostFtdcDepthMarketDataField *p)
{
    CThostFtdcDepthMarketDataField * pe = new CThostFtdcDepthMarketDataField;
    ::memcpy(pe, p,sizeof(CThostFtdcDepthMarketDataField));
    emit getPpricePush(pe);
}

// 添加合约信息
void TradeWidget::instrEmit(CTradeSpiImp * t)
{
    emit getInstrPush(t);
}

// 添加持仓消息
void TradeWidget::posiEmit(CTradeSpiImp * t, CThostFtdcInvestorPositionField * pPosi, bool bLast)
{
    if(pPosi)
    {
        CThostFtdcInvestorPositionField * posi = new CThostFtdcInvestorPositionField;
        ::memcpy(posi, pPosi, sizeof(CThostFtdcInvestorPositionField));

        emit getPosiPush(t, posi, bLast);
    }
    else
        emit getPosiPush(t, pPosi, bLast);
}

// 添加账户资金消息
void TradeWidget::fundEmit(CTradeSpiImp * t, CThostFtdcTradingAccountField * pFund)
{
    CThostFtdcTradingAccountField * fund = new CThostFtdcTradingAccountField;
    ::memcpy(fund, pFund, sizeof(CThostFtdcTradingAccountField));
    emit getFundPush(t, fund);
}

// 添加订单消息
void TradeWidget::orderEmit(CTradeSpiImp * t, CThostFtdcOrderField * pOrder, bool bLast)
{
    if(pOrder){
        CThostFtdcOrderField * order = new CThostFtdcOrderField;
        ::memcpy(order, pOrder, sizeof(CThostFtdcOrderField));
        if(order->OrderStatus==THOST_FTDC_OST_Canceled)
        {
            CThostFtdcInputOrderActionField oRsp;
            strncpy(oRsp.OrderRef, order->OrderRef, sizeof(oRsp.OrderRef));
            checkCancelOrder(&oRsp);
        }
        emit getOrderPush(t, order, bLast);
    }
    else
        emit getOrderPush(t, pOrder, bLast);
}

// 添加成交消息
void TradeWidget::tradeEmit(CTradeSpiImp * t, CThostFtdcTradeField * pTrade, bool bLast)
{
    if(pTrade){
        CThostFtdcTradeField * trade = new CThostFtdcTradeField;
        ::memcpy(trade, pTrade, sizeof(CThostFtdcTradeField));
        emit getTradePush(t, trade, bLast);
    }
    else
        emit getTradePush(t, pTrade, bLast);
}

// 订单消息推送
void TradeWidget::orderMessageEmit(QString mes)
{
    QString mess = mes;
    emit orderMessage(mess);
}

// 添加行情记录
void TradeWidget::addPrice(CThostFtdcDepthMarketDataField *p)
{
    CThostFtdcDepthMarketDataField * sIn = quotMap[QString::fromLocal8Bit(p->InstrumentID)];
    CThostFtdcDepthMarketDataField * cIn = NULL;
    // 判断是否需要更新主力合约列表
    CThostFtdcInstrumentField * qins = insMap[QString::fromLocal8Bit(p->InstrumentID)];
    if(qins && QString(qins->ProductID) != "")
    {
        QMap<QString, CThostFtdcInstrumentField *>::const_iterator iter = insMap_zl.find(QString::fromLocal8Bit(qins->ProductID));
        CThostFtdcInstrumentField * zlins = iter != insMap_zl.end() ? iter.value():NULL;
        if(!zlins)
        {
            insMap_zl[QString::fromLocal8Bit(qins->ProductID)] = qins;
        }
        else
        {
            CThostFtdcDepthMarketDataField * qq = quotMap[QString::fromLocal8Bit(zlins->InstrumentID)];
            if(qq && qq->OpenInterest < p->OpenInterest)
                insMap_zl[QString::fromLocal8Bit(qins->ProductID)] = qins;
        }
    }
    if(!sIn)
    {
        quotMap[QString::fromLocal8Bit(p->InstrumentID)] = p;
        cIn = p;
        if(pTraderSpi->loginFlags && !CSubmit && cIn)
        {
            slot_showOrderWin(cIn->InstrumentID);
        }
        return;
    }
    else
    {
        cIn = sIn;
        //// 判断价格有无变化 没有的话直接return;
        //if(sIn->Price-p->Price+0.00001 < 0.0001)
        //{
        //	delete p;
        //	return;
        //}
        ::memcpy(sIn,p,sizeof(CThostFtdcDepthMarketDataField));
        delete p;
    }
    // 校验订单状态
    checkOrderStatus(sIn);
    checkFloatingPL(sIn);
    if(CSubmit && CSubmit->selectInstr)
    {
        if(!CSubmit->selectInstr->curInstr)
        {
            CSubmit->selectInstr->curInstr = qins;
            emit floating();
        }
        else if(strcmp(CSubmit->selectInstr->curInstr->InstrumentID , cIn->InstrumentID) == 0 || cmWidget || pview)
        {
            emit floating();
        }
    }
}

// 校验浮动盈亏
void TradeWidget::checkFloatingPL(CThostFtdcDepthMarketDataField * p)
{
    QMapIterator<QString, TradeInfo> ti(tradeInfoLst);
    while (ti.hasNext())
    {
        QString key = ti.next().key();
        TradeInfo & tti = tradeInfoLst[key];
        tti.fund->PositionProfit = 0;
        QMapIterator<QString, PosiPloy *> poi(tti.posiLst);
        while(poi.hasNext())
        {
            QString keyP = poi.next().key();
            PosiPloy * pPloy = tti.posiLst[keyP];
            QList<CThostFtdcInvestorPositionField *> uposi;
            if(pPloy)
                uposi = pPloy->posi;
            if(uposi.size() > 0)
            {
//				if(keyP == p->InstrumentID && p->LastPrice > 0)
//				{
//					double yk = insMap[QString::fromLocal8Bit(p->InstrumentID)]->TradeUnit*((p->LastPrice-uposi->BuyPrice)*uposi->BuyQty+(uposi->SellPrice - p->LastPrice)*uposi->SellQty);
//                    uposi->PositionProfit = yk;
//                    tti.fund->PositionProfit += yk;
//				}
//				else
                for(int index = 0; index < uposi.size(); index++)
                {
                    tti.fund->PositionProfit += uposi[index]->PositionProfit;
                }
            }
        }
        updateStatusBar(tti.fund);
    }
}

// 跟踪OCO状态
void TradeWidget::checkOcoStatus(TradeInfo & tf, TThostFtdcOrderRefType OrderRef)
{
    for(int oci=0;oci<ocoList.length();oci++)
    {
        OCOGROUP & ocog = ocoList[oci];
        if(::strcmp(ocog.OrderID1, OrderRef) == 0)
        {
            CThostFtdcOrderField * oco2 = tf.orderLst[ocog.OrderID2];
            if(!oco2) continue;
            oco2->OrderStatus = THOST_FTDC_OST_Canceled;
            ocoList.removeAt(oci);
            break;
        }
        if(::strcmp(ocog.OrderID2, OrderRef) == 0)
        {
            CThostFtdcOrderField * oco2 = tf.orderLst[ocog.OrderID1];
            if(!oco2) continue;
            oco2->OrderStatus = THOST_FTDC_OST_Canceled;
            ocoList.removeAt(oci);
            break;
        }
    }
}

void TradeWidget::checkOrderStatus(CThostFtdcDepthMarketDataField * cQuot)
{
    if(!cQuot) return;
    TradeInfo & tf = tradeInfoLst[loginW->userName];
    QMap<QString,PosiPloy *>::const_iterator iter = tf.posiLst.find(QString::fromLocal8Bit(cQuot->InstrumentID));
    PosiPloy * pploy = iter != tf.posiLst.end() ? iter.value():NULL;
    CThostFtdcInstrumentField * ci = insMap[QString::fromLocal8Bit(cQuot->InstrumentID)];
    if(!ci) return;
    double minMove = ci->PriceTick;
    if(!ci)  // 缺少合约信息
        return;
    QMapIterator<QString, CThostFtdcOrderField *> ot(tf.orderLst);
    while(ot.hasNext())
    {
        CThostFtdcOrderField * iti = tf.orderLst[ot.next().key()];
        if(!iti) //订单项为空
            continue;
        // 只处理未触发的订单
        if(iti->OrderStatus != THOST_FTDC_OST_NotTouched)
            continue;
        if(::strcmp(iti->InstrumentID, cQuot->InstrumentID) != 0)
            continue;
        bool sendFlag = false;
        double priceDis = cQuot->LastPrice - iti->LimitPrice;
        if(pploy && QString(iti->OrderRef).startsWith("ZZZS")) // 追踪止损
        {
            if(iti->Direction == THOST_FTDC_D_Sell && cQuot->LastPrice >= pploy->trailPriceu+0.00001)
            {
                pploy->trigFlag = true; // 多仓追踪止损触发
            }
            if(iti->Direction == THOST_FTDC_D_Buy && cQuot->LastPrice <= pploy->trailPriceu-0.00001)
            {
                pploy->trigFlag = true; // 空仓追踪止损触发
            }
            if(iti->Direction == THOST_FTDC_D_Sell && cQuot->LastPrice - iti->LimitPrice > pploy->points +0.00001 && pploy->trigFlag) // 多仓止损 当前价-止损价 变大
            {
                iti->LimitPrice = cQuot->LastPrice - pploy->points;
                pploy->trailPriceu = iti->LimitPrice + pploy->points;
            }
            if(iti->Direction == THOST_FTDC_D_Buy && iti->LimitPrice - cQuot->LastPrice > pploy->points - 0.00001 && pploy->trigFlag) // 空仓止损 止损价-当前价 变大
            {
                iti->LimitPrice = cQuot->LastPrice + pploy->points;
                pploy->trailPriceu = iti->LimitPrice-pploy->points;
            }
            if(iti->Direction == THOST_FTDC_D_Sell && pploy->trigFlag && cQuot->LastPrice <= iti->LimitPrice-0.00001)
            {
                sendFlag = true; // 多仓止损条件达到
            }
            if(iti->Direction == THOST_FTDC_D_Buy && pploy->trigFlag && cQuot->LastPrice >= iti->LimitPrice+0.00001)
            {
                sendFlag = true; // 空仓止损条件达到
            }
        }
        else
        {
            switch(iti->ContingentCondition)
            {
            case THOST_FTDC_CC_Immediately:
                {
                    if(priceDis < 0.00001 && priceDis > -0.00001)
                    {
                        sendFlag = true;
                    }
                }
                break;
            case THOST_FTDC_CC_LastPriceGreaterThanStopPrice:
                {
                    if(cQuot->LastPrice > iti->LimitPrice+minMove-0.00001)
                    {
                        sendFlag = true;
                    }
                }
                break;
            case THOST_FTDC_CC_LastPriceGreaterEqualStopPrice:
                {
                    if(cQuot->LastPrice > iti->LimitPrice-0.00001)
                    {
                        sendFlag = true;
                    }
                }
                break;
            case THOST_FTDC_CC_LastPriceLesserThanStopPrice:
                {
                    if(cQuot->LastPrice < iti->LimitPrice-minMove+0.00001)
                    {
                        sendFlag = true;
                    }
                }
                break;
            case THOST_FTDC_CC_LastPriceLesserEqualStopPrice:
                {
                    if(cQuot->LastPrice < iti->LimitPrice+0.00001)
                    {
                        sendFlag = true;
                    }
                }
                break;
            }
        }
        if(sendFlag)
        {
            CThostFtdcInputOrderField pInputOrder;
            ::memset(&pInputOrder,0,sizeof(CThostFtdcInputOrderField));
            strncpy(pInputOrder.BrokerID, loginW->m_users.BrokerID, sizeof(pInputOrder.BrokerID));
            strncpy(pInputOrder.InvestorID, iti->InvestorID,sizeof(pInputOrder.InvestorID)); /* 投资者号 */
            memcpy(pInputOrder.InstrumentID, iti->InstrumentID,sizeof(pInputOrder.InstrumentID));
            pInputOrder.Direction = iti->Direction; /* 买卖标志 */
            pInputOrder.CombOffsetFlag[0] = iti->CombOffsetFlag[0]; /* 开平标志 */
            pInputOrder.CombHedgeFlag[0] = THOST_FTDC_BHF_Speculation;
            pInputOrder.VolumeTotalOriginal = iti->VolumeTotalOriginal;	/* 数量 */
            strncpy(pInputOrder.OrderRef,iti->OrderRef,sizeof(pInputOrder.OrderRef));
            pInputOrder.ContingentCondition = THOST_FTDC_CC_Immediately; // 通道只接受‘5’
            strncpy(pInputOrder.ExchangeID, iti->ExchangeID,sizeof(pInputOrder.ExchangeID));// 交易所号
            if(pploy)
            {
                pploy->trigFlag = false;
                pploy->trailPriceu = 0;
                pploy->points = 0;
            }
            if(iti->Direction == THOST_FTDC_D_Sell)
            {
                pInputOrder.LimitPrice = cQuot->LowerLimitPrice;	/* 跌停价格 */
            }
            else
            {
                pInputOrder.LimitPrice = cQuot->UpperLimitPrice;	/* 涨停价格 */
            }
            pInputOrder.OrderPriceType = iti->OrderPriceType;
            pInputOrder.TimeCondition = THOST_FTDC_TC_GFD;
            pInputOrder.VolumeCondition = THOST_FTDC_VC_AV;
            pInputOrder.MinVolume = 1;
            pInputOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
//            strcpy( pInputOrder.MacAddress, "fe80::75ef:97de:2366:e490" );
//            strcpy( pInputOrder.IPAddress, "10.150.1.23" );
            iti->OrderStatus = THOST_FTDC_OST_Canceled;
            tf.api->ReqOrderInsert(&pInputOrder, 0); // 录入订单
            orderEmit(tf.spi, iti);
            checkOcoStatus(tf,iti->OrderRef);
        }
    }
}

// 添加合约信息
void TradeWidget::addInstr(CTradeSpiImp * t)
{
    disconnect(this, SIGNAL(getInstrPush(CTradeSpiImp *)),this, SLOT(addInstr(CTradeSpiImp *)));
    insMap = t->tempCons;
    QMapIterator<QString, CThostFtdcInstrumentField *> insItr(insMap);
    while(insItr.hasNext())
    {
        QString keyP = insItr.next().key();
        CThostFtdcInstrumentField * instr = insMap[keyP];
        // 自选合约
        // todo 与db配置做判断
        // 主力合约 行情接收后做判断
        // 上期所
        if(::strcmp(instr->ExchangeID, "SHFE") == 0)
        {
            insMap_sq[keyP] = instr;
        }
        // 郑商所
        if(::strcmp(instr->ExchangeID, "CZCE") == 0)
        {
            insMap_zs[keyP] = instr;
        }
        // 大商所
        if(::strcmp(instr->ExchangeID, "DCE") == 0)
        {
            insMap_ds[keyP] = instr;
        }
        // 中金所
        if(::strcmp(instr->ExchangeID, "CFFEX") == 0)
        {
            insMap_zj[keyP] = instr;
        }
        // 能源中心
        if(::strcmp(instr->ExchangeID, "INE") == 0)
        {
            insMap_ny[keyP] = instr;
        }
    }
    pQuotApi = CThostFtdcMdApi::CreateFtdcMdApi("md");
    quotSpi = new CMdSpiImp(pQuotApi);
    if( pQuotApi == NULL || quotSpi == NULL )
    {
        return;
    }
    pQuotApi->RegisterSpi(quotSpi);
    pQuotApi->RegisterFront((char*)loginW->m_listAddr.at(loginW->serverAddrIndex).marketServ.toStdString().c_str());
    pQuotApi->Init( );

    connect(this, SIGNAL(getPpricePush(CThostFtdcDepthMarketDataField *)),this, SLOT(addPrice(CThostFtdcDepthMarketDataField *)), Qt::QueuedConnection);
}

// 添加订单记录
void TradeWidget::addOrder(CTradeSpiImp * t, CThostFtdcOrderField * order, bool bLast)
{
    if(bLast)
    {
        for( ; ; )
        {
            QThread::msleep(1000);
            int nRequestIDs = CreateNewRequestID();
            CThostFtdcQryTradeField reqInfo = {0};
            int ret = pTraderApi->ReqQryTrade(&reqInfo, nRequestIDs);
            qInfo() << "ReqQryTrade: " << ret;
            if(ret == -2 || ret == -3)
                QThread::msleep(100);
            else
                break;
        }
    }
    if(!order)return;
    //mutex.lock();
    CThostFtdcOrderField & pOrder = *order;
    QMapIterator<QString, TradeInfo> i(tradeInfoLst);
    int scale = getScale(pOrder.InstrumentID);
    while (i.hasNext())
    {
        TradeInfo & ti = tradeInfoLst[i.next().key()];
        if(ti.spi == t)
        {
            CThostFtdcOrderField * olt = ti.orderLst[QString::fromLocal8Bit(pOrder.OrderRef)];
            if(!olt)
            {
                if(pOrder.InsertTime[0] == 0)
                {
                    QTime t = QTime::currentTime();
                    QDate dt = QDate::currentDate();
                    sprintf(pOrder.InsertDate,"%04d%02d%02d", dt.year(), dt.month(),dt.day());
                    sprintf(pOrder.InsertTime,"%02d:%02d:%02d", t.hour(), t.minute(),t.second());
                }
                ti.orderLst[QString::fromLocal8Bit(pOrder.OrderRef)] = order;
            }
            else
            {
                ::memcpy(olt,order,sizeof(CThostFtdcOrderField));
                delete order;
            }
            break;
        }
    }
    //mutex.unlock();
    if(omWidget)
    {
        omWidget->update();
    }
    if(CSubmit)
    {
        CSubmit->update();
    }
}

// 添加成交记录
void TradeWidget::addTrade(CTradeSpiImp * t, CThostFtdcTradeField  * trade, bool bLast)
{
    if(bLast)
    {
        for( ; ; )
        {
            QThread::msleep(1000);
            int nRequestIDs = CreateNewRequestID();
            CThostFtdcQryInvestorPositionField reqInfo = {0};
            int ret = pTraderApi->ReqQryInvestorPosition(&reqInfo, nRequestIDs);
            qInfo() << "ReqQryInvestorPosition: " << ret;
            if(ret == -2 || ret == -3)
                QThread::msleep(100);
            else
                break;
        }
    }
    if(!trade) return;
    //mutex.lock();
    CThostFtdcTradeField & pTrade = *trade;
    QMapIterator<QString, TradeInfo> i(tradeInfoLst);
    int scale = getScale(pTrade.InstrumentID);
    QString MOid = QString::fromLocal8Bit(pTrade.TradeID).append("_").append(pTrade.OrderRef);
    while (i.hasNext())
    {
        TradeInfo & ti = tradeInfoLst[i.next().key()];
        if(ti.spi == t)
        {
            CThostFtdcTradeField * olt = ti.tradeLst[MOid];
            if(!olt)
            {
                ti.tradeLst[MOid] = trade;
            }
            else
            {
                ::memcpy(olt,olt,sizeof(CThostFtdcTradeField));
                delete trade;
            }
            break;
        }
    }
}

void TradeWidget::onTimerReqPos()
{
    for( ; ; )
    {
        QThread::msleep(1000);
        int nRequestIDs = CreateNewRequestID();
        CThostFtdcQryInvestorPositionField reqInfo = {0};
        int ret = pTraderApi->ReqQryInvestorPosition(&reqInfo, nRequestIDs);
        qInfo() << "ReqQryInvestorPosition: " << ret;
        if(ret == -2 || ret == -3)
            QThread::msleep(100);
        else
            break;
    }
}

// 添加账户资金记录
void TradeWidget::addFunds(CTradeSpiImp * t, CThostFtdcTradingAccountField * fund)
{
    //mutex.lock();
    CThostFtdcTradingAccountField pFund = *fund;
    bool updateFlag = false;
    QMapIterator<QString, TradeInfo> i(tradeInfoLst);
    while (i.hasNext())
    {
        TradeInfo & ti = tradeInfoLst[i.next().key()];
        if(ti.spi == t)
        {
            ::memcpy(ti.fund, &pFund,sizeof(CThostFtdcTradingAccountField));
            updateFlag = true;
            // 需重新清算该资金账户的浮动盈亏
            ti.fund->PositionProfit = 0;
            QMapIterator<QString, PosiPloy *> poi(ti.posiLst);
            while(poi.hasNext())
            {
                QString keyP = poi.next().key();
                QList<CThostFtdcInvestorPositionField *> uposi;
                if(ti.posiLst[keyP])
                    uposi = ti.posiLst[keyP]->posi;
//                if(uposi.size() != 0 && ::memcmp(uposi->InvestorID,fund->AccountID,sizeof(TThostFtdcInvestorIDType)) == 0)
                if(uposi.size() > 0)
                {
                    for(int index = 0; index < uposi.size(); index++)
                        ti.fund->PositionProfit += uposi[index]->PositionProfit;
                }
            }
            updateStatusBar(ti.fund);
            break;
        }
    }
    delete fund;

//    m_timer.start(5000);
}


void TradeWidget::addPosi(CTradeSpiImp * t, CThostFtdcInvestorPositionField * posi, bool bLast)
{
    if(bLast)
    {
        for( ; ; )
        {
            QThread::msleep(1000);
            int nRequestIDs = CreateNewRequestID();
            CThostFtdcQryTradingAccountField reqInfo = {0};
            int ret = pTraderApi->ReqQryTradingAccount(&reqInfo, nRequestIDs);
            qInfo() << "ReqQryTradingAccount: " << ret;
            if(ret == -2 || ret == -3)
                QThread::msleep(100);
            else
                break;
        }
    }
    //mutex.lock();
    if(!posi)
        return;
    CThostFtdcInvestorPositionField & pPosi = *posi;
    // 绘制前先订阅
    int count = 1;
    char **InstrumentID = new char*[count];
    InstrumentID[0] = posi->InstrumentID;
    pQuotApi->SubscribeMarketData(InstrumentID, 1);
    insMap_dy[QString::fromLocal8Bit(posi->InstrumentID)] = insMap[QString::fromLocal8Bit(posi->InstrumentID)];
    delete[] InstrumentID;
    int scale = getScale(pPosi.InstrumentID);
    QMapIterator<QString, TradeInfo> i(tradeInfoLst);
    while (i.hasNext())
    {
        TradeInfo & ti = tradeInfoLst[i.next().key()];
        if(ti.spi == t)
        {
            PosiPloy * oltp = ti.posiLst[posi->InstrumentID];
            if(!oltp)
            {
                PosiPloy * pp = new PosiPloy;
                pp->trailPriceu = 0;
                pp->posi.append(posi);
                pp->points = 0;
                ti.posiLst[posi->InstrumentID] = pp;
//                CThostFtdcDepthMarketDataField * p = quotMap[QString::fromLocal8Bit(posi->InstrumentID)];
//				if(p && p->Price > 0)
//				{
//					double yk = insMap[QString::fromLocal8Bit(posi->InstrumentID)]->TradeUnit*((p->Price-posi->BuyPrice)*posi->BuyQty+(posi->SellPrice - p->Price)*posi->SellQty);
//					posi->FloatingPL = yk;
//				}
            }
            else
            {
                for(int index = 0; index<oltp->posi.size(); index++)
                {
                    CThostFtdcInvestorPositionField *pTemp = oltp->posi.at(0);
                    if(pTemp->PosiDirection == posi->PosiDirection)
                        oltp->posi.removeAt(index);
                }
                oltp->posi.append(posi);
//                ::memcpy(oltp->posi, posi, sizeof(CThostFtdcInvestorPositionField));
//                CThostFtdcDepthMarketDataField * p = quotMap[QString::fromLocal8Bit(posi->InstrumentID)];
//				if(p && p->Price > 0)
//				{
//					double yk = insMap[QString::fromLocal8Bit(posi->InstrumentID)]->TradeUnit*((p->Price-posi->BuyPrice)*posi->BuyQty+(posi->SellPrice - p->Price)*posi->SellQty);
//					oltp->posi->FloatingPL = yk;
//				}
//				delete posi;
            }
            break;
        }
    }
    //mutex.unlock();
    if(cmWidget && bLast)
    {
        cmWidget->update();
    }
}

void TradeWidget::comuWithTradeImp(TRADE_SIGNAL sig)
{
    msgLabel->setText("");
    switch(sig)
    {
    case LOGIN:
        loginW->show();
        break;
    case LOGOUT:
        loginW->show();
        break;
    default:
        break;
    }
}

// 异常信息处理
void TradeWidget::errManage(int code, QString mess)
{
    //关闭定时器
    // timeLimiter->stop();
    switch(code)
    {
        case 1005:
        {
            //QMessageBox::StandardButton button;
            //button = QMessageBox::question(this, QString::fromLocal8Bit("提示"), mess.append(QString::fromLocal8Bit("， 是否重新登录？")), QMessageBox::Yes | QMessageBox::No);
            //if (button == QMessageBox::No) {
            //	exit(0);
            //}
            //else if (button == QMessageBox::Yes) {
            //	loginW->show();
            //}
            Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, mess, false, QString::fromLocal8Bit("提示"), NULL, 0);
            nt.exec();
            loginW->show();
            break;
        }
        case 1003:
            {
                //QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), mess);
                Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, mess, false, QString::fromLocal8Bit("提示"), NULL, 0);
                nt.exec();
                exit(0);
                break;
            }
        default:
            {
                //QMessageBox::about(NULL, QString::fromLocal8Bit("提示"), mess);
                Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, mess, false, QString::fromLocal8Bit("提示"), NULL, 0);
                nt.exec();
                break;
            }
    }
}

void TradeWidget::initTradeWin()
{
    ui.setupUi(this);
    // 退出程序
    connect(ui.actionlogOut, SIGNAL(triggered(bool)), this, SLOT(close()), Qt::QueuedConnection);
    // 订单管理
    connect(ui.actionOrder, SIGNAL(triggered(bool)), this, SLOT(orderManage()), Qt::QueuedConnection);
    // 持仓管理
    connect(ui.actionCM, SIGNAL(triggered(bool)), this, SLOT(posiManage()), Qt::QueuedConnection);
    // 报价板
    connect(ui.priceAction, SIGNAL(triggered(bool)), this, SLOT(showPriceW()), Qt::QueuedConnection);
    // 传统下单界面
    /*connect(ui.tradeAction, SIGNAL(triggered(bool)), this, SLOT(showTradeW()), Qt::QueuedConnection);*/
    // 设置
    connect(ui.configAction, SIGNAL(triggered(bool)), this, SLOT(showConfW()), Qt::QueuedConnection);
    // 修改密码界面
    connect(ui.passwordAction, SIGNAL(triggered(bool)), this, SLOT(showChangePassword()), Qt::QueuedConnection);
    // 成交单
    connect(ui.dealAction, SIGNAL(triggered(bool)), this, SLOT(showDealW()), Qt::QueuedConnection);
    // 帮助
    connect(ui.helpAction, SIGNAL(triggered(bool)), this, SLOT(help()), Qt::QueuedConnection);
    // K线图
//	connect(ui.actionK, SIGNAL(triggered(bool)),this, SLOT(showCurve()), Qt::QueuedConnection);

    connect(loginW,SIGNAL(loginSec()),this, SLOT(tradingAftorLogin()), Qt::QueuedConnection);
    connect(loginW, SIGNAL(pushTradeToFrom(TRADE_SIGNAL)),this, SLOT(comuWithTradeImp(TRADE_SIGNAL)), Qt::QueuedConnection);
    connect(loginW, SIGNAL(errShow(int, QString)),this, SLOT(errManage(int, QString)), Qt::QueuedConnection);
    // API 信号槽
    connect(this, SIGNAL(getOrderPush(CTradeSpiImp *, CThostFtdcOrderField *, bool)),this, SLOT(addOrder(CTradeSpiImp *, CThostFtdcOrderField *, bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(getPosiPush(CTradeSpiImp *, CThostFtdcInvestorPositionField *, bool)),this, SLOT(addPosi(CTradeSpiImp *, CThostFtdcInvestorPositionField *, bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(getFundPush(CTradeSpiImp *, CThostFtdcTradingAccountField *)),this, SLOT(addFunds(CTradeSpiImp *, CThostFtdcTradingAccountField *)), Qt::QueuedConnection);
    connect(this, SIGNAL(getTradePush(CTradeSpiImp *, CThostFtdcTradeField *, bool)),this, SLOT(addTrade(CTradeSpiImp *, CThostFtdcTradeField *, bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(getInstrPush(CTradeSpiImp *)),this, SLOT(addInstr(CTradeSpiImp *)), Qt::QueuedConnection);
    connect(this, SIGNAL(cancelOrder(CThostFtdcInputOrderActionField *)),this, SLOT(doCancelOrder(CThostFtdcInputOrderActionField *)), Qt::QueuedConnection);
    connect(this, SIGNAL(tradeConnSec(CTradeSpiImp *)),this, SLOT(doTradeConnSec(CTradeSpiImp *)), Qt::QueuedConnection);

    // 订单消息处理
    connect(this, SIGNAL(orderMessage(QString)),this, SLOT(messageSrv(QString)), Qt::QueuedConnection);
}

TradeWidget::~TradeWidget()
{
    // 切断API推送
    disconnect(this, SIGNAL(getOrderPush(CTradeSpiImp *, CThostFtdcOrderField *)),this, SLOT(addOrder(CTradeSpiImp *, CThostFtdcOrderField *)));
    disconnect(this, SIGNAL(getPosiPush(CTradeSpiImp *, CThostFtdcInvestorPositionField *)),this, SLOT(addPosi(CTradeSpiImp *, CThostFtdcInvestorPositionField *)));
    disconnect(this, SIGNAL(getFundPush(CTradeSpiImp *, CThostFtdcTradingAccountField *)),this, SLOT(addFunds(CTradeSpiImp *, CThostFtdcTradingAccountField *)));
    disconnect(this, SIGNAL(getTradePush(CTradeSpiImp *, CThostFtdcTradeField *)),this, SLOT(addTrade(CTradeSpiImp *, CThostFtdcTradeField *)));
    disconnect(this, SIGNAL(getInstrPush(CTradeSpiImp *)),this, SLOT(addInstr(CTradeSpiImp *)));
    disconnect(this, SIGNAL(getPpricePush(CThostFtdcDepthMarketDataField *)),this, SLOT(addPrice(CThostFtdcDepthMarketDataField *)));
    disconnect(this, SIGNAL(cancelOrder(CThostFtdcInputOrderActionField *)),this, SLOT(doCancelOrder(CThostFtdcInputOrderActionField *)));
    disconnect(this, SIGNAL(tradeConnSec(CTradeSpiImp *)),this, SLOT(doTradeConnSec(CTradeSpiImp *)));

    exit(0);
}

void TradeWidget::slot_showOrderWin(TThostFtdcInstrumentIDType m_strInstr)
{
    if(!pTraderApi)
    {
        //QMessageBox::about(NULL,"提示","未登录，请先登录！");
        Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("未登录，请先登录！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
        nt.exec();
        return;
    }
    QString m_strInstrument = QString(m_strInstr);
    CThostFtdcDepthMarketDataField * t = quotMap[m_strInstrument];
    if(!t)
    {
        //QMessageBox::about(NULL,"提示","缺少行情，暂停下单！");
        Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("缺少行情，暂停下单！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
        nt.exec();
        return;
    }
}

// 订单消息处理
void TradeWidget::messageSrv(QString mes)
{
    //int lins = 11;
    //while(lins < mes.length())
    //{
    //	mes = mes.insert(lins,"\r\n");
    //	lins += 13;
    //}
    QWidget * messWin = new QWidget();
    messWin->setWindowFlags(Qt::Popup);
    messWin->setWindowFlags(Qt::FramelessWindowHint);
    messWin->setAttribute(Qt::WA_TranslucentBackground);
    QLabel * backL = new QLabel(messWin);
    backL->setPixmap(QPixmap(":/image/images/notice.png"));
    QLabel * mesL = new QLabel(messWin);
    mesL->setText(mes);
    const QRect wR = QApplication::desktop()->screenGeometry();
    messWin->setGeometry(wR.right()-140,wR.bottom()-140,120,120);
    mesL->setGeometry(0,0,messWin->width(),messWin->height()-20);
    backL->setGeometry(0,0,messWin->width(),messWin->height());
    mesL->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    messWin->show();
    QTimer::singleShot(3000, messWin, SLOT(close()));
}

void TradeWidget::orderManage()
{
    // QMessageBox::about(NULL,"提示","建设中！");
    if(!omWidget)
        omWidget = new OrderManage(this);
    omWidget->setGeometry(DW/2-215,DH/2-125,430,250);
    omWidget->show();
//#ifdef WIN32
//    ::SetWindowPos(HWND(omWidget->winId()), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//#endif
}

// 持仓管理
void TradeWidget::posiManage()
{
    // QMessageBox::about(NULL,"提示","建设中！");
    if(!cmWidget)
        cmWidget = new PosiManage(this);
    cmWidget->setGeometry(DW/2-215,DH/2-125,430,250);
    cmWidget->show();
//#ifdef WIN32
//	::SetWindowPos(HWND(cmWidget->winId()), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//#endif
}

// 报价版展示
void TradeWidget::showPriceW()
{
    // QMessageBox::about(NULL,"提示","报价板建设中！");
    if(quotMap.count() == 0)
    {
        // 数量不能为0
        Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("暂缺行情！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
        nt.exec();
        return;
    }
    if(!pview)
        pview = new PriceView(this);
    pview->setGeometry(DW/2-430/2,DH/2-125,430,250);
    pview->show();
//#ifdef WIN32
//	::SetWindowPos(HWND(pview->winId()), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//#endif
}


// 打开传统下单界面
//void TradeWidget::showTradeW()
//{
//	if(this->CSubmit)
//		showTradeW(this->CSubmit->selectInstr->curInstr);
//}

// 传统下单界面展示
//void TradeWidget::showTradeW(CThostFtdcInstrumentField *s)
//{
//	//QMessageBox::about(NULL,"提示","传统下单界面建设中！");
//	if(quotMap[s->InstrumentID] == 0)
//	{
//		return;
//	}
//	if(!oview)
//	oview = new OrderView(this);
//	oview->selectInstr->curInstr = s;
//	oview->setGeometry(DW/2-180,DH/2-85,360,170);
//	oview->show();
//	::SetWindowPos(HWND(oview->winId()), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//}

// 打开设置界面
void TradeWidget::showConfW()
{
    if(!cview)
    cview = new configWidget(this);
    cview->setGeometry(DW/2-165,DH/2-100,330,200);
    cview->initByPrivats();
    cview->show();
//#ifdef WIN32
//	::SetWindowPos(HWND(cview->winId()), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//#endif
}

// 打开修改密码界面
void TradeWidget::showChangePassword()
{
    if(!cpView)
    cpView = new changePassword(NULL);
    cpView->setGeometry(DW/2-165,DH/2-100,330,200);
    cpView->show();
//#ifdef WIN32
//	::SetWindowPos(HWND(cpView->winId()), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//#endif
}

// 成交单界面
void TradeWidget::showDealW() {
    if (!dview)
        dview = new dealView(this);
    dview->setGeometry(DW/2-180,DH/2-125,360,250);
    dview->show();
//#ifdef WIN32
//    ::SetWindowPos(HWND(dview->winId()), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//#endif
}

// 帮助
void TradeWidget::help()
{
    Help * hp = new Help(this);
    hp->setGeometry(DW/2-200,DH/2-124,400,248);
    hp->show();
}

// 展示K线图
void TradeWidget::showCurve()
{
    if(!CSubmit)
    {
        Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("主窗口构建中！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
        nt.exec();
        return;
    }
    CThostFtdcInstrumentField * ci = CSubmit->selectInstr->curInstr;
//	if(!ci || !pHisApi)
    if(!ci)
    {
        Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("主窗口构建中！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
        nt.exec();
        return;
    }
//	CurveUnit * CUnit = new CurveUnit(ci);
//	CUnit->setGeometry(DW/2-215,DH/2-125,430,250);
//	CUnit->setActionm5();
//	CUnitList.append(CUnit);
//	CUnit->show();
}
