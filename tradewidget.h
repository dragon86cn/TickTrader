#ifndef TRADEWIDGET_H
#define TRADEWIDGET_H

#include <QMainWindow>
#include <QMap>
#include <QTimer>
#include "ui_tradewidget.h"
#include <QDesktopServices>
#include <QDesktopWidget>
#include "loginWin.h"
#include <QMutex>
#include "notice.h"

#define PRODUCT_VERSION 3

#define DW QApplication::desktop()->screenGeometry().width()
#define DH QApplication::desktop()->screenGeometry().height()

typedef struct
{
    QList<CThostFtdcInvestorPositionField *> posi;
    double points;      // 回调点数
    double trailPriceu; // 触发价位
    bool trigFlag;      // 触发标志
}PosiPloy;

typedef struct
{
    QString name;        // 账户名称
    QString accountName; // 登录名
    QString accountPass; // 登录密码
    QString routeName;   // 通道名
    bool  updated;       // 状态变更标志
    CThostFtdcTraderApi* api;  // API对象
    CTradeSpiImp *spi; // 回调对象
    CThostFtdcTradingAccountField * fund;       // 该账户对应的资金
    QMap<QString,PosiPloy *> posiLst;// 该账户对应的持仓信息 KEY:合约号
    QMap<QString,CThostFtdcTradeField *> tradeLst;// 该账户对应的成交信息 KEY:成交号
    QMap<QString, CThostFtdcOrderField *> orderLst;// 该账户对应的订单信息 KEY:订单号
}TradeInfo;

typedef struct {
    char tif[32];                // 账户名
    TThostFtdcOrderRefType OrderRef;	/* 订单号 */
    TThostFtdcInvestorIDType InvestorID;	/* 投资者号 */
    TThostFtdcInstrumentIDType InstrumentID;	/* 合约号 */
    TThostFtdcDirectionType Direction;	/* 买卖标志 */
    TThostFtdcCombOffsetFlagType CombOffsetFlag;	/* 开平标志 */
    TThostFtdcOrderPriceTypeType PriceType;	/* 价格类型 */
    TThostFtdcPriceType Price;	/* 价格 */
    TThostFtdcVolumeType Qty;	/* 数量 */
    TThostFtdcContingentConditionType ConditionMethod;	/* 条件方法 */
    TThostFtdcExchangeIDType ExchangeID;	/* 交易所号 */
}NEWORDERINF;

typedef struct {
    TThostFtdcOrderRefType OrderID1;	/* 订单号1 */
    TThostFtdcOrderRefType OrderID2;	/* 订单号2 */
}OCOGROUP; //oco组合

class CoolSubmit;
class OrderManage;
class PosiManage;
class PriceView;
//class OrderView;
class configWidget;
class changePassword;
class dealView;
class TradeWidget : public QMainWindow
{
    Q_OBJECT

public:
    TradeWidget(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());
    ~TradeWidget();
    // status 用
    QLabel * loginStatusLbl;
    // 合约列表
    QMap<QString, CThostFtdcInstrumentField *> insMap;
    // // 自选合约
    QMap<QString, CThostFtdcInstrumentField *> insMap_zx;
    // 主力合约
    QMap<QString, CThostFtdcInstrumentField *> insMap_zl;
    // 能源中心
    QMap<QString, CThostFtdcInstrumentField *> insMap_ny;
    // 上期所
    QMap<QString, CThostFtdcInstrumentField *> insMap_sq;
    // 郑商所
    QMap<QString, CThostFtdcInstrumentField *> insMap_zs;
    // 大商所
    QMap<QString, CThostFtdcInstrumentField *> insMap_ds;
    // 中金所
    QMap<QString, CThostFtdcInstrumentField *> insMap_zj;
    // 订阅的合约
    QMap<QString, CThostFtdcInstrumentField *> insMap_dy;
    // 获取合约的报价精度
    int getScale(QString InstrumentID);
    // 行情MAP
    QMap<QString, CThostFtdcDepthMarketDataField *> quotMap;
    // 待修改的订单列表
    QList<NEWORDERINF> o2upLst;
    // oco组合列表
    QList<OCOGROUP> ocoList;
    // 撤单成功后检查待下的新单列表
    void checkCancelOrder(CThostFtdcInputOrderActionField *pOrderCancelRsp);
    // 行情推送
    void priceEmit(CThostFtdcDepthMarketDataField *p);
    // 添加合约信息
    void instrEmit(CTradeSpiImp * t);
    // 添加持仓消息
    void posiEmit(CTradeSpiImp * t, CThostFtdcInvestorPositionField * pPosi, bool bLast = false);
    // 添加账户资金消息
    void fundEmit(CTradeSpiImp * t, CThostFtdcTradingAccountField * pFund);
    // 添加订单消息
    void orderEmit(CTradeSpiImp * t, CThostFtdcOrderField * pOrder, bool bLast = false);
    // 添加成交消息
    void tradeEmit(CTradeSpiImp * t, CThostFtdcTradeField * pTrade, bool bLast = false);
    // 订单消息推送
    void orderMessageEmit(QString mes);
    // 订单管理界面
    OrderManage * omWidget;
    // 持仓管理界面
    PosiManage * cmWidget;
    // 报价版窗口
    PriceView * pview;
    // 传统下单窗口
    //OrderView * oview;
    // 成交单窗口
    dealView * dview;
    // 交易账户连接成功
    void TradeConnect(CTradeSpiImp * t);
    Ui::TradeWidgetClass ui;
    // 快捷交易界面
    CoolSubmit * CSubmit;
    // 运行通道
    void runTrade(UserInfo & element);
    // 更新状态栏
    void updateStatusBar(CThostFtdcTradingAccountField * fund);
    // 打开传统下单界面
    //void showTradeW(CThostFtdcInstrumentField *s);
    // 跟踪OCO状态
    void checkOcoStatus(TradeInfo & tf, TThostFtdcOrderRefType OrderRef);
    // 校验订单状态
    void checkOrderStatus(CThostFtdcDepthMarketDataField * sq);
    // 校验浮动盈亏
    void checkFloatingPL(CThostFtdcDepthMarketDataField * sq);
protected:
    void closeEvent (QCloseEvent * event);
private:
    // 状态栏提示信息
    QLabel * msgLabel;

    QTimer m_timer;
public slots:
    void onTimerReqPos();
    void slot_showOrderWin(TThostFtdcInstrumentIDType m_strInstr);
    void tradingAftorLogin();
    // 构建交易窗口
    void initTradeWin();
    // 异常信息处理
    void errManage(int code, QString mess);
    // 弹出登录窗口
    void comuWithTradeImp(TRADE_SIGNAL sig);
    // 添加行情记录
    void addPrice(CThostFtdcDepthMarketDataField *p);
    // 收到合约信息提送
    void addInstr(CTradeSpiImp * t);
    // 添加订单记录
    void addOrder(CTradeSpiImp * t, CThostFtdcOrderField * pOrder, bool bLast);
    // 添加持仓记录
    void addPosi(CTradeSpiImp * t, CThostFtdcInvestorPositionField * pPosi, bool bLast);
    // 添加账户资金记录
    void addFunds(CTradeSpiImp * t, CThostFtdcTradingAccountField * pFund);
    // 添加成交记录
    void addTrade(CTradeSpiImp * t, CThostFtdcTradeField * pTrade, bool bLast);
    // 撤单成功推送
    void doCancelOrder(CThostFtdcInputOrderActionField *pOrderCancelRsp);
    // 交易账户连接成功
    void doTradeConnSec(CTradeSpiImp * t);
    // 订单消息处理
    void messageSrv(QString mes = "");
    // 订单管理
    void orderManage();
    // 持仓管理
    void posiManage();
    // 报价版展示
    void showPriceW();
    // 打开设置界面
    void showConfW();
    // 打开传统下单界面
    /*void showTradeW();*/
    // 打开成交单界面
    void showDealW();
    // 打开修改密码界面
    void showChangePassword();
    // 帮助
    void help();
    // 展示K线图
    void showCurve();
signals:
    // 订单删除了
    void orderDroped();
    // 行情推送
    void getPpricePush(CThostFtdcDepthMarketDataField *p);
    // 交易账户登录成功
    void tradeConnSec(CTradeSpiImp * t);
    // 收到合约信息提送
    void getInstrPush(CTradeSpiImp * t);
    // 收到订单提送
    void getOrderPush(CTradeSpiImp * t, CThostFtdcOrderField * order, bool bLast);
    // 收到持仓提送
    void getPosiPush(CTradeSpiImp * t,CThostFtdcInvestorPositionField * pPosi, bool bLast);
    // 收到资金推送
    void getFundPush(CTradeSpiImp * t,CThostFtdcTradingAccountField * pFund);
    // 收到成交推送
    void getTradePush(CTradeSpiImp * t,CThostFtdcTradeField * pTrade, bool bLast);
    // 撤单成功推送
    void cancelOrder(CThostFtdcInputOrderActionField *pOrderCancelRsp);
    // 订单处理rsp消息
    void orderMessage(QString mes);
    // 盈亏变化
    void floating();
};

// 行情数据接口
extern CMdSpiImp * quotSpi;
// 行情接口框架
extern CThostFtdcMdApi* pQuotApi;
// 登录接口框架
extern CThostFtdcTraderApi* pTraderApi;
// 登录回调接口
extern CTradeSpiImp *pTraderSpi;

// 账户map
extern QMap<QString, TradeInfo> tradeInfoLst;

#endif // TRADEWIDGET_H
