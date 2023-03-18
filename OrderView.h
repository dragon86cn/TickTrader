#ifndef ORDERVIEW_H
#define ORDERVIEW_H
#include <QWidget>
#include <QPainter>
#include <QLineEdit>
#include <QPushButton>
#include <QWheelEvent>
#include <QMouseEvent>
#include "loginWin.h"

class TradeWidget;
class InstrManage;
class OrderView : public QWidget
{
	Q_OBJECT

public:
    OrderView(TradeWidget * tWidget, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~OrderView();
	// 下单窗口
	TradeWidget *tw; 
	// 合约选择
	InstrManage * selectInstr; 
	// Buy按钮区域
	QRect buyRect;
	// Sell按钮区域
	QRect sellRect;
	// 调整价位区域
	QRect changePriceRect;
	// 调整手数区域
	QRect changeLotRect;
	// 合约号区域
	QRect InstrumentIDRect;
	// 批量手数区域
	QRect batchLotRect;
	// 合约输入区域
	QRect insRect;
	// 调整价位
	double udPrice;
	// 调整手数
	int udLot;
	// 初始价格
    CThostFtdcDepthMarketDataField orQuot;
protected:
	// 构建或repaint或update后触发
	void paintEvent(QPaintEvent * event);
	// 鼠标移动时的事件处理
	void mouseMoveEvent(QMouseEvent * event);
	// 记录鼠标松开时的状态
	void mouseReleaseEvent(QMouseEvent * event);
	// 键盘事件
	void keyPressEvent( QKeyEvent * event );
};

#endif // ORDERVIEW_H
