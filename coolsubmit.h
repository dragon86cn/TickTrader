#ifndef COOLSUBMIT_H
#define COOLSUBMIT_H
#include <QWidget>
#include <QtGui/QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QWheelEvent>
#include <QMouseEvent>
#include "tradewidget.h"
#include "InstrManage.h"

#define LIMITORDER "限价单"
#define STOPMARKETORDER "止损市价单"
#define ONETICK "+1TICK限价单"
#define TWOTICK "+2TICK限价单"
#define THREETICK "+3TICK限价单"
#define STOPLIMITORDER "止损限价单"

typedef struct
{
    TThostFtdcInstrumentIDType InsID;	/* 合约号 */
    TThostFtdcInvestorIDType invID; // 投资者号
	double price; // 价格
	int account;  // 数量
	char bs;      // 买卖方向
}StopParam;

class InstrManage;
class CoolSubmit: public QWidget
{
	Q_OBJECT
public:
    CoolSubmit(QString Instr, CThostFtdcDepthMarketDataField * t, TradeWidget * tWidget, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~CoolSubmit();
	// 滚轮向上
	void wheelup();
	// 滚轮向下
	void wheeldown();
	// 交易窗口
	TradeWidget * tw;
	// 最初价格
    CThostFtdcDepthMarketDataField * sQuot;
	// 订单索引
    QMap<QString, CThostFtdcOrderField *> orderMap;
	// 当前资金账户
	QString currentAccout;
	// 记录提示信息
	QString messageInfo;
	// 用于设定手数
	QLineEdit * numL;
	// 设置订单模式
	// QComboBox * orderModeC; 
	// 选择交易品种
	InstrManage * selectInstr; 
	// 右键菜单 - 止损限价
	QAction *limitAction;
	// 连续输入标志 0 未输入 1输入中 2 搜合约
	int continueFlag;
	// 价格精度
	int pScale;
	// 最小变动价位
	double minMove;
	// 卖价位数量
	QMap<int, int> sellOrder; 
	// 买价位数量
	QMap<int, int> buyOrder; 
	// 单击效果用
	int clkborderWidth;
	// 条件方法
    TThostFtdcDirectionType methodFlag;
	// 设置wheelSteps
	void setWheelSteps(int steps){wheelSteps=steps;};
	// 删除订单
	void dropOrder(char bos, double price);
	// 修改订单
	void updateOrder(char bos, double price, double price2);
	// 新建订单
	void insertOrder(char bos, double price, int fpoints = 0, int qty = 0);
	// 新建止损订单
	void insertLOrder(char bos, double price, int fpoints = 0, int qty = 0);
	// 删所有买订单
	void dropAll(char bos);
private:
	// 一页总行数
	int linNum;
	// 记录滚轮的位置
	int wheelSteps;
	// 最大滚轮位置
	int maxWheelSteps;
	// 最小滚轮位置
	int minWheelSteps;
	// 记录鼠标拖动的宽度起点
	int moveWPixs;
	// 记录鼠标拖动的高度起点
	int moveHPixs;
	// 每行数据的像素高度
	int linPix;
	// 现价区域
	QRect curPriceR;
	// 叫买价区域
	QRect BidPriceR;
	// 叫卖价区域
	QRect AskPriceR;

	// 品种选择区域
	QRect selectInstrR;
	// 提交数量区域
	QRect submitCountsR;
	// 提交可以下卖单区域
	QRect submitSR;
	// 提交可以下买单区域
	QRect submitBR;
	//// 盈利显示区域
	//QRect ProfitR;
	// 镜像区域
	QRect scrollR;
	// 滚动条bar
	QRect sBarR;
	// 数量1区域
	QRect rcount1;
	// 数量2区域
	QRect rcount2;
	// 数量3区域
	QRect rcount3;
	// 数量5区域
	QRect rcount5;
	// 数量10区域
	QRect rcount10;
	// 数量20区域
	QRect rcount20;
	// 撤所有买单区域
	QRect cAllBuy;
	// 撤所有卖单区域
	QRect cAllSell;
	// 撤所有区域
	QRect cAll;
	// 右侧涨幅区域
	QRect disperRect;
	// 右侧持仓区域
	QRect qtyRect;
	// 右侧盈亏区域；
	QRect bsRect;
	// 右键菜单
	QMenu * ractions;
	// 追踪止损
	QAction *trallingAction;
	// OCO
	QAction *ocoAction;
	// OCO price
	double price1;
	// OCO price
	double price2;
	// OCO bos
	char bos1;
	// OCO 触发条件
	char meFlag1;
	// OCO price
	double price3;
	// OCO price
	double price4;
	// OCO bos
	char bos2;
	// OCO 触发条件
	char meFlag2;
	// 右键位置x
	int curposx;
	// 右键位置y
	int curposy;
protected:
	// oco校验
	void checkOCO(double curp, char bos, double p1, double p2);
	// 建立OCO订单
	void initOcoOrder();
	// 建立追踪止损单
    void initZZOrder(TThostFtdcDirectionType bs);
	// 处理鼠标滚轮事件
	void wheelEvent(QWheelEvent * event);
	// 构建或repaint或update后触发
	void paintEvent(QPaintEvent * event);
	// 记录鼠标按下时的状态
	void mousePressEvent(QMouseEvent * event);
	// 窗口双击事件
	void mouseDoubleClickEvent(QMouseEvent * event);
	// 记录鼠标松开时的状态
	void mouseReleaseEvent(QMouseEvent * event);
	// 鼠标移动时的事件处理
	void mouseMoveEvent(QMouseEvent * event);
	// 键盘事件
	void keyPressEvent ( QKeyEvent * event );
	// 右键菜单
	void contextMenuEvent(QContextMenuEvent* e);
public slots:
	// 超时处理
	void onTimerOut();
	// 撤单响应
	void rcancelAction();
	// 限价止损
	void rLimitStopAction();
	// 追踪止损
	void rtrallingAction();
	//OCO
	void ocoTrallAction();
	// oso
	void osoTrallAction();
	// 切换合约
	void initByInstr();
};

#endif // COOLSUBMIT_H
