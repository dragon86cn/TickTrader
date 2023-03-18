#include "help.h"
#include "coolsubmit.h"
#include "tradewidget.h"
#include <QUrl>
#include <QDesktopServices>

Help::Help(TradeWidget * tWidget, QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f)
{
	ui.setupUi(this);
	QPalette palette;
    QPixmap pixmap(":/image/images/hpback.png");
	palette.setBrush(QPalette::Window, QBrush(pixmap.scaled(330, 200)));
	this->setPalette(palette);
	QIcon icon;
    icon.addFile(QString::fromUtf8(":/image/images/help.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);
	setMouseTracking(true);
	// 关闭时自动释放
	setAttribute(Qt::WA_DeleteOnClose, false);
    connect(ui.label_4,SIGNAL(linkActivated(QString)),this,SLOT(openUrl(QString)));  //在.h里面定义一个槽private slots: void openUrl(QString url);
}

Help::~Help()
{
}

void Help::paintEvent(QPaintEvent * event)
{
}


// 鼠标移动时的事件处理
void Help::mouseMoveEvent(QMouseEvent * event)
{
	update();
}


// 记录鼠标松开时的状态
void Help::mouseReleaseEvent(QMouseEvent * event)
{
}

void Help::openUrl(QString url)
{
	QDesktopServices::openUrl(url); // 软件下载主页
}
