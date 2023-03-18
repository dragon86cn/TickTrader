#include "configWidget.h"
#include "coolsubmit.h"
#include "tradewidget.h"
#include <QComboBox>

//extern void writeUsers(DBUsers & di);
//extern void writePrivates(PrivateIns & pi);

configWidget::configWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    ui.setupUi(this);
    ::memset(&pi,0,sizeof(PrivateIns));
    //::memset(pi.InstrumentID,0,100*sizeof(TThostFtdcInstrumentIDType));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/image/images/config.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);
    setMouseTracking(true);
    initByPrivats();
    setAttribute(Qt::WA_DeleteOnClose, false);
    //connect(ui.singleClickButton,SIGNAL(clicked(bool)), this, SLOT(setSingle(bool)));
    //connect(ui.doubleClickButton,SIGNAL(clicked(bool)), this, SLOT(setDouble(bool)));
    connect(ui.checkOrderTip,SIGNAL(stateChanged(int)), this, SLOT(setNotice(int)));
    connect(ui.tickComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(initTickMode(int)));
    connect(ui.tickCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setTickMode(int)));
}

configWidget::~configWidget()
{
}

void configWidget::initByPrivats()
{
    pi.clickmode = 1;
    ui.checkOrderTip->setChecked(pi.notice);
    switch(pi.fPoints)
    {
    case 0:
        ui.tickCheckBox->setCheckState(Qt::Unchecked);
        ui.tickComboBox->setEnabled(false);
        break;
    case 1:
    case 2:
    case 3:
        ui.tickComboBox->setCurrentIndex(pi.fPoints-1);
        ui.tickCheckBox->setCheckState(Qt::Checked);
        break;
    }
}

void configWidget::paintEvent(QPaintEvent * event)
{
    /*if(pi.clickmode == 1)
    {
        ui.singleClickButton->setChecked(true);
    }
    else
    {
        ui.doubleClickButton->setChecked(true);
    }*/
    if(pi.notice == 0)
    {
        ui.checkOrderTip->setChecked(false);
    }
    else
    {
        ui.checkOrderTip->setChecked(true);
    }
}

void configWidget::setSingle(bool cmode)
{
    pi.clickmode = 1;
//    writePrivates(pi);
    update();
}

void configWidget::setDouble(bool cmode)
{
    pi.clickmode = 2;
//    writePrivates(pi);
    update();
}

void configWidget::setNotice(int s)
{
    if(s == 2)
    {
        pi.notice = 1;
    }
    else
    {
        pi.notice = 0;
    }
//    writePrivates(pi);
//    char * nc = "notice";
}

void configWidget::initTickMode(int points) {
    switch (points) {
        case 0:
            pi.fPoints = 1;
            break;
        case 1:
            pi.fPoints = 2;
            break;
        case 2:
            pi.fPoints = 3;
            break;

        default:
            break;
    }
//    writePrivates(pi);
}

void configWidget::setTickMode(int modeCode) {
    if(!ui.tickCheckBox->isChecked()) {
        ui.tickComboBox->setEnabled(false);
        pi.fPoints = 0;
    } else {
        pi.fPoints = ui.tickComboBox->currentIndex()+1;
        initTickMode(ui.tickComboBox->currentIndex());
        ui.tickComboBox->setEnabled(true);
    }
//    writePrivates(pi);
}

void configWidget::mouseMoveEvent(QMouseEvent * event)
{
    update();
}

void configWidget::mouseReleaseEvent(QMouseEvent * event)
{
}
