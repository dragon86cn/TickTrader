#ifndef POSIMANAGE_H
#define POSIMANAGE_H
#include <QWidget>
#include <QPainter>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include "loginWin.h"

class TradeWidget;
class PosiManage: public QWidget
{
	Q_OBJECT
public:
    PosiManage(TradeWidget * tWidget, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~PosiManage();// 用于设定手数
	TradeWidget * tw; // 下单窗口
protected:
	// 构建或repaint或update后触发
	void paintEvent(QPaintEvent * event);
	// 鼠标移动时的事件处理
	void mouseMoveEvent(QMouseEvent * event);
	// 记录鼠标松开时的状态
	void mouseReleaseEvent(QMouseEvent * event);
	// 窗口双击事件
	void mouseDoubleClickEvent(QMouseEvent * event);
	// 处理鼠标滚轮事件
	void wheelEvent(QWheelEvent * event);
	// 滚动行数
	int wrows;
	// 单击行数
	int clickLine;
	// 使用叫买叫卖价
	bool BSPriceIsUsed;
	//// 数量区域
	//QRect qtyRect;
	//// 数量编辑区域
	//QRect qtyEditRect;
	//// 价格区域
	//QRect priceRect;
	// CheckBox区域
	QRect checkRect;
	// 平仓区域
	QRect closeRect;
	// 行权区域
	QRect depositRect;
	// 当前持仓
    CThostFtdcInvestorPositionField * c_posi;
	// 当前价格
    CThostFtdcDepthMarketDataField *s_Quot;
	// 数量键入
	QLineEdit *qtyLineEdit;
	// 实时价格
	double posiCurrentPrice;
	// 价格
	double BSPrice;
	// 数量
	int BSQty;
	// 选中数量
	int selectedBSQty;
	// 选中价格
	double selectedPrice;
private:
	// 绘制持仓表格
	void paintPosiTable(QPainter * p);
	// 绘制按钮区域
	void paintBox(QPainter * p);
public slots:
	// 平仓
//	void closePosition(CThostFtdcInvestorPositionField *currentPosi);
	// 行权
//	void depositAction(CThostFtdcInvestorPositionField *currentPosi);
	void closePosition();
	void depositAction();
	// k线展示
	void showCurve();
};

#endif
