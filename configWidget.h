#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H
#include <QWidget>
#include <QtGui/QPainter>
#include "ui_configWidget.h"
#include "loginWin.h"

class configWidget : public QWidget
{
	Q_OBJECT

public:
    configWidget(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~configWidget();
	PrivateIns pi;
	void initByPrivats();
private:
	Ui::configForm ui;

//	typedef enum tickMode {
//	// 追价
//	singleTickMode = 0,
//	doubleTickMode,
//	thirdTickMode,
//	defaultTickMode
//};

protected:
	// 构建或repaint或update后触发
	void paintEvent(QPaintEvent * event);
	// 鼠标移动时的事件处理
	void mouseMoveEvent(QMouseEvent * event);
	// 记录鼠标松开时的状态
	void mouseReleaseEvent(QMouseEvent * event);
public slots:
	void setNotice(int s);
	void setSingle(bool cmode);
	void setDouble(bool cmode);
	void setTickMode(int modeCode);
	void initTickMode(int points = 0);
};

#endif // CONFIGWIDGET_H
