#ifndef DEALVIEW_H
#define DEALVIEW_H
#include <QWidget>
#include <QPainter>
#include <QLineEdit>
#include <QTableWidget>

class TradeWidget;
class CThostFtdcTradeField;
class dealView : public QWidget
{
	Q_OBJECT

public:
    dealView(TradeWidget * tWidget, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~dealView();
	
	TradeWidget * tw;
	// 当前订单
    CThostFtdcTradeField * c_trader;
	// 单击行数
	int clickLine;
	// 滚动行数
	int wrows;

protected:
	// 构建或repaint或update后触发
	void paintEvent(QPaintEvent * event);
	// 记录鼠标松开时的状态
	void mouseReleaseEvent(QMouseEvent * event);
	// 处理鼠标滚轮事件
	void wheelEvent(QWheelEvent * event);
private:
	// 绘制订单表格
	void paintOrderTable(QPainter * p);
};

#endif // DEALVIEW_H
