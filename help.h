#ifndef HELP_H
#define HELP_H
#include <QWidget>
#include <QPainter>
#include <QLineEdit>
#include <QPushButton>
#include "ui_help.h"

class TradeWidget;
class Help: public QWidget
{
	Q_OBJECT
public:
    Help(TradeWidget * tWidget, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~Help();// 用于设定手数
private:
	Ui::HelpFrom ui;
protected:
	// 构建或repaint或update后触发
	void paintEvent(QPaintEvent * event);
	// 鼠标移动时的事件处理
	void mouseMoveEvent(QMouseEvent * event);
	// 记录鼠标松开时的状态
	void mouseReleaseEvent(QMouseEvent * event);
public slots:
	void openUrl(QString url);
};

#endif
