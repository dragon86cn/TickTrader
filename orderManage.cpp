#include "orderManage.h"
#include "coolsubmit.h"
#include "tradewidget.h"
#include <QPainterPath>

#define TOPPIX 30
#define LINPIX 20
#define MINWID 800

// 构建登录窗口
extern loginWin * loginW;
extern QString convertBsFlag(TThostFtdcDirectionType flag);
extern QString convertOcFlag(TThostFtdcDirectionType flag);
extern QString convertPriceType(TThostFtdcDirectionType flag);
extern QString convertConditionMethod(TThostFtdcDirectionType flag);
extern QString convertOrderStatus(TThostFtdcDirectionType flag);
extern QString convertExchangeID(TThostFtdcExchangeIDType ID);
extern quint32 CreateNewRequestID();

OrderManage::OrderManage(TradeWidget * tWidget, QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f)
{
	tw = tWidget;
	QIcon icon;
    icon.addFile(QString::fromUtf8(":/image/images/OE@32px.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);
	setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowTitle(QString::fromLocal8Bit("订单管理"));
	c_order = NULL;
	udNum = 0;
	orderStatus = false;
	moveWPixs = 9999;
	moveHPixs = 9999;
	wrows = 0;
	v_sc = 0;
	clickLine = 0;
	setMouseTracking(true);
	setMenu = new QMenu(this);
	QAction *spiAction = setMenu->addAction(QString::fromLocal8Bit("只显示未成交"));
	connect(spiAction, SIGNAL(triggered(bool)), this, SLOT(changeOrderStatus()));
	spiAction->setCheckable(true);
	// 关闭时自动释放
	setAttribute(Qt::WA_DeleteOnClose, false);
}

OrderManage::~OrderManage()
{
}

// 窗口大小调整
void OrderManage::resizeEvent(QResizeEvent * event)
{
	v_sc = 0;
}


// 右键菜单
void OrderManage::contextMenuEvent(QContextMenuEvent* e)
{
	setMenu->exec(cursor().pos()); 
}

// 右键 ：只显示活跃订单
void OrderManage::changeOrderStatus()
{
	orderStatus = !orderStatus;
	update();
}

void OrderManage::paintEvent(QPaintEvent * event)
{
	//int x = QWidget::mapFromGlobal(cursor().pos()).x();
	//int y = QWidget::mapFromGlobal(cursor().pos()).y();
	QPainter painter(this); 
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QColor(34, 34, 34)); 
	// 绘制按钮区域
	paintBox(&painter);
	// 绘制订单表格
	paintOrderTable(&painter);
}

// 绘制按钮区域
void OrderManage::paintBox(QPainter * p)
{
	// top 70像素 区域用于绘制按钮
	p->fillRect(QRect(0,0,width(),24),QColor(173,173,173));
	//// 调价按钮
	//upriceRect = QRect(5,5,2*TOPPIX,2*TOPPIX);
	QLinearGradient linearGradient(5,5,5,2*TOPPIX+5);
	//linearGradient.setColorAt(0,QColor(128, 128, 128));
	//linearGradient.setColorAt(1,QColor(84, 84, 84));
	//p->setBrush(QBrush(linearGradient));
	//p->setPen(QColor(210, 210, 210));
	//p->drawRoundedRect(upriceRect, 5, 5);
	//p->fillRect(QRect(6,25,59,20), QColor(240, 240, 240));
	QPainterPath upP;
	//upP.moveTo(35,10);
	//upP.lineTo(40,17);
	//upP.lineTo(37,17);
	//upP.lineTo(37,20);
	//upP.lineTo(33,20);
	//upP.lineTo(33,17);
	//upP.lineTo(30,17);
	//upP.lineTo(35,10);
	//p->fillPath(upP,QColor(240, 240, 240));
	QPainterPath dnP;
	//dnP.moveTo(35,60);
	//dnP.lineTo(40,53);
	//dnP.lineTo(37,53);
	//dnP.lineTo(37,50);
	//dnP.lineTo(33,50);
	//dnP.lineTo(33,53);
	//dnP.lineTo(30,53);
	//dnP.lineTo(35,60);
	//p->fillPath(dnP,QColor(240, 240, 240));
	//if(c_order)
	//{
	//	int scale = tw->getScale(QString(c_order->InstrumentID));
	//	p->setPen(QColor(34, 34, 34));
	//	p->drawText(QRect(6,25,59,20),Qt::AlignCenter, QString::number(c_order->Price+udNum,'f',scale));
	//}
	//
	//// 修改按钮
	//changeRect = QRect(70,5,2*TOPPIX,2*TOPPIX);
	//linearGradient.setStart(70,5);
	//linearGradient.setFinalStop(70,2*TOPPIX+5);
	//linearGradient.setColorAt(0,QColor(123, 192, 96));
	//linearGradient.setColorAt(1,QColor(97, 172, 67));
	//p->setBrush(QBrush(linearGradient));
	//p->setPen(QColor(210, 210, 210));
	//p->drawRoundedRect(changeRect, 5, 5);
	//p->setPen(QColor(240, 240, 240));
	//p->drawText(changeRect,Qt::AlignCenter, QString::fromLocal8Bit("改价"));

	// 单步up按钮
	p->setPen(QColor(210, 210, 210));
	upOneRect = QRect(70,2,50,20);
	linearGradient.setStart(70,2);
	linearGradient.setFinalStop(70,22);
	linearGradient.setColorAt(0,QColor(51,114,159));
	linearGradient.setColorAt(1,QColor(51,114,159));
	p->setBrush(QBrush(linearGradient));
	p->drawRoundedRect(upOneRect,2,2);
	upP.moveTo(78,8);
	upP.lineTo(83,15);
	upP.lineTo(78,12);
	upP.lineTo(73,15);
	upP.lineTo(78,8);
	p->fillPath(upP,QColor(240, 240, 240));
	p->setPen(QColor(255,255,255));
	p->drawText(QRect(80,2,40,20),Qt::AlignCenter, QString::fromLocal8Bit("Tick"));//调高
	// 单步dn按钮
	dnOneRect = QRect(122,2,50,20);
	linearGradient.setStart(122,2);
	linearGradient.setFinalStop(122,22);
	linearGradient.setColorAt(0,QColor(142,33,34));
	linearGradient.setColorAt(1,QColor(142,33,34));
	p->setBrush(QBrush(linearGradient));
	p->setPen(QColor(210, 210, 210));
	p->drawRoundedRect(dnOneRect,2,2);
	dnP.moveTo(130,15);
	dnP.lineTo(135,8);
	dnP.lineTo(130,12);
	dnP.lineTo(125,8);
	dnP.lineTo(130,15);
	p->fillPath(dnP,QColor(240, 240, 240));
	p->setPen(QColor(255,255,255));
	p->drawText(QRect(132,2,40,20),Qt::AlignCenter, QString::fromLocal8Bit("Tick"));//调低
	// 撤消按钮
	p->setPen(QColor(210, 210, 210));
	cancelRect = QRect(2,2,55,20);
	linearGradient.setStart(2,2);
	linearGradient.setFinalStop(2,22);
	linearGradient.setColorAt(0,QColor(138,138,138));
	linearGradient.setColorAt(1,QColor(138,138,138));
	p->setBrush(QBrush(linearGradient));
	p->drawRoundedRect(cancelRect,2,2);
	p->setPen(QColor(255,255,255));
	p->drawText(cancelRect,Qt::AlignCenter, QString::fromLocal8Bit("撤单"));
	//p->setPen(QColor(34, 34, 34));
	//QRect cRect = QRect(295,5,4*TOPPIX,2*TOPPIX);
	//checkRect = QRect(300,TOPPIX-7,20,20);
	//linearGradient.setStart(295,TOPPIX+5);
	//linearGradient.setFinalStop(295,2*TOPPIX+5);
	//linearGradient.setColorAt(0,QColor(210, 54, 54));
	//linearGradient.setColorAt(1,QColor(178, 40, 40));
	//p->setBrush(QColor(240,240,240));
	//p->setPen(QColor(34, 34, 34));
	//p->drawRect(checkRect);
	//if(orderStatus)
	//{
	//	QPainterPath chP;
	//	chP.moveTo(300,TOPPIX+3);
	//	chP.lineTo(310,TOPPIX+13);
	//	chP.lineTo(320,TOPPIX-7);
	//	chP.lineTo(310,TOPPIX+6);
	//	chP.lineTo(300,TOPPIX+3);
	//	p->fillPath(chP,QColor(34, 34, 34));
	//}
	//p->setPen(QColor(0,0,0));
	//p->drawText(QRect(325,5,3*TOPPIX,2*TOPPIX),Qt::AlignCenter, QString::fromLocal8Bit("只显示活跃订单"));
}

// 绘制按钮区域
void OrderManage::paintOrderTable(QPainter * p)
{
	QPen pen(QColor(240,210,6));
	pen.setWidth(2);
	p->setPen(pen);
	int marTop = 24;
	int wid_ = width()>MINWID?width():MINWID;
	int vscl = v_sc*wid_/width(); // 横向滚动条偏移像素
	int pvh = height()-16;
	int vbw = width()*width()/wid_;
	QRect tableRect = QRect(0,marTop,width(),height()-marTop);
	p->fillRect(tableRect, QColor(225, 225, 225));
	int linPix = marTop;
	TradeInfo * ti = &(tradeInfoLst[loginW->userName]);
	linPix += LINPIX;
	p->setPen(QColor(203, 203, 203));
	p->drawLine(0,linPix,wid_,linPix);
    int wspan[8] = {11,11,11, 11, 11, 11, 11, 12};
    int wrats[8] = {0,0,0,       0,0,0,     0,};
    for(int i=1;i<8;i++)
	{
		wrats[i]=wrats[i-1]+wspan[i-1];
	}
    QString otitle[8] = {QString::fromLocal8Bit("报单时间")
						 ,QString::fromLocal8Bit("商品"), QString::fromLocal8Bit("买卖")
						 //,QString::fromLocal8Bit("开平")
						 ,QString::fromLocal8Bit("价格")
						 ,QString::fromLocal8Bit("数量"),
						 QString::fromLocal8Bit("成交") // QString::fromLocal8Bit("剩余")
                         //,QString::fromLocal8Bit("成交价")
                         ,QString::fromLocal8Bit("状态")
						 , QString::fromLocal8Bit("备注")
						};
	int afg = Qt::AlignLeft|Qt::AlignVCenter;
	QFont def = p->font();
	QFont font;
	font.setFamily(QString::fromUtf8("微软雅黑"));
	font.setBold(true);
	p->setFont(font);
    for(int c=0;c<7;c++)
	{
		int c1 = wid_*wrats[c]/100 - vscl;
		int c2 = wid_*wrats[c+1]/100 - vscl;
		p->setPen(QColor(203, 203, 203));
		p->drawLine(c2,linPix-LINPIX,c2,linPix);
		QPen tpen(QColor(34, 34, 34));
		tpen.setWidth(2);
		p->setPen(tpen);
		afg = Qt::AlignLeft|Qt::AlignVCenter;
		if(c>=3 && c<=6)
				afg = Qt::AlignRight|Qt::AlignVCenter;
		p->drawText(QRect(c1+2,linPix-LINPIX,c2-c1-4,LINPIX),afg, otitle[c]);
	}
    p->drawText(QRect(wid_*wrats[7]/100 - vscl+2,linPix-LINPIX,wid_*(100-wrats[7])/100-4,LINPIX),afg, otitle[7]);
	p->setFont(def);
	bool jo = false; // 奇偶行显示 false奇 true偶
	// 订单按时间排序
    QMap<QString, CThostFtdcOrderField *> oLst;// 该账户对应的订单信息 KEY:订单时间
    QMapIterator<QString, CThostFtdcOrderField *> s(ti->orderLst);
	int dis = 0; // 用于区分订单时间不一样的情况
	int showNum = 0;
	while (s.hasNext())
	{
        CThostFtdcOrderField * sOrder = ti->orderLst[s.next().key()];
        if(!sOrder)
            continue;
		if(pQuotApi && tw->insMap_dy[QString::fromLocal8Bit(sOrder->InstrumentID)] == NULL)
		{
			// 绘制前先订阅
            int count = 1;
            char **InstrumentID = new char*[count];
            InstrumentID[0] = sOrder->InstrumentID;
            pQuotApi->SubscribeMarketData(InstrumentID, 1);
			tw->insMap_dy[QString::fromLocal8Bit(sOrder->InstrumentID)] = tw->insMap[QString::fromLocal8Bit(sOrder->InstrumentID)];
            delete[] InstrumentID;
		}
        if(orderStatus && sOrder->OrderStatus != THOST_FTDC_OST_NoTradeQueueing && sOrder->OrderStatus != THOST_FTDC_OST_NotTouched)
			continue;
        QString dt = QString(sOrder->InsertDate).append(QString(sOrder->InsertTime));
		if(!oLst.contains(dt))
		{
			oLst[dt] = sOrder;
		}
		else
		{
			oLst[dt.append(QString::number(dis))] = sOrder;
			dis++;
		}
		showNum++;
	}
	//if(showNum == 0)
	//{
	//	QFont def = p->font();
	//	QFont font;
	//	font.setFamily(QString::fromUtf8("Cambria Math"));
	//	font.setPointSize(30);
	//	p->setFont(font);
	//	p->setPen(QColor(34, 34, 34));
	//	p->drawText(tableRect,Qt::AlignCenter, QString::fromLocal8Bit("暂无订单"));
	//	p->setFont(def);
	//	return;
	//}
    QMapIterator<QString, CThostFtdcOrderField *> op(oLst);
	op.toBack();
	int igrows = wrows>0?wrows:0;
	while (op.hasPrevious() && linPix<height())
	{
        CThostFtdcOrderField * sOrder = oLst[op.previous().key()];
		if(!sOrder) continue;
		if(igrows > 0)
		{
			igrows --;
			continue;
		}
		linPix += LINPIX;
        CThostFtdcInstrumentField * sbInstr = tw->insMap[QString::fromLocal8Bit(sOrder->InstrumentID)];
		QColor tPenc = QColor(66, 124, 211);
		if(!sbInstr)
			continue;
		int scale = tw->getScale(sOrder->InstrumentID);
		if(jo)
		{
			p->fillRect(QRect(0,linPix-LINPIX,wid_,LINPIX),QColor(221,223,225));
		}
		if(marTop+clickLine*LINPIX == linPix)
		{
			if(c_order != sOrder)
				udNum = 0;
			c_order = sOrder;
			tPenc = QColor(193, 40, 44);
			p->fillRect(QRect(0,linPix-LINPIX,wid_,LINPIX),QColor(255,255,255));
		}
		jo = !jo;
		p->setPen(QColor(203, 203, 203));
		p->drawLine(0,linPix,wid_,linPix);
        otitle[0] = QString::fromLocal8Bit(sOrder->InsertTime);/* 报单时间 */
		otitle[1] = QString::fromLocal8Bit(sbInstr->InstrumentName);/* 合约号 */
        otitle[2] = convertBsFlag(sOrder->Direction).append(convertOcFlag(sOrder->CombOffsetFlag[0]));/* 买卖标志 *//* 开平标志 */
        otitle[3] = QString::number(sOrder->LimitPrice,'f',scale);/* 价格 */
        otitle[4] = QString::number(sOrder->VolumeTotalOriginal);/* 数量 */
        otitle[5] = QString::number(sOrder->VolumeTotalOriginal - sOrder->VolumeTotal); /* 成交 */  /* 剩余数量 */
//        otitle[6] = QString::number(sOrder->LimitPrice,'f',scale);   /* 成交均价 */
        otitle[6] = convertOrderStatus(sOrder->OrderStatus);  /* 状态 */
//		otitle[8] = QString::fromLocal8Bit(sOrder->OrderID);  /* 订单号 */
        if( strcmp( sOrder->StatusMsg, "全部成交报单已提交" ) == 0 ){
            if(sOrder->ContingentCondition == THOST_FTDC_CC_Immediately) // 条件方法 为无的时候 限价单 写在备注一栏
            {
                otitle[7] = QString::fromLocal8Bit(LIMITORDER);   /* 限价单 */
            }
            else
            {
                if(sOrder->OrderPriceType == THOST_FTDC_OPT_AnyPrice) // 价格类型为市价
                {
    //				switch(sOrder->FilledPoints)
    //				{
    //				case 1:
    //					otitle[8] = QString::fromLocal8Bit(ONETICK);   /* +1tick */
    //					break;
    //				case 2:
    //					otitle[8] = QString::fromLocal8Bit(TWOTICK);   /* +2tick */
    //					break;
    //				case 3:
    //					otitle[8] = QString::fromLocal8Bit(THREETICK);   /* +3tick */
    //					break;
    //				}
                    otitle[7] = QString::fromLocal8Bit(ONETICK);
                }
                else
                {
                    otitle[7] = QString::fromLocal8Bit(STOPLIMITORDER);   /* 止损限价单 */
                }
            }
        }
        else{
            otitle[7] = QString::fromLocal8Bit(sOrder->StatusMsg);
        }

//		otitle[10] = QString::fromLocal8Bit(sOrder->DetailStatus);  /* 详细状态 */
		int afg = Qt::AlignLeft|Qt::AlignVCenter;
        for(int rc=0;rc<8;rc++)
		{
			int c1 = wid_*wrats[rc]/100 - vscl;
			int c2 = wid_*wrats[rc+1]/100 - vscl;
			p->setPen(QColor(189, 189, 189));
			p->drawLine(c2,linPix-LINPIX,c2,linPix);
			p->setPen(tPenc);
			afg = Qt::AlignLeft|Qt::AlignVCenter;
			if(rc>=3 && rc<=6)
				afg = Qt::AlignRight|Qt::AlignVCenter;
			p->setPen(QColor(0, 0, 0));
			p->drawText(QRect(c1+2,linPix-LINPIX,c2-c1-4,LINPIX), afg, otitle[rc]);
		}
        p->drawText(QRect(wid_*wrats[7]/100 - vscl+2,linPix-LINPIX,width()*(100-wrats[7])/100-4,LINPIX), afg, otitle[7]);
	}
	if(wid_ > width())
	{
		vsBarR = QRect(0,pvh,wid_,16);
		p->fillRect(vsBarR,QColor(119, 119, 119));
		vsBarRt = QRect(v_sc,pvh+1,vbw,14);
		p->fillRect(vsBarRt,QColor(238, 238, 238));
	}
}


// 记录鼠标按下时的状态
void OrderManage::mousePressEvent(QMouseEvent * event)
{
	if (event->button() != Qt::LeftButton) {
		return;
	}
	moveWPixs = event->x();
	moveHPixs = event->y();
	update();
}


// 鼠标移动时的事件处理
void OrderManage::mouseMoveEvent(QMouseEvent * event)
{
	if(moveWPixs < width() && width() < MINWID)
	{
		v_sc += event->pos().x()-moveWPixs;
		moveWPixs = event->pos().x();
		v_sc = v_sc<0?0:v_sc;
		v_sc = v_sc+vsBarRt.width()>width()?width()-vsBarRt.width():v_sc;
	}
	update();
}


// 记录鼠标松开时的状态
void OrderManage::mouseReleaseEvent(QMouseEvent * event)
{
	if(event->button() != Qt::LeftButton)
	{
		return;
	}
	int x = event->pos().x();
	int y = event->pos().y();
	moveWPixs = 9999;
	moveHPixs = 9999;
    CThostFtdcInstrumentField * csi = NULL;
	double minMove = 0.01;
	if(y> 44)
		clickLine = (y-24)/LINPIX+1;
	if(c_order)
	{
		csi = tw->insMap[QString(c_order->InstrumentID)];
		if(csi)
            minMove = csi->PriceTick;
	}
	//if(changeRect.contains(event->pos()))
	//{
	//	if(c_order && csi && c_order->BSFlag != BCESConstBSFlagExecute)
	//	{
	//		updateOrder(c_order, c_order->Price+udNum);
	//		udNum = 0;
	//	}
	//}
	//if(upriceRect.contains(event->pos()) && upriceRect.contains(QPoint(x,y+40))) // 加
	//{
	//	udNum += minMove;
	//}
	//if(upriceRect.contains(event->pos()) && upriceRect.contains(QPoint(x,y-40))) // 减
	//{
	//	udNum -= minMove;
	//}
    if(upOneRect.contains(event->pos()) && c_order) // 调高
	{
        updateOrder(c_order, c_order->LimitPrice+minMove);
	}
    if(dnOneRect.contains(event->pos()) && c_order) // 调低
	{
        updateOrder(c_order, c_order->LimitPrice-minMove);
	}
	if(cancelRect.contains(event->pos()))
	{
		dropOrder(c_order);
	}
	//if(checkRect.contains(event->pos()))
	//{
	//	orderStatus = !orderStatus;
	//}
	update();
}

// 窗口双击事件
void OrderManage::mouseDoubleClickEvent(QMouseEvent * event)
{
	if(!c_order) return;
	if(!tw->CSubmit) return;
	tw->CSubmit->selectInstr->curInstr = tw->insMap[QString::fromLocal8Bit(c_order->InstrumentID)];
	tw->CSubmit->initByInstr();
	tw->CSubmit->update();
}

// 处理鼠标滚轮事件
void OrderManage::wheelEvent(QWheelEvent * event)
{
	if(event->delta() > 0)
	{
		if(wrows>0)
		{
			wrows--;
			clickLine ++;
		}
	}
	else if(event->delta() < 0)
	{
		clickLine --;
		wrows++;
	}
	update();
}

// 修改订单
void OrderManage::updateOrder(CThostFtdcOrderField * co, double price2)
{
	if(!co)
		return;
    CThostFtdcInstrumentField * ci = tw->insMap[co->InstrumentID];
	if(!ci)
		return;
	TradeInfo * ti = &(tradeInfoLst[loginW->userName]);
    if(co->ContingentCondition == THOST_FTDC_CC_Immediately && (co->OrderStatus == THOST_FTDC_OST_NoTradeQueueing || co->OrderStatus == THOST_FTDC_OST_PartTradedQueueing))
	{
		NEWORDERINF noi;
		::memset(&noi,0,sizeof(NEWORDERINF));
        sprintf(noi.tif, "%s", loginW->userName);
        strncpy(noi.OrderRef,co->OrderRef,sizeof(noi.OrderRef)); /* 订单号 */
		strncpy(noi.InvestorID,co->InvestorID,sizeof(noi.InvestorID)); /* 投资者号 */
		strncpy(noi.InstrumentID,co->InstrumentID,sizeof(noi.InstrumentID)); /* 合约号 */
        noi.Direction = co->Direction; /* 买卖标志 */
        noi.CombOffsetFlag[0] = co->CombOffsetFlag[0]; /* 开平标志 */
        noi.PriceType = co->OrderPriceType;	/* 价格类型 */
		noi.Price = price2;	/* 价格 */
        noi.ConditionMethod = co->ContingentCondition;	/* 条件方法 */
        noi.Qty = co->VolumeTotal;	/* 剩余数量 */
		strncpy(noi.ExchangeID, co->ExchangeID,sizeof(noi.ExchangeID));
		tw->o2upLst.append(noi);
		int nRequestID = CreateNewRequestID();
        CThostFtdcInputOrderActionField pCancelReq;
        ::memset(&pCancelReq,0,sizeof(CThostFtdcInputOrderActionField));
        strncpy(pCancelReq.OrderRef,co->OrderRef,sizeof(pCancelReq.OrderRef)); /* 订单号 */
		strncpy(pCancelReq.InvestorID,co->InvestorID,sizeof(pCancelReq.InvestorID)); /* 投资者号 */
		strncpy(pCancelReq.InstrumentID,co->InstrumentID,sizeof(pCancelReq.InstrumentID)); /* 合约号 */
		strncpy(pCancelReq.ExchangeID,ci->ExchangeID,sizeof(pCancelReq.ExchangeID)); /* 交易所号 */
        ti->api->ReqOrderAction(&pCancelReq, nRequestID); // 撤销订单
	}
    if((co->OrderPriceType == THOST_FTDC_OPT_AnyPrice || co->OrderPriceType == THOST_FTDC_OPT_LimitPrice)&& co->OrderStatus == THOST_FTDC_OST_NotTouched)  // 止损单
	{
		int scale = tw->getScale(co->InstrumentID);
		// 更新虚拟本地订单价格
        co->LimitPrice = price2;
	}
	tw->CSubmit->update();
}

// 删除订单
void OrderManage::dropOrder(CThostFtdcOrderField * co)
{
	if(!co)
		return;
    CThostFtdcInstrumentField * ci = tw->insMap[co->InstrumentID];
	if(!ci)
		return;
	TradeInfo & ti = tradeInfoLst[loginW->userName];
    if(co->OrderStatus == THOST_FTDC_OST_NoTradeQueueing || co->OrderStatus == THOST_FTDC_OST_PartTradedQueueing) // 撤销订单
	{
		int nRequestID = CreateNewRequestID();
        CThostFtdcInputOrderActionField pCancelReq;
        ::memset(&pCancelReq,0,sizeof(CThostFtdcInputOrderActionField));
        strncpy(pCancelReq.OrderRef,co->OrderRef,sizeof(pCancelReq.OrderRef)); /* 订单号 */
		strncpy(pCancelReq.InvestorID,co->InvestorID,sizeof(pCancelReq.InvestorID)); /* 投资者号 */
		strncpy(pCancelReq.InstrumentID,co->InstrumentID,sizeof(pCancelReq.InstrumentID)); /* 合约号 */
		strncpy(pCancelReq.ExchangeID,ci->ExchangeID,sizeof(pCancelReq.ExchangeID)); /* 交易所号 */
        ti.api->ReqOrderAction(&pCancelReq, nRequestID);
        //if(ti.api->ReqOrderAction(&pCancelReq, nRequestID) == 0)
		//{
		//	QMessageBox::about(NULL,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("已发送订单撤销请求！"));
		//}
		//else
		//{
		//	QMessageBox::about(NULL,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("订单撤销请求失败！"));
		//}
	}
    if(co->OrderStatus == THOST_FTDC_OST_NotTouched) // 撤销止损单
	{
        co->OrderStatus = THOST_FTDC_OST_Canceled;
        tw->checkOcoStatus(ti, co->OrderRef);
	}
}
