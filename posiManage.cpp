#include "posiManage.h"
#include "coolsubmit.h"
#include "tradewidget.h"
#include "notice.h"

#define LINPIX 20
// 构建登录窗口
extern loginWin * loginW;
// 线程锁
//extern QMutex mutex;
extern QString convertBsFlag(TThostFtdcDirectionType flag);
extern QString convertOcFlag(TThostFtdcDirectionType flag);
extern QString convertPriceType(TThostFtdcDirectionType flag);
extern QString convertConditionMethod(TThostFtdcDirectionType flag);
extern QString convertOrderStatus(TThostFtdcDirectionType flag);
extern QString convertExchangeID(TThostFtdcExchangeIDType ID);
extern quint32 CreateNewRequestID();

PosiManage::PosiManage(TradeWidget * tWidget, QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f)
{
	tw = tWidget;
	QIcon icon;
    icon.addFile(QString::fromUtf8(":/image/images/CC@32px.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);
	setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowTitle(QString::fromLocal8Bit("持仓管理"));
	connect(tw,SIGNAL(floating()), this, SLOT(update()));
	setMouseTracking(true);
	// 初始化
	qtyLineEdit = new QLineEdit(this);
	qtyLineEdit->setText("1");
	qtyLineEdit->hide();
	qtyLineEdit->setAlignment(Qt::AlignHCenter);
	qtyLineEdit->setValidator(new QIntValidator(0, 999999999, this));
	BSPriceIsUsed = true;
	c_posi = NULL;
	s_Quot = NULL;
	wrows = 0;
	clickLine = 0;
	posiCurrentPrice = 0;
	BSPrice = 0;
	BSQty = 0;
	selectedBSQty = 0;
	selectedPrice = 0;
	// 关闭时自动释放
	setAttribute(Qt::WA_DeleteOnClose, false);

	QAction *closePosition_action  = new QAction(QString::fromLocal8Bit("&平仓"), this);
	addAction(closePosition_action);
	connect(closePosition_action, SIGNAL(triggered()),this, SLOT(closePosition()));

//	addAction(new QAction(QString::fromLocal8Bit("&打开趋势图"), this));
//	QAction *openK_action  = new QAction(QString::fromLocal8Bit("&打开趋势图"), this);
//	addAction(openK_action);
//	connect(openK_action, SIGNAL(triggered()),this, SLOT(showCurve()), Qt::QueuedConnection);

//	QAction *deposit_action  = new QAction(QString::fromLocal8Bit("&行权"), this);
//	addAction(deposit_action);
//	connect(deposit_action, SIGNAL(triggered()),this, SLOT(depositAction()));

	setContextMenuPolicy(Qt::ActionsContextMenu);
}

PosiManage::~PosiManage()
{
}

void PosiManage::paintEvent(QPaintEvent * event)
{
	//int x = QWidget::mapFromGlobal(cursor().pos()).x();
	//int y = QWidget::mapFromGlobal(cursor().pos()).y();
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QColor(34, 34, 34));
	// 绘制按钮区域
//	paintBox(&painter);
	// 绘制持仓表格
	paintPosiTable(&painter);
	 
	painter.fillRect(QRect(0,0,width(),23),QColor(221,223,225));
	TradeInfo ti = tradeInfoLst[loginW->userName];
    double dqqy = ti.fund->Available+ti.fund->CurrMargin+ti.fund->FrozenCash+ti.fund->FrozenMargin+ti.fund->PositionProfit; // 当前权益
	QString quanyi = QString::number(dqqy,'f',0);

    QString yingkui = QString::number(ti.fund->PositionProfit,'f',0);

	QFont def = painter.font();
	QFont font;
	font.setFamily(QString::fromUtf8("微软雅黑"));
	font.setPointSize(9);
	painter.setFont(font);

    if ( ti.fund->PositionProfit >= 0.00 ) {
		painter.fillRect(QRect(0.75*width(),0,0.25*width(),24), QColor(210, 54, 54));
	} else {
		painter.fillRect(QRect(0.75*width(),0,0.25*width(),24), QColor(87, 167, 56));
	}

	painter.setPen(QColor(0, 0, 0));
	painter.drawText(QRect(0.5*width(),0,0.25*width(),24),  Qt::AlignRight|Qt::AlignVCenter, quanyi);
	painter.drawText(QRect(0.75*width(),0,0.25*width(),24),  Qt::AlignRight|Qt::AlignVCenter, yingkui);
	painter.setFont(def);
}

// 绘制持仓表格
void PosiManage::paintPosiTable(QPainter * p)
{
	int marTop = 24;
	QRect tableRect = QRect(0,24,width(),height()-24);
	p->fillRect(tableRect, QColor(225, 225, 225));
	QRect firstLine = QRect(0,24,width(),20);
	p->fillRect(firstLine, QColor(196, 196, 196));
	int linPix = marTop;
	TradeInfo * ti = &(tradeInfoLst[loginW->userName]);
	if(ti->posiLst.count() == 0)
	{
		QFont def = p->font();
		QFont font;
		font.setFamily(QString::fromUtf8("微软雅黑"));
		font.setPointSize(30);
		p->setFont(font);
		p->drawText(tableRect,Qt::AlignCenter, QString::fromLocal8Bit("暂无持仓"));
		p->setFont(def);
		return;
	}
	p->setPen(QColor(203, 203, 203));
	p->drawLine(0,linPix+LINPIX,width(),linPix+LINPIX);
    int wspan[5] = {35,15,15,25/*,14,14,   14*/,25};
    int wrats[5] = {0,0,0,0,0/*,0,   0,0*/};
    for(int i=1;i<5;i++)
	{
		wrats[i]=wrats[i-1]+wspan[i-1];
	}
    QString otitle[5] = {QString::fromLocal8Bit("商品"),QString::fromLocal8Bit("方向")
						 ,QString::fromLocal8Bit("数量")/*, QString::fromLocal8Bit("买持仓均价")*/
						 ,QString::fromLocal8Bit("价格")/*, QString::fromLocal8Bit("卖持仓均价")*/
						 ,/*QString::fromLocal8Bit("平仓盈亏"), */QString::fromLocal8Bit("盈亏")
						// ,QString::fromLocal8Bit("止盈价"), QString::fromLocal8Bit("止损价")
	};
	int afg = Qt::AlignLeft|Qt::AlignVCenter;
	QFont def = p->font();
	QFont font;
	font.setFamily(QString::fromUtf8("微软雅黑"));
	font.setBold(true);
	p->setFont(font);
    for(int c=0;c<5;c++)
	{
		if(c > 0)
			afg = Qt::AlignRight|Qt::AlignVCenter;
		int c1 = width()*wrats[c]/100;
		int c2 = width()*wrats[c+1]/100;
		p->setPen(QColor(203, 203, 203));
		p->drawLine(c2,linPix,c2,linPix+LINPIX);
		QPen tpen(QColor(68,68,68));
		tpen.setWidth(2);
		p->setPen(tpen);
		p->drawText(QRect(c1+2,linPix,c2-c1-4,LINPIX),afg, otitle[c]);
	}
    p->drawText(QRect(width()*wrats[4]/100+2,linPix,width()*(100-wrats[4])/100-4,LINPIX), afg, otitle[4]);
	p->setFont(def);
	bool jo = false; // 奇偶行显示 false奇 true偶
	QMapIterator<QString, PosiPloy *> i(ti->posiLst);
	int igrows = wrows>0?wrows:0;
    while (i.hasNext() && linPix<height())
    {
        PosiPloy * pp = ti->posiLst[i.next().key()];
        if(igrows > 0)
        {
            igrows --;
            continue;
        }
        if(!pp || pp->posi.size() == 0)
            continue;
        for(int index=0; index<pp->posi.size(); index++)
        {
            CThostFtdcInvestorPositionField * cpi = pp->posi.at(index);
            if(cpi->Position == 0)
                continue;
//            qInfo() << cpi->InstrumentID << cpi->PosiDirection << cpi->Position << cpi->PositionCost << cpi->PositionProfit;
            linPix += LINPIX;
            CThostFtdcInstrumentField * sbInstr = tw->insMap[QString::fromLocal8Bit(cpi->InstrumentID)];
            if(!sbInstr) continue;
            if(pQuotApi && tw->insMap_dy[QString::fromLocal8Bit(sbInstr->InstrumentID)] == NULL)
            {
                // 绘制前先订阅
                int count = 1;
                char **InstrumentID = new char*[count];
                InstrumentID[0] = sbInstr->InstrumentID;
                pQuotApi->SubscribeMarketData(InstrumentID, 1);
                tw->insMap_dy[sbInstr->InstrumentID] = sbInstr;
                delete[] InstrumentID;
            }
            int scale = tw->getScale(cpi->InstrumentID);
            if(jo)
            {
                p->fillRect(QRect(0,linPix+1,width(),LINPIX-1),QColor(220,220,220));
            }
            if(marTop+clickLine*LINPIX == linPix)
            {
                c_posi = cpi;
                s_Quot = tw->quotMap[QString(c_posi->InstrumentID)];
                if(s_Quot)
                    posiCurrentPrice = s_Quot->LastPrice;
                p->fillRect(QRect(0,linPix+1,width(),LINPIX-1),QColor(255,255,255));
            }
            jo = !jo;
            p->setPen(QColor(203, 203, 203));
            p->drawLine(0,linPix+LINPIX,width(),linPix+LINPIX);
            BSQty = cpi->Position;
            otitle[0] = QString::fromLocal8Bit(sbInstr->InstrumentName);/* 商品 */
            otitle[1] = cpi->PosiDirection == THOST_FTDC_PD_Long ? QString::fromLocal8Bit("多"):QString::fromLocal8Bit("空");/* 多空 */
            otitle[2] = QString::number(BSQty);  /* 持仓量 */
            BSPrice = cpi->PositionCost/(cpi->Position*1.00f)/(sbInstr->VolumeMultiple*1.00f);
            otitle[3] = QString::number(BSPrice,'f',scale);/* 持仓均价 */
            //otitle[3] = QString::number(cpi->SellQty);/* 卖持仓量 */
            //otitle[4] = QString::number(cpi->SellPrice,'f',scale);  /* 卖持仓均价 */
            /*otitle[5] = QString::number(cpi->ClosedPL,'f',2);*//* 平仓盈亏 */
            otitle[4] = QString::number(cpi->PositionProfit,'f',2);/* 浮动盈亏 */
            //otitle[7] = QString::number(cpi->ProfitStopPrice,'f',scale);/* 止盈价 */
            //otitle[8] = QString::number(cpi->LossStopPrice,'f',scale);/* 止损价 */
            int afg = Qt::AlignLeft|Qt::AlignVCenter;
            for(int rc=0;rc<4;rc++)
            {
                if(rc >= 2)
                    afg = Qt::AlignRight|Qt::AlignVCenter;
                int c1 = width()*wrats[rc]/100;
                int c2 = width()*wrats[rc+1]/100;
                p->setPen(QColor(203, 203, 203));
                p->drawLine(c2,linPix,c2,linPix+LINPIX);
                p->setPen(QColor(47,135,214));
                QRect vR = QRect(c1 + 1, linPix + 1, c2 - c1 - 2, LINPIX - 2);
                if ( rc == 2 && BSQty > 0 ) {
                    p->fillRect(vR, QColor(13, 150, 214));
                    p->setPen(QColor(240, 240, 240));
                } else if ( rc == 2 && BSQty < 0 ) {
                    p->fillRect(vR, QColor(210, 54, 54));
                    p->setPen(QColor(240, 240, 240));
                }/* else if ( rc == 5 && cpi->ClosedPL > 0.00 ) {
                    p->fillRect(vR, QColor(210, 54, 54));
                    p->setPen(QColor(240, 240, 240));
                } else if ( rc == 5 && cpi->ClosedPL < 0.00 ) {
                    p->fillRect(vR, QColor(87, 167, 56));
                    p->setPen(QColor(240, 240, 240));
                }*/
                p->setPen(QColor(0, 0, 0));
                p->drawText(QRect(c1+2,linPix,c2-c1-4,LINPIX), afg, otitle[rc]);
            }
            QRect vvR = QRect(width()*wrats[4]/100 + 1, linPix + 1, width()*(100 - wrats[4])/100 - 2, LINPIX - 2);
            p->setPen(QColor(47,135,214));
            if ( cpi->PositionProfit > 0.00 ) {
                p->fillRect(vvR, QColor(210, 54, 54));
                p->setPen(QColor(240, 240, 240));
            } else if (cpi->PositionProfit < 0.00) {
                p->fillRect(vvR, QColor(87, 167, 56));
                p->setPen(QColor(240, 240, 240));
            }
            p->setPen(QColor(0, 0, 0));
            p->drawText(QRect(width()*wrats[4]/100+2,linPix,width()*(100-wrats[4])/100-4,LINPIX), afg, otitle[4]);
        }
    }
}

// 绘制按钮区域
void PosiManage::paintBox(QPainter * p)
{
	int x = QWidget::mapFromGlobal(cursor().pos()).x();
	int y = QWidget::mapFromGlobal(cursor().pos()).y();
	// ToolBar 高60px的按钮区域
	p->fillRect(QRect(0, 0, width(), 48),QColor(173,173,173));
	p->setPen(QColor(104,104,104));
	p->drawLine(0,24,width(),24);
	p->drawLine(0,48,width(),48);
	//// 数量区域
	//qtyRect = QRect(5, 5, 3*LINPIX, 3*LINPIX);
	//qtyEditRect = QRect(6, LINPIX+5, 3*LINPIX-2, LINPIX);
	QLinearGradient linearGradient;
	//linearGradient.setStart(5, 5);
	//linearGradient.setFinalStop(5, 3*LINPIX+5);
	//linearGradient.setColorAt(0,QColor(128, 128, 128));
	//linearGradient.setColorAt(1,QColor(84, 84, 84));
	//p->setBrush(QBrush(linearGradient));
	//p->setPen(QColor(210, 210, 210));
	//p->drawRoundedRect(qtyRect, 5, 5);
	//p->fillRect(qtyEditRect, QColor(240, 240, 240));
	//qtyLineEdit->setGeometry(qtyEditRect);
	//p->setPen(QColor(34, 34, 34));
	//QPainterPath upQ;
	//upQ.moveTo(35,10);
	//upQ.lineTo(40,17);
	//upQ.lineTo(37,17);
	//upQ.lineTo(37,20);
	//upQ.lineTo(33,20);
	//upQ.lineTo(33,17);
	//upQ.lineTo(30,17);
	//upQ.lineTo(35,10);
	//p->fillPath(upQ,QColor(240, 240, 240));
	//QPainterPath dnQ;
	//dnQ.moveTo(35,60);
	//dnQ.lineTo(40,53);
	//dnQ.lineTo(37,53);
	//dnQ.lineTo(37,50);
	//dnQ.lineTo(33,50);
	//dnQ.lineTo(33,53);
	//dnQ.lineTo(30,53);
	//dnQ.lineTo(35,60);
	//p->fillPath(dnQ,QColor(240, 240, 240));
	//if(c_posi)
	//{
	//	int bsQty = c_posi->BuyQty - c_posi->SellQty;
	//	int scale = tw->getScale(QString(c_posi->InstrumentID));
	//	if (c_posi->BuyQty == 0) {
	//		bsQty = c_posi->SellQty;
	//	} else if (c_posi->SellQty == 0) {
	//		bsQty = c_posi->BuyQty;
	//	} else if (bsQty < 0) {
	//		bsQty *= (-1);
	//	}
	//	p->setPen(QColor(34, 34, 34));
	//	if (!qtyLineEdit->hasFocus()) {
	//		qtyLineEdit->setText(QString::number(selectedBSQty));
	//	}
	//	selectedBSQty = bsQty + udQty;
	//	bool ok;
	//	int qtyText = qtyLineEdit->text().toInt(&ok, 10);
	//	if (qtyText > selectedBSQty ) {
	//		qtyLineEdit->setText(QString::number(selectedBSQty));
	//	} else if ( qtyText < 1) {
	//		qtyLineEdit->setText("1");
	//	}
	//	p->drawText(qtyEditRect,Qt::AlignCenter, qtyLineEdit->text());
	//}

	//// 价格区域
	//priceRect = QRect(3*LINPIX+15, 5, 3*LINPIX, 3*LINPIX);
	//linearGradient.setStart(3*LINPIX+15,5);
	//linearGradient.setFinalStop(3*LINPIX+15,3*LINPIX+5);
	//linearGradient.setColorAt(0,QColor(128, 128, 128));
	//linearGradient.setColorAt(1,QColor(84, 84, 84));
	//p->setBrush(QBrush(linearGradient));
	//p->setPen(QColor(210, 210, 210));
	//p->drawRoundedRect(priceRect, 5, 5);
	//p->fillRect(QRect(3*LINPIX+16, LINPIX+5, 3*LINPIX-2, LINPIX), QColor(240, 240, 240));
	//QPainterPath upP;
	//upP.moveTo(35+3*LINPIX+10,10);
	//upP.lineTo(40+3*LINPIX+10,17);
	//upP.lineTo(37+3*LINPIX+10,17);
	//upP.lineTo(37+3*LINPIX+10,20);
	//upP.lineTo(33+3*LINPIX+10,20);
	//upP.lineTo(33+3*LINPIX+10,17);
	//upP.lineTo(30+3*LINPIX+10,17);
	//upP.lineTo(35+3*LINPIX+10,10);
	//p->fillPath(upP,QColor(240, 240, 240));
	//QPainterPath dnP;
	//dnP.moveTo(35+3*LINPIX+10,60);
	//dnP.lineTo(40+3*LINPIX+10,53);
	//dnP.lineTo(37+3*LINPIX+10,53);
	//dnP.lineTo(37+3*LINPIX+10,50);
	//dnP.lineTo(33+3*LINPIX+10,50);
	//dnP.lineTo(33+3*LINPIX+10,53);
	//dnP.lineTo(30+3*LINPIX+10,53);
	//dnP.lineTo(35+3*LINPIX+10,60);
	//p->fillPath(dnP,QColor(240, 240, 240));

	//p->setPen(QColor(34, 34, 34));
	//if(c_posi && false == BSPriceIsUsed) {
	//	int scale = tw->getScale(QString(c_posi->InstrumentID));
	//	p->drawText(QRect(3*LINPIX+16, LINPIX+5, 3*LINPIX-2, LINPIX),Qt::AlignCenter, QString::number(posiCurrentPrice+udPrice,'f',scale));
	//	selectedPrice = posiCurrentPrice+udPrice;
	//} else if (c_posi && true == BSPriceIsUsed) {
	//	int scale = tw->getScale(QString(c_posi->InstrumentID));
	//	int qtyMinus = c_posi->BuyQty - c_posi->SellQty;
	//	if (c_posi->SellQty == 0 || qtyMinus > 0) {
	//		if(s_Quot)
	//			posiCurrentPrice = s_Quot->BidPrice;
	//	} else if (c_posi->BuyQty == 0 || qtyMinus < 0) {
	//		if(s_Quot)
	//			posiCurrentPrice = s_Quot->AskPrice;
	//	}
	//	p->drawText(QRect(3*LINPIX+16, LINPIX+5, 3*LINPIX-2, LINPIX),Qt::AlignCenter, QString::number(posiCurrentPrice,'f',scale));
	//	selectedPrice = posiCurrentPrice;
	//}

	//// CheckBox
	//QFont def = p->font();
	//QFont font;
	//font.setPointSize(8);
	//p->setFont(font);
	//QRect cRect = QRect(8*LINPIX+4, 10, 4*LINPIX+2, 17);
	//checkRect = QRect(7*LINPIX+5, 10, 17, 17);
	//linearGradient.setStart(7*LINPIX+5, 10);
	//linearGradient.setFinalStop(7*LINPIX+5, LINPIX+7);
	//linearGradient.setColorAt(0,QColor(240,240,240));
	//linearGradient.setColorAt(1,QColor(205,205,205));
	//p->setBrush(QBrush(linearGradient));
	//p->setPen(QColor(34, 34, 34));
	//p->drawRect(checkRect);
	//if(BSPriceIsUsed)
	//{
	//	p->setPen(QPen(QColor(34, 34, 34), 3));
	//	p->drawLine(7*LINPIX+6, 18, 7*LINPIX+13, LINPIX+6);
	//	p->drawLine(7*LINPIX+13, LINPIX+6, 8*LINPIX+1, 11);
	//}
	//p->drawText(cRect,Qt::AlignCenter, QString::fromLocal8Bit("使用叫买/叫卖价"));
	//p->setFont(def);

	// 平仓区域
	closeRect = QRect(2, 26, 50, 20);
	linearGradient.setStart(2, 26);
	linearGradient.setFinalStop(2, 46);
	if(closeRect.contains(x,y)) {
		linearGradient.setColorAt(0,QColor(93, 20, 118));
		linearGradient.setColorAt(1,QColor(116, 40, 148));
	} else {
		linearGradient.setColorAt(0,QColor(116, 40, 148));
		linearGradient.setColorAt(1,QColor(93, 20, 118));
	}
	p->setBrush(QBrush(linearGradient));
	p->setPen(QColor(210, 210, 210));
	p->drawRoundedRect(closeRect, 2, 2);
	p->setPen(QColor(240, 240, 240));
	p->drawText(closeRect,Qt::AlignCenter, QString::fromLocal8Bit("平仓"));//平仓
	// 行权区域
/*	depositRect = QRect(70, 26, 50, 20);
	if(c_posi && c_posi->BuyQty>0 && tw->insMap[QString::fromLocal8Bit(c_posi->InstrumentID)] && tw->insMap[QString(c_posi->InstrumentID)]->InstrumentType == 'O') // TODO
	{
		linearGradient.setStart(70, 26);
		linearGradient.setFinalStop(70, 46);
		if(depositRect.contains(x,y)) {
			linearGradient.setColorAt(0,QColor(178, 40, 40));
			linearGradient.setColorAt(1,QColor(210, 54, 54));
		} else {
			linearGradient.setColorAt(0,QColor(210, 54, 54));
			linearGradient.setColorAt(1,QColor(178, 40, 40));
		}
		p->setBrush(QBrush(linearGradient));
		p->setPen(QColor(210, 210, 210));
		p->drawRoundedRect(depositRect, 2, 2);
		p->setPen(QColor(240, 240, 240));
		p->drawText(depositRect,Qt::AlignCenter, QString::fromLocal8Bit("行权"));//平仓
	}
	else
	{
		linearGradient.setStart(12*LINPIX+5, 2*LINPIX-2);
		linearGradient.setFinalStop(12*LINPIX+5, 3*LINPIX+5);
		linearGradient.setColorAt(0,QColor(187, 187, 187));
		linearGradient.setColorAt(1,QColor(134, 134, 134));
		p->setBrush(QBrush(linearGradient));
		p->setPen(QColor(210, 210, 210));
		p->drawRoundedRect(depositRect, 2, 2);
		p->setPen(QColor(0, 0, 0));
		p->drawText(depositRect,Qt::AlignCenter, QString::fromLocal8Bit("行权"));//平仓
    }*/
	p->setPen(QColor(34, 34, 34));
}

// 鼠标移动时的事件处理
void PosiManage::mouseMoveEvent(QMouseEvent * event)
{
	update();
}

// 记录鼠标松开时的状态
void PosiManage::mouseReleaseEvent(QMouseEvent * event)
{
	if(event->button() != Qt::LeftButton)
	{
		return;
	}
	int x = event->pos().x();
	int y = event->pos().y();
    CThostFtdcInstrumentField * csi = NULL;
	double minMove = 0.01;
	int mQty = 0;
	if ( y > 47) {
		clickLine = (y-28)/LINPIX;
	}
//	if(c_posi) {
//		mQty = c_posi->BuyQty - c_posi->SellQty;
//		csi = tw->insMap[QString(c_posi->InstrumentID)];
//		if(csi) {
//            minMove = csi->PriceTick;
//		}
//	}
//	if (mQty < 0) {
//		mQty *= (-1);
//	}
	//if(qtyRect.contains(event->pos()) && qtyRect.contains(QPoint(x,y+40)) && mQty + udQty < mQty) // 加手数
	//{
	//	udQty++;
	//}
	//if(qtyRect.contains(event->pos()) && qtyRect.contains(QPoint(x,y-40)) && mQty + udQty > 1) // 减手数
	//{
	//	udQty--;
	//}
	//
	//if(qtyEditRect.contains(QPoint(x, y))) {
	//	qtyLineEdit->show();
	//	qtyLineEdit->setFocus(Qt::OtherFocusReason);
	//} else {
	//	qtyLineEdit->hide();
	//	//qtyLineEdit->setText(QString::number(selectedBSQty));
	//	bool ok;
	//	int qtyText = qtyLineEdit->text().toInt(&ok, 10);
	//	selectedBSQty = qtyText;
	//}

	//if(priceRect.contains(event->pos()) && priceRect.contains(QPoint(x,y+40))) // 加价格
	//{
	//	udPrice += minMove;
	//}
	//if(priceRect.contains(event->pos()) && priceRect.contains(QPoint(x,y-40))) // 减价格
	//{
	//	udPrice -= minMove;
	//}
	
	if(closeRect.contains(event->pos()) && c_posi && csi) {
		// 平仓操作
//		closePosition(c_posi);
		// 重置选中量
		selectedBSQty = 0;
		selectedPrice = 0;
	}

	if(checkRect.contains(event->pos())) {
		BSPriceIsUsed = !BSPriceIsUsed;
	}
//	if(c_posi && c_posi->BuyQty>0 && tw->insMap[QString::fromLocal8Bit(c_posi->InstrumentID)] && tw->insMap[QString(c_posi->InstrumentID)]->InstrumentType == 'O' && depositRect.contains(event->pos()))
//	{
//		// 行权操作
////		depositAction(c_posi);
//	}
	update();
}

// 处理鼠标滚轮事件
void PosiManage::wheelEvent(QWheelEvent * event)
{
	if(event->delta() > 0)
	{
		if(wrows >0)
		{
			clickLine ++;
			wrows --;
		}
	}
	else if(event->delta() < 0)
	{
		clickLine --;
		wrows++;
	}
	update();
}

	// 窗口双击事件
void PosiManage::mouseDoubleClickEvent(QMouseEvent * event)
{
	if(!c_posi) return;
	if(!tw->CSubmit) return;
	tw->CSubmit->selectInstr->curInstr = tw->insMap[QString::fromLocal8Bit(c_posi->InstrumentID)];
	tw->CSubmit->initByInstr();
	tw->CSubmit->update();
}

// 平仓
void PosiManage::closePosition() {
    CThostFtdcInvestorPositionField *currentPosi = c_posi;
	//传入当前订单以及平仓价格
	if (!currentPosi)
		return;
    if(currentPosi->Position == 0)
		return;
    CThostFtdcInstrumentField * cI = tw->insMap[QString::fromLocal8Bit(currentPosi->InstrumentID)];
	if(!cI)
		return;
    CThostFtdcDepthMarketDataField * pp = tw->quotMap[QString::fromLocal8Bit(cI->InstrumentID)];
	if(!pp)
		return;
	int scale = tw->getScale(QString(currentPosi->InstrumentID));
    CThostFtdcInputOrderField sellOrder;
    CThostFtdcInputOrderField buyOrder;
	// 录入订单
	QString msg = "";
    int nRequestID = CreateNewRequestID();
	// 买卖标志判断
    if (currentPosi->PosiDirection == THOST_FTDC_PD_Long) {
		// 多仓, 多平
        ::memset(&sellOrder, 0, sizeof(CThostFtdcInputOrderField));
        strncpy(sellOrder.BrokerID, loginW->m_users.BrokerID, sizeof(sellOrder.BrokerID));
		strncpy(sellOrder.InvestorID, loginW->userName,sizeof(sellOrder.InvestorID));						// 投资者号
		strncpy(sellOrder.InstrumentID, cI->InstrumentID,sizeof(sellOrder.InstrumentID));						// 合约号
		strncpy(sellOrder.ExchangeID, cI->ExchangeID,sizeof(sellOrder.ExchangeID));							// 交易所号
        sprintf(sellOrder.OrderRef, "%12i", nRequestID);
        sellOrder.Direction = THOST_FTDC_D_Sell;
        sellOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
        sellOrder.CombHedgeFlag[0] = THOST_FTDC_BHF_Speculation;
        sellOrder.TimeCondition = THOST_FTDC_TC_GFD;
        sellOrder.VolumeCondition = THOST_FTDC_VC_AV;
        sellOrder.MinVolume = 1;
        sellOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
        sellOrder.LimitPrice = pp->BidPrice1;		// 价格
        sellOrder.VolumeTotalOriginal = currentPosi->Position;			// 数量
        sellOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;				// 限价
        sellOrder.ContingentCondition = THOST_FTDC_CC_Immediately;		// 限价单模式
        msg = QString::fromLocal8Bit(cI->InstrumentID).append(QString::fromLocal8Bit(" 卖出 ")).append(QString::number(currentPosi->Position)).append(QString::fromLocal8Bit("手 价格")).append(QString::number(pp->BidPrice1));
	}
    if (currentPosi->PosiDirection == THOST_FTDC_PD_Short) {
		// 空仓, 空平
        ::memset(&buyOrder, 0, sizeof(CThostFtdcInputOrderField));
        strncpy(buyOrder.BrokerID, loginW->m_users.BrokerID, sizeof(buyOrder.BrokerID));
		strncpy(buyOrder.InvestorID, loginW->userName,sizeof(buyOrder.InvestorID));						// 投资者号
		strncpy(buyOrder.InstrumentID, cI->InstrumentID,sizeof(buyOrder.InstrumentID));						// 合约号
		strncpy(buyOrder.ExchangeID, cI->ExchangeID,sizeof(buyOrder.ExchangeID));							// 交易所号
        sprintf(buyOrder.OrderRef, "%12i", nRequestID);
        buyOrder.Direction = THOST_FTDC_D_Buy;
        buyOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
        buyOrder.CombHedgeFlag[0] = THOST_FTDC_BHF_Speculation;
        buyOrder.TimeCondition = THOST_FTDC_TC_GFD;
        buyOrder.VolumeCondition = THOST_FTDC_VC_AV;
        buyOrder.MinVolume = 1;
        buyOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
        buyOrder.LimitPrice = pp->AskPrice1;		// 价格
        buyOrder.VolumeTotalOriginal = currentPosi->Position;			// 数量
        buyOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;				// 限价
        buyOrder.ContingentCondition = THOST_FTDC_CC_Immediately;		// 限价单模式
        msg.append("\r\n").append(QString::fromLocal8Bit(cI->InstrumentID).append(QString::fromLocal8Bit(" 买入 ")).append(QString::number(currentPosi->Position)).append(QString::fromLocal8Bit("手 价格")).append(QString::number(pp->AskPrice1)));
	}
	Notice nt(Notice::NOTICE_TYPE_WARNING, msg, true, QString::fromLocal8Bit("平仓确认"), NULL, 0);
	nt.exec();
	if (!nt.pushButton) {
		return;
	}
    TradeInfo & info = tradeInfoLst[loginW->userName];
    if(currentPosi->PosiDirection == THOST_FTDC_PD_Long)
        info.api->ReqOrderInsert(&sellOrder, nRequestID);
	nRequestID = CreateNewRequestID();
    if(currentPosi->PosiDirection == THOST_FTDC_PD_Short)
        info.api->ReqOrderInsert(&buyOrder, nRequestID);
}

// 行权
void PosiManage::depositAction()
{
//    CThostFtdcInvestorPositionField *currentPosi = c_posi;
//	if(!currentPosi)
//		return;
//    CThostFtdcInstrumentField * cI = tw->insMap[QString::fromLocal8Bit(currentPosi->InstrumentID)];
//	if(!cI)
//		return;
//	int nRequestID = CreateNewRequestID();
//    CThostFtdcInputOrderField pInputOrder;
//    ::memset(&pInputOrder,0,sizeof(CThostFtdcInputOrderField));
//	strncpy(pInputOrder.InvestorID, loginW->userName, sizeof(pInputOrder.InvestorID));     /* 投资者号 */
//	strncpy(pInputOrder.InstrumentID, cI->InstrumentID, sizeof(pInputOrder.InstrumentID));     /* 合约号 */
//	strncpy(pInputOrder.ExchangeID, cI->ExchangeID, sizeof(pInputOrder.ExchangeID));   /* 交易所号 */
//	pInputOrder.BSFlag = BCESConstBSFlagExecute; /* 买卖标志 : 行权 */
//	pInputOrder.Qty = currentPosi->BuyQty;	/* 数量 */
//    pInputOrder.PriceType = THOST_FTDC_OPT_LimitPrice;   /* 限价 */
//    pInputOrder.ConditionMethod = THOST_FTDC_CC_Immediately;	/* 限价单模式 */
//	TradeInfo & ti = tradeInfoLst[loginW->userName];
//	ti.api->OrderInsertReq(&pInputOrder, nRequestID);
}

void PosiManage::showCurve()
{
//	if(tw->insMap[QString::fromLocal8Bit(c_posi->InstrumentID)])
//	{
//		CurveUnit * CUnit = new CurveUnit(tw->insMap[QString::fromLocal8Bit(c_posi->InstrumentID)]);
//		CUnit->setGeometry(DW/2-215,DH/2-125,430,250);
//		CUnit->setActionm5();
//		CUnitList.append(CUnit);
//		CUnit->show();
//	}
}
