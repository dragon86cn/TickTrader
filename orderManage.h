#ifndef ORDERMANAGE_H
#define ORDERMANAGE_H
#include <QWidget>
#include <QtGui/QPainter>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>

class TradeWidget;
class CThostFtdcOrderField;
class OrderManage: public QWidget
{
	Q_OBJECT
public:
    OrderManage(TradeWidget * tWidget,QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~OrderManage();// 用于设定手数
	TradeWidget * tw; // 下单窗口
	//// 修改按钮区域
	//QRect changeRect;
	//// 调价区域
	//QRect upriceRect;
	// 单步up区域
	QRect upOneRect;
	// 单步down区域
	QRect dnOneRect;
	// 撤单区域
	QRect cancelRect;
	//// checkBox区域
	//QRect checkRect;
	// 横向滚动条区域
	QRect vsBarR;
	// 纵向滚动条区域
	QRect hsBarR;
	// 横向滚动条拖动区域
	QRect vsBarRt;
	// 纵向滚动条拖动区域
	QRect hsBarRt;
	// 横向滚动条位置
	int v_sc;
	// 记录鼠标拖动的宽度起点
	int moveWPixs;
	// 记录鼠标拖动的高度起点
	int moveHPixs;
	// 当前订单
    CThostFtdcOrderField * c_order;
	// 调高调低量
	double udNum;
	// 单击行数
	int clickLine;
	// 只显示已报入订单
	bool orderStatus;
	// 滚动行数
	int wrows;
	// 右键菜单
	QMenu * setMenu;
protected:
	// 右键菜单
	void contextMenuEvent(QContextMenuEvent* e);
	// 构建或repaint或update后触发
	void paintEvent(QPaintEvent * event);
	// 记录鼠标按下时的状态
	void mousePressEvent(QMouseEvent * event);
	// 鼠标移动时的事件处理
	void mouseMoveEvent(QMouseEvent * event);
	// 记录鼠标松开时的状态
	void mouseReleaseEvent(QMouseEvent * event);
	// 窗口双击事件
	void mouseDoubleClickEvent(QMouseEvent * event);
	// 处理鼠标滚轮事件
	void wheelEvent(QWheelEvent * event);
	// 窗口大小调整
	void resizeEvent(QResizeEvent * event);
private:
	// 修改订单
    void updateOrder(CThostFtdcOrderField * co, double price2);
	// 删除订单
    void dropOrder(CThostFtdcOrderField * co);
	// 绘制按钮区域
	void paintBox(QPainter * p);
	// 绘制订单表格
	void paintOrderTable(QPainter * p);
	public slots:
		// 右键 ：只显示活跃订单
		void changeOrderStatus();
};

#endif
