#ifndef INSTRMANAGE_H
#define INSTRMANAGE_H
#include <QWidget>
#include <QPainter>
#include <QLineEdit>
#include <QLabel>
#include <QMap>
#include "loginWin.h"

class InstrManage: public QWidget
{
	Q_OBJECT
public:
    InstrManage(CThostFtdcInstrumentField * sd, QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~InstrManage();// 用于设定手数
	QLineEdit * searchBox; // 输入关键字
	QLabel * lenLbl; // 联想文字
	// 常用合约列表
    QMap<QString, CThostFtdcInstrumentField *> cinsMap;
	// 检索集合
    QMap<QString, CThostFtdcInstrumentField *> * minMap;
    CThostFtdcInstrumentField * curInstr;
    QList<CThostFtdcInstrumentField *> insList;// 符合的合约列表
	int lineIndex; // 选择的行数
	// 初期化显示
    void init(QMap<QString, CThostFtdcInstrumentField *> * map);
protected:
	// 构建或repaint或update后触发
	void paintEvent(QPaintEvent * event);
	// 鼠标移动时的事件处理
	void mouseMoveEvent(QMouseEvent * event);
	// 记录鼠标松开时的状态
	void mouseReleaseEvent(QMouseEvent * event);
	// 键盘事件
	void keyPressEvent ( QKeyEvent * event );
public slots:
	void SearchInstr(QString s);
signals:
	void changed();
};

#endif
