#ifndef NOTICE_H
#define NOTICE_H

#include <QDialog>
#include <QPainter>
#include <QLineEdit>
#include <QPushButton>

class Notice : public QDialog
{
	Q_OBJECT

public:
	enum NOTICE_TYPE {
		NOTICE_TYPE_STANDARD = 0,	// 标准窗口
		NOTICE_TYPE_NOTIFICATION,	// 通知窗口
		NOTICE_TYPE_WARNING,		// 警告窗口
		NOTICE_TPYE_ERROR,			// 错误窗口
	};

    Notice(NOTICE_TYPE type, QString mess, bool noticeStatus, QString title, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~Notice();
	
	// 确定区域
	QRect reserveRect;
	// 取消区域
	QRect cancelRect;

	// checkBox区域
	QRect checkRect;

	QRect checkBoxRect;
	
	// CheckBox状态
	bool _noticeStatus;
	
	// 确认true&取消false的button
	bool pushButton;

	// 窗口名
	QString winTitle;

	// 提示内容
	QString message;

private:
	// 提示类型实例
	NOTICE_TYPE _type;

protected:
	// 构建或repaint或update后触发
	void paintEvent(QPaintEvent * event);
	// 记录鼠标松开时的状态
	void mouseReleaseEvent(QMouseEvent * event);
	// 键盘事件
	void keyPressEvent(QKeyEvent *event);

};

#endif // NOTICE_H
