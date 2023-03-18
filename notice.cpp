#include "notice.h"
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QDesktopServices>
#include "tradewidget.h"

#define PT 12
#define DW QApplication::desktop()->screenGeometry().width()
#define DH QApplication::desktop()->screenGeometry().height()
#define NWidth 25*PT
#define NHeight 15*PT

Notice::Notice(NOTICE_TYPE type, QString mess, bool noticeStatus, QString title, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
	QIcon icon;
    icon.addFile(QString::fromUtf8(":/image/images/title.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);
	message = mess;
	pushButton = false;
	_type = type;
	setAttribute(Qt::WA_TranslucentBackground);
	winTitle = title; // 弹窗框体Title
	/*switch (type) {
		case NOTICE_TYPE_STANDARD:
			winTitle = QString::fromLocal8Bit("提示");
			break;
		case NOTICE_TYPE_NOTIFICATION:
			winTitle = QString::fromLocal8Bit("通知");
			break;
		case NOTICE_TYPE_WARNING:
			winTitle = QString::fromLocal8Bit("注意");
			break;
		case NOTICE_TPYE_ERROR:
			winTitle = QString::fromLocal8Bit("错误");
			break;
		default: break;
	}*/
	this->setWindowTitle(winTitle);
	this->setMinimumSize(QSize(NWidth, NHeight));
	this->setMaximumSize(QSize(NWidth, NHeight));
	this->setGeometry((DW - NWidth)/2, (DH - NHeight)/2, NWidth, NHeight);
	_noticeStatus = noticeStatus;
	// 关闭时自动释放
	setAttribute(Qt::WA_DeleteOnClose, false);
}

Notice::~Notice()
{
}

void Notice::paintEvent(QPaintEvent * event) {
	
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QColor(34, 34, 34)); 
	painter.fillRect(QRect(0, 0, NWidth, NHeight), QColor(239, 239, 239));
	QFont def = painter.font();
	QFont font;
	QLinearGradient linearGradient;
	
	// 提示信息
	font.setPointSize(11);
	painter.setFont(font);
	/*if (message.size() > 27) {
		QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);*/
		//option.setWrapMode(QTextOption::WordWrap);
		//painter.drawText(QRect(PT, PT, NWidth-2*PT, NHeight-6*PT), /*Qt::AlignCenter,*/ message, option);
	/*} else {*/
		QTextOption option(Qt::AlignCenter);
		option.setWrapMode(QTextOption::WordWrap);	
		painter.drawText(QRect(PT, PT, NWidth-2*PT, NHeight-6*PT), /*Qt::AlignCenter,*/ message, option);
	//}

	font.setPointSize(10);
	painter.setFont(font);
	// 确定区域
	painter.setPen(QColor(83, 148, 233));
	if ( _type == NOTICE_TYPE_NOTIFICATION || _type == NOTICE_TPYE_ERROR ) {
		reserveRect = QRect(8*PT, NHeight - 3*PT, 9*PT, 2*PT);
		linearGradient.setStart(8*PT, NHeight - 3*PT);
		linearGradient.setFinalStop(8*PT, NHeight - PT);
		linearGradient.setColorAt(0,QColor(101, 174, 248));
		linearGradient.setColorAt(1,QColor(35, 138, 249));
		painter.setBrush(QBrush(linearGradient));
		painter.drawRoundedRect(reserveRect, 4, 4);
		painter.setPen(QColor(255, 255, 255));
		painter.drawText(reserveRect, Qt::AlignCenter, QString::fromLocal8Bit("确定"));
		return;
	}
	painter.setPen(QColor(83, 148, 233));
	reserveRect = QRect(4*PT, NHeight - 3*PT, 8*PT, 2*PT);
	linearGradient.setStart(4*PT, NHeight - 3*PT);
	linearGradient.setFinalStop(4*PT, NHeight - PT);
	linearGradient.setColorAt(0,QColor(101, 174, 248));
	linearGradient.setColorAt(1,QColor(35, 138, 249));
	painter.setBrush(QBrush(linearGradient));
	painter.drawRoundedRect(reserveRect, 4, 4);
	painter.setPen(QColor(255, 255, 255));
	painter.drawText(reserveRect, Qt::AlignCenter, QString::fromLocal8Bit("确定"));

	// 取消区域
	painter.setPen(QColor(220, 220, 220));
	cancelRect = QRect(NWidth - 12*PT, NHeight - 3*PT, 8*PT, 2*PT);
	linearGradient.setStart(NWidth - 12*PT, NHeight - 3*PT);
	linearGradient.setFinalStop(NWidth - 12*PT, NHeight - PT);
	linearGradient.setColorAt(0,QColor(255, 255, 255));
	linearGradient.setColorAt(1,QColor(255, 255, 255));
	painter.setBrush(QBrush(linearGradient));
	painter.drawRoundedRect(cancelRect, 3, 3);
	painter.setPen(QColor(34, 34, 34)); 
	painter.drawText(cancelRect, Qt::AlignCenter, QString::fromLocal8Bit("取消"));
	
	if ( _type == NOTICE_TYPE_WARNING ) {
		return;
	}

	// 提示状态&CheckBox
	painter.setPen(QColor(220, 220, 220));
	checkRect = QRect((NWidth-7*PT)/2, NHeight - 6*PT, 7*PT, PT);
	checkBoxRect = QRect((NWidth-7*PT)/2, NHeight - 6*PT, PT, PT);
	linearGradient.setStart((NWidth-7*PT)/2, NHeight - 6*PT);
	linearGradient.setFinalStop((NWidth-7*PT)/2, NHeight - 5*PT);
	linearGradient.setColorAt(0,QColor(255, 255, 255));
	linearGradient.setColorAt(1,QColor(255, 255, 255));
	painter.setBrush(QBrush(linearGradient));
	painter.drawRoundedRect(checkBoxRect, 1, 1);
	if (_noticeStatus) {
		painter.setPen(QColor(220, 220, 220));
		linearGradient.setStart((NWidth-7*PT)/2, NHeight - 6*PT);
		linearGradient.setFinalStop((NWidth-7*PT)/2, NHeight - 5*PT);
		linearGradient.setColorAt(0,QColor(101, 174, 248));
		linearGradient.setColorAt(1,QColor(35, 138, 249));
		painter.setBrush(QBrush(linearGradient));
		painter.drawRoundedRect(checkBoxRect, 1, 1);
		painter.setPen(QPen(QColor(255, 255, 255), 3));
		painter.drawLine((NWidth-7*PT)/2 + 2, NHeight - 6*PT + 6 + 2, (NWidth-7*PT)/2 + 6, NHeight - 5*PT - 2);
		painter.drawLine((NWidth-7*PT)/2 + 6, NHeight - 5*PT - 2, (NWidth-7*PT)/2 + PT - 2, NHeight - 6*PT + 2);
	}
	painter.setPen(QColor(34, 34, 34));
	font.setPointSize(8);
	painter.setFont(font);
	painter.drawText(QRect((NWidth-7*PT)/2 + PT, NHeight - 6*PT, 6*PT, PT), Qt::AlignCenter, QString::fromLocal8Bit("下次不再确认"));
	painter.setFont(def);
}

// 记录鼠标松开时的状态
void Notice::mouseReleaseEvent(QMouseEvent * event) {
	int x = event->pos().x();
	int y = event->pos().y();

	if ( cancelRect.contains(event->pos()) ) {
		// 取消
		pushButton = false;
		done(0);
	} else if ( reserveRect.contains(event->pos()) ) {
		// 确定
		pushButton = true;
		done(0);
	}
	if ( checkRect.contains(event->pos()) ) {
		//选中CheckBox 关闭订单提示
		_noticeStatus = !_noticeStatus;
		update();
	}
}

// 键盘事件
void Notice::keyPressEvent(QKeyEvent *event) {
	switch (event->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return: {
			pushButton = true;
			done(0);
			break;
			}
		case Qt::Key_Escape: {
			pushButton = false;
			done(0);
			break;
		}
		case Qt::Key_Space: {
 			if ( _type == NOTICE_TYPE_WARNING ) {
			return;
			}
			_noticeStatus = !_noticeStatus;
			update();
			break;
		}
		default:
			break;
	}
	//if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
	//	// 确定
	//	pushButton = true;
	//	done(0);
	//} else if (event->key() == Qt::Key_Escape) {
	//	// 取消
	//	pushButton = false;
	//	done(0);
	//}
	//if ( _type == NOTICE_TYPE_WARNING ) {
	//	return;
	//}
	//if (event->key() == Qt::Key_Space) {
	//	//选中CheckBox 关闭订单提示
	//	_noticeStatus = !_noticeStatus;
	//	update();
	//}
}
