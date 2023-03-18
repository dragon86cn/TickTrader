#ifndef PRICEVIEW_H
#define PRICEVIEW_H
#include <QWidget>
#include <QPainter>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include "InstrManage.h"

typedef struct
{
	QString name;       // 集合名
	QRect privateSet;    // 绘图区域
	QList<QString> instrList; // 合约号清单
}PrivateInstrs;  // 自选合约

#define DEFAULT_W 1200

class TradeWidget;

class PriceView: public QWidget {
	Q_OBJECT
public:
    PriceView(TradeWidget * tWidget, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~PriceView();// 用于设定手数
	TradeWidget * tw; // 下单窗口
protected:
	// 构建或repaint或update后触发
	void paintEvent(QPaintEvent *event);
	// 鼠标移动时的事件处理
	void mouseMoveEvent(QMouseEvent * event);
	// 记录鼠标按下时的状态
	void mousePressEvent(QMouseEvent * event);
	// 记录鼠标松开时的状态
	void mouseReleaseEvent(QMouseEvent * event);
	// 窗口双击事件
	void mouseDoubleClickEvent(QMouseEvent * event);
	// 处理鼠标滚轮事件
	void wheelEvent(QWheelEvent * event);
	// 右键菜单
	void contextMenuEvent(QContextMenuEvent* e);
	// 键盘事件
	void keyPressEvent ( QKeyEvent * event);
	// 窗口大小调整
	void resizeEvent(QResizeEvent * event);
	// 滚动行数
	int wrows;
	// 横向滚动条位置
	int v_sc;
	// 纵向滚动条位置
	int h_sc;
	// 横向滚动条区域
	QRect vsBarR;
	// 纵向滚动条区域
	QRect hsBarR;
	// 横向滚动条拖动区域
	QRect vsBarRt;
	// 纵向滚动条拖动区域
	QRect hsBarRt;
	// 单击行数
	int clickLine;
	// 记录鼠标拖动的宽度起点
	int moveWPixs;
	// 记录鼠标拖动的高度起点
	int moveHPixs;
	// 自选合约区域
	QRect setsRect;
	// 赛选合约板
	QRect filterRect;
	//// 自选合约板
	//QRect selfRect;
	// 选择交易品种
	InstrManage * selectInstr; 
	// 右键菜单
	QMenu * setMenu;
	// 自选合约清单
	// QMap<QString, PrivateInstrs *> priInstrMap;
	PrivateInstrs * priRect; // 自选
	PrivateInstrs * zlsRect; // 主力
	// 当前 合约类
	PrivateInstrs * curPi;
	// 取消订阅行情
	void cancelQuotReq();
	// 加载数据			
	void loadDBinfo();
	// 校验当前合约是否在自相关列表中 true 在 false 不在
    bool checkInstr(CThostFtdcInstrumentField * is);
	// 当前合约map
    QMap<QString, CThostFtdcInstrumentField *> * mapTemp;
	// 显示的合约map
    QMap<QString, CThostFtdcInstrumentField *> viewTemp;
private:
	// 绘制自选板
	void paintPriBoard(QPainter *p);
	// 绘制报价板
	void paintPriceBoard(QPainter *p);
public slots:
	void getInstr();
	// 打开下单界面
	/*void showTradeW();*/
	// 添加到自选合约map
	void addToSelfMap();
	// 删除自选
	void delFrSelfMap();
	// k线展示
	void showCurve();
};

#endif
