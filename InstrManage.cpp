#include "InstrManage.h"
#include "coolsubmit.h"
#include "tradewidget.h"

#define LINPIX 20
extern TradeWidget * g_tw;

InstrManage::InstrManage(CThostFtdcInstrumentField * sd, QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f)
{
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(Qt::FramelessWindowHint);
	setWindowOpacity(0.5);
	curInstr = sd;
	lineIndex= 0;
	minMap = NULL;
    int size = g_tw->insMap.count();
    if(curInstr == nullptr && g_tw && g_tw->insMap.count() > 0){
        QMap<QString, CThostFtdcInstrumentField *>::iterator iter = g_tw->insMap.begin();
        CThostFtdcInstrumentField *ins = iter.value();
        curInstr = new CThostFtdcInstrumentField;
        memcpy(curInstr, ins, sizeof(ins));
    }
	/*posiFilterBox = new QCheckBox(this);
	posiFilterBox->setText(QString::fromLocal8Bit("仅显示持仓"));
	connect(posiFilterBox,SIGNAL(clicked(bool)), this, SLOT(SearchClicked(bool)));*/
	searchBox = new QLineEdit(this);
	searchBox->setStyleSheet("color:rgb(34, 34, 34); background-color:rgb(237, 240, 95)");
	lenLbl = new QLabel(searchBox);
	// lenLbl->setStyleSheet("background-color:red");
	QPalette pa;
	pa.setColor(QPalette::WindowText,Qt::red);
	lenLbl->setPalette(pa);
	lenLbl->hide();
	connect(searchBox,SIGNAL(textChanged(QString)), this, SLOT(SearchInstr(QString)));
	searchBox->setText(curInstr->InstrumentID);
	searchBox->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
	searchBox->setWindowOpacity(0.4);
	setMouseTracking(true);
}

InstrManage::~InstrManage()
{
}

void InstrManage::paintEvent(QPaintEvent * event)
{
	int x = QWidget::mapFromGlobal(cursor().pos()).x();
	int y = QWidget::mapFromGlobal(cursor().pos()).y();
	QPainter painter(this); 
	painter.setPen(Qt::white); 
	painter.fillRect(QRect(0,0,width(),LINPIX),Qt::white);
	searchBox->setGeometry(0,0,width(),LINPIX);
	bool usdFlag = true;
	if(insList.length() == 0)
		return;
	for(int d =1;d<=insList.length();d++)
	{
        CThostFtdcInstrumentField * sd = insList[d-1];
		if(!sd)
			continue;
		if(d > 10)
			break;
		QRect rLine = QRect(0,LINPIX*d,width(),LINPIX);
        CThostFtdcDepthMarketDataField * rQout = g_tw->quotMap[QString::fromLocal8Bit(sd->InstrumentID)];
		if(rQout)
			painter.fillRect(rLine, QColor(237, 240, 95));
		else
			painter.fillRect(rLine, QColor(81, 218, 220));
		if(usdFlag && !cinsMap.contains(QString(sd->InstrumentID)))
		{
			usdFlag = false;
			painter.setPen(Qt::blue); 
			painter.drawLine(0,LINPIX*(d+1),width(),LINPIX*(d+1));
		}
		painter.setPen(QColor(34, 34, 34)); 
		painter.drawText(rLine,Qt::AlignCenter, QString::fromLocal8Bit(sd->InstrumentID).append(",").append(QString::fromLocal8Bit(sd->InstrumentName)));
		if(d-1 == lineIndex)
		{
			painter.drawRect(1,LINPIX*d+1,width()-2,LINPIX-2);
		}
		else if(rLine.contains(x,y))
		{
			painter.drawRect(1,LINPIX*d+1,width()-2,LINPIX-2);
		}
	}
}

void InstrManage::SearchInstr(QString s)
{
	lineIndex = 0;
	insList.clear();
	// 过滤空格 防止键盘误敲击
	s = s.remove(" ");
	// 先遍历常用合约
    QMapIterator<QString, CThostFtdcInstrumentField *> cinsItr(cinsMap);
	while(cinsItr.hasNext())
	{
		QString keyP = cinsItr.next().key();
        CThostFtdcInstrumentField * instr = cinsMap[keyP];
		if(!instr)
			continue;
		if(minMap && minMap->value(QString::fromLocal8Bit(instr->InstrumentID)) && QString(instr->InstrumentID).toLower().startsWith(s.toLower()) && !insList.contains(instr))
		{
			insList.append(instr);
		}
	}
	if(!minMap)
        minMap = &g_tw->insMap;
	// 遍历主力合约
    QMapIterator<QString, CThostFtdcInstrumentField *> zlItr(g_tw->insMap_zl);
	while(zlItr.hasNext())
	{
		QString keyP = zlItr.next().key();
		if(cinsMap.contains(keyP) && minMap->contains(keyP))
			continue;
        CThostFtdcInstrumentField * instr = g_tw->insMap_zl[keyP];
		if(instr && QString(instr->InstrumentID).toLower().startsWith(s.toLower()) && !insList.contains(instr))
		{
			insList.append(instr);
		}
	}
	// 再遍历其他合约
    QMapIterator<QString, CThostFtdcInstrumentField *> insItr(*minMap);
	while(insItr.hasNext())
	{
		QString keyP = insItr.next().key();
        CThostFtdcInstrumentField * instr = minMap->value(keyP);
        if(cinsMap.contains(keyP) && !g_tw->insMap_zl.contains(keyP))
			continue;
		if(instr && QString(instr->InstrumentID).toLower().startsWith(s.toLower()) && !insList.contains(instr))
		{
			insList.append(instr);
		}
	}
	if(s != "" && insList.length()>0)
	{
		QString lenStr = QString(insList[0]->InstrumentID).mid(s.length());
		lenLbl->setText(lenStr);
		int sbw = searchBox->width()+s.length()*9;
		lenLbl->setGeometry(sbw/2,2,lenStr.length()*9,LINPIX-4);
		lenLbl->show();
	}
	else
	{
		lenLbl->setText("");
		lenLbl->hide();
	}
	update();
}

// 鼠标移动时的事件处理
void InstrManage::mouseMoveEvent(QMouseEvent * event)
{
	update();
}


// 记录鼠标松开时的状态
void InstrManage::mouseReleaseEvent(QMouseEvent * event)
{
	int y = QWidget::mapFromGlobal(cursor().pos()).y();
	int yl = y/LINPIX-1>=0?y/LINPIX-1:0;
	if(event->button() == Qt::LeftButton)
	{
		if(insList.length() > yl)
		{
			curInstr = insList[yl];
			cinsMap[QString(curInstr->InstrumentID)] = curInstr;
		}
		searchBox->clearFocus();
		this->hide();
		emit changed();
	}
}

void InstrManage::keyPressEvent(QKeyEvent * event )
{
	int ikey = event->key();
	if((ikey == Qt::Key_Enter || ikey == Qt::Key_Return) && insList.length() > lineIndex)
	{
		curInstr = insList[lineIndex];
		cinsMap[QString(curInstr->InstrumentID)] = curInstr;
		searchBox->clearFocus();
		this->hide();
		emit changed();
	}
	if(ikey == Qt::Key_Escape)
	{
		searchBox->clearFocus();
		this->hide();
	}
	else if(ikey == Qt::Key_Up)
	{
		lineIndex = lineIndex > 0?lineIndex-1:0;
		update();
	}
	else if(ikey == Qt::Key_Down)
	{
		if(lineIndex < insList.length()-1)
			lineIndex++;
		update();
	}
}

void InstrManage::init(QMap<QString, CThostFtdcInstrumentField *> * map)
{
	if(map)
		minMap = map;
	searchBox->setText("");
	searchBox->setFocus(Qt::OtherFocusReason);
	this->show();
}
