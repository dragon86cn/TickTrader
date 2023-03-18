#include "OrderView.h"
#include "tradewidget.h"
#include "InstrManage.h"
#include "configWidget.h"
#include "notice.h"
#include <QPainterPath>

#define LINPIX 30
// 构建登录窗口
extern loginWin * loginW;
// 设置界面
extern configWidget * cview;
extern quint32 CreateNewRequestID();

OrderView::OrderView(TradeWidget * tWidget, QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f)
{
	tw = tWidget;
	QIcon icon;
    icon.addFile(QString::fromUtf8(":/image/images/trade.png"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(icon);
	setAttribute(Qt::WA_TranslucentBackground);
	this->setWindowTitle(QString::fromLocal8Bit("下单"));
	udPrice = 0;
	udLot = 1;
	selectInstr = new InstrManage(NULL, this);
	connect(selectInstr, SIGNAL(changed()), this, SLOT(update()));
	selectInstr->hide();
	this->setMinimumSize(QSize(360, 170));
	this->setMaximumSize(QSize(360, 170));
    ::memset(&orQuot,0,sizeof(CThostFtdcDepthMarketDataField));
	// 关闭时自动释放
	setAttribute(Qt::WA_DeleteOnClose, false);
}

OrderView::~OrderView()
{

}

void OrderView::paintEvent(QPaintEvent * event) {

	QPainter painter(this);
    CThostFtdcInstrumentField * ci = selectInstr->curInstr;
	if(!ci) return;
    double minMove = ci->PriceTick;
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QColor(34, 34, 34)); 
	painter.fillRect(QRect(0, 0, width(), height()), QColor(240, 240, 240));
	QFont def = painter.font();
	QFont font;
	//font.setFamily(QString::fromLocal8Bit("微软雅黑"));
	//qApp->setFont(font);		// font设置为默认字体

	// 调整价位区域
	changePriceRect = QRect(5, 5, 4*LINPIX, 3*LINPIX);
	QLinearGradient linearGradient(5, 5, 5, 3*LINPIX+5);
	linearGradient.setStart(5, 5);
	linearGradient.setFinalStop(5, 3*LINPIX+5);
	linearGradient.setColorAt(0,QColor(128, 128, 128));
	linearGradient.setColorAt(1,QColor(84, 84, 84));
	painter.setBrush(QBrush(linearGradient));
	painter.drawRoundedRect(changePriceRect, 5, 5);
	painter.fillRect(QRect(6, LINPIX+5, 4*LINPIX-1, LINPIX), QColor(240, 240, 240));
	QPainterPath upPriceP;
	upPriceP.moveTo(65,10);
	upPriceP.lineTo(95,20);
	upPriceP.lineTo(75,20);
	upPriceP.lineTo(75,30);
	upPriceP.lineTo(55,30);
	upPriceP.lineTo(55,20);
	upPriceP.lineTo(35,20);
	upPriceP.lineTo(65,10);
	painter.fillPath(upPriceP, QColor(240, 240, 240));
	QPainterPath dnPriceP;
	dnPriceP.moveTo(65,90);
	dnPriceP.lineTo(95,80);
	dnPriceP.lineTo(75,80);
	dnPriceP.lineTo(75,70);
	dnPriceP.lineTo(55,70);
	dnPriceP.lineTo(55,80);
	dnPriceP.lineTo(35,80);
	dnPriceP.lineTo(65,90);
	painter.fillPath(dnPriceP,QColor(240, 240, 240));
	if(ci && tw->quotMap[ci->InstrumentID])
	{
		int scale = tw->getScale(ci->InstrumentID);
		if(::strcmp(orQuot.InstrumentID, ci->InstrumentID) != 0)
		{
            ::memcpy(&orQuot, tw->quotMap[ci->InstrumentID], sizeof(CThostFtdcDepthMarketDataField));
		}
        painter.drawText(QRect(6, LINPIX+5, 4*LINPIX-1, LINPIX),Qt::AlignCenter, QString::number(orQuot.LastPrice+udPrice*minMove,'f',scale));
	}

	// 调整手数区域
	changeLotRect = QRect(135, 5, 4*LINPIX, 3*LINPIX);
	linearGradient.setStart(135, 5);
	linearGradient.setFinalStop(135, 3*LINPIX+5);
	linearGradient.setColorAt(0,QColor(128, 128, 128));
	linearGradient.setColorAt(1,QColor(84, 84, 84));
	painter.setBrush(QBrush(linearGradient));
	painter.drawRoundedRect(changeLotRect, 5, 5);
	painter.fillRect(QRect(136, LINPIX+5, 4*LINPIX-1, LINPIX), QColor(240, 240, 240));
	QPainterPath upLotP;
	upLotP.moveTo(195,10);
	upLotP.lineTo(225,20);
	upLotP.lineTo(205,20);
	upLotP.lineTo(205,30);
	upLotP.lineTo(185,30);
	upLotP.lineTo(185,20);
	upLotP.lineTo(165,20);
	upLotP.lineTo(195,10);
	painter.fillPath(upLotP, QColor(240, 240, 240));
	QPainterPath dnLotP;
	dnLotP.moveTo(195,90);
	dnLotP.lineTo(225,80);
	dnLotP.lineTo(205,80);
	dnLotP.lineTo(205,70);
	dnLotP.lineTo(185,70);
	dnLotP.lineTo(185,80);
	dnLotP.lineTo(165,80);
	dnLotP.lineTo(195,90);
	painter.fillPath(dnLotP,QColor(240, 240, 240));
	// 下单手数显示
	painter.drawText(QRect(136, LINPIX+5, 4*LINPIX-1, LINPIX),Qt::AlignCenter, QString::number(udLot));

	// Buy按钮区域
	buyRect = QRect(5, 3*LINPIX+15, 5*LINPIX+20, 2*LINPIX);
	linearGradient.setStart(5, 3*LINPIX+15);
	linearGradient.setFinalStop(5, 5*LINPIX+15);
	linearGradient.setColorAt(0,QColor(13, 150, 214));
	linearGradient.setColorAt(1,QColor(10, 120, 171));
	painter.setBrush(QBrush(linearGradient));
	painter.drawRoundedRect(buyRect, 5, 5);

	font.setPointSize(20);
	painter.setFont(font);
	painter.drawText(buyRect,Qt::AlignCenter, QString::fromLocal8Bit("买"));

	// Sell按钮区域
	sellRect = QRect(5*LINPIX+35, 3*LINPIX+15, 5*LINPIX+20, 2*LINPIX);
	linearGradient.setStart(5*LINPIX+35, 3*LINPIX+15);
	linearGradient.setFinalStop(5*LINPIX+35, 5*LINPIX+15);
	linearGradient.setColorAt(0,QColor(210, 54, 54));
	linearGradient.setColorAt(1,QColor(178, 40, 40));
	painter.setBrush(QBrush(linearGradient));
	painter.drawRoundedRect(sellRect, 5, 5);
	painter.drawText(sellRect,Qt::AlignCenter, QString::fromLocal8Bit("卖"));
	painter.setFont(def); // 重置字体

	// 批量手数区域
	batchLotRect = QRect(8*LINPIX+25, LINPIX+10, 3*LINPIX, 2*LINPIX-5);
	linearGradient.setStart(8*LINPIX+25, LINPIX+10);
	linearGradient.setFinalStop(8*LINPIX+25, 3*LINPIX+5);
	linearGradient.setColorAt(0,QColor(128, 128, 128));
	linearGradient.setColorAt(1,QColor(84, 84, 84));
	painter.setBrush(QBrush(linearGradient));
	painter.drawRoundedRect(batchLotRect, 3, 3);
	painter.setPen(QColor(34, 34, 34));
	painter.drawLine(8*LINPIX+25, LINPIX+29, 11*LINPIX+25, LINPIX+29);//横线1
	painter.drawLine(8*LINPIX+25, 2*LINPIX+18, 11*LINPIX+25, 2*LINPIX+18);//横线2
	painter.drawLine(10*LINPIX+10, LINPIX+29, 10*LINPIX+10, 2*LINPIX+18);//竖线
	painter.drawLine(9*LINPIX+25, LINPIX+10, 9*LINPIX+25, LINPIX+29);
	painter.drawLine(10*LINPIX+25, LINPIX+10, 10*LINPIX+25, LINPIX+29);
	//快捷手数样式
	painter.setPen(QColor(240, 240, 240));
	painter.drawText(QRect(8*LINPIX+25, LINPIX+10, LINPIX, 18), Qt::AlignCenter, QString::fromLocal8Bit("1"));//5
	painter.drawText(QRect(9*LINPIX+25, LINPIX+10, LINPIX, 18), Qt::AlignCenter, QString::fromLocal8Bit("5"));//10
	painter.drawText(QRect(10*LINPIX+25, LINPIX+10, LINPIX, 18), Qt::AlignCenter, QString::fromLocal8Bit("10"));//50
	painter.drawText(QRect(8*LINPIX+25, LINPIX+28, LINPIX+15, 18), Qt::AlignCenter, QString::fromLocal8Bit("50"));//100
	painter.drawText(QRect(10*LINPIX+10, LINPIX+28, LINPIX+15, 18), Qt::AlignCenter, QString::fromLocal8Bit("100"));//500
	painter.drawText(QRect(8*LINPIX+25, 2*LINPIX+18, 3*LINPIX, 19), Qt::AlignCenter, QString::fromLocal8Bit("500"));//clear
	
	// 合约输入区域
	insRect = QRect(8*LINPIX+25, 5, 3*LINPIX, LINPIX);
	painter.setPen(QColor(34, 34, 34));
	linearGradient.setStart(8*LINPIX+25, 5);
	linearGradient.setFinalStop(8*LINPIX+25, LINPIX);
	linearGradient.setColorAt(0,QColor(128, 128, 128));
	linearGradient.setColorAt(1,QColor(84, 84, 84));
	painter.setBrush(QBrush(linearGradient));
	painter.drawRoundedRect(insRect, 2, 2);
	painter.setPen(QColor(240, 240, 240));
	selectInstr->setGeometry(8*LINPIX+25, 5, 3*LINPIX, 5*LINPIX);
	if(selectInstr->curInstr)
	{
		painter.drawText(insRect, Qt::AlignCenter, selectInstr->curInstr->InstrumentID);
	}
}


// 键盘事件
void OrderView::keyPressEvent( QKeyEvent * event )
{
	if(event->key() >= 0x30 && event->key() <= 0x39) // 0--9
	{
		selectInstr->init(&tw->insMap);
		QString text = selectInstr->searchBox->text();
		selectInstr->searchBox->setText(text.append(event->key()));
	}
	else if(event->key() >= 0x41 && event->key() <= 0x5A) //a--z
	{
		selectInstr->init(&tw->insMap);
		QString text = selectInstr->searchBox->text();
		selectInstr->searchBox->setText(text.append(event->key()));
	}
}

// 鼠标移动时的事件处理
void OrderView::mouseMoveEvent(QMouseEvent * event) {
	update();
}

// 记录鼠标松开时的状态
void OrderView::mouseReleaseEvent(QMouseEvent * event) {
	int x = event->pos().x();
	int y = event->pos().y();
    CThostFtdcInstrumentField * ci = selectInstr->curInstr;
	if(!ci) return;
    double minMove = ci->PriceTick;
    double price = orQuot.LastPrice+udPrice*minMove;
	int pScale = 2;
	if(minMove > 0.95)
		pScale = 0;
	if(minMove > 0.095 && minMove < 0.95)
		pScale = 1;
	if(minMove > 0.0095 && minMove < 0.095)
		pScale = 2;
	if(minMove > 0.00095 && minMove < 0.0095)
		pScale = 3;
	if(changePriceRect.contains(event->pos()) && changePriceRect.contains(QPoint(x,y+60))) // 提高价位
	{
		udPrice += 1;
	}
	if(changePriceRect.contains(event->pos()) && changePriceRect.contains(QPoint(x,y-60))) // 降低价位
	{
		udPrice -= 1;
	}
	if(changeLotRect.contains(event->pos()) && changeLotRect.contains(QPoint(x,y+60))) // 增加手数
	{
		udLot++;
	}
	if(changeLotRect.contains(event->pos()) && changeLotRect.contains(QPoint(x,y-60))) // 减少手数
	{
		udLot --;
		udLot = udLot <1 ? 1 :udLot;
	}
	if(buyRect.contains(event->pos())) // Buy
	{
		if(price < 0.001)
		{
			// 暂且行情
			Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("缺少行情！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
			nt.exec();
			return;
		}
		if(!udLot)
		{
			// 数量不能为0
			Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("数量不能为0！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
			nt.exec();
			return;
		}
		int nRequestID = CreateNewRequestID();
        CThostFtdcInputOrderField pInputOrder;
        ::memset(&pInputOrder,0,sizeof(CThostFtdcInputOrderField));
        strncpy(pInputOrder.BrokerID, loginW->m_users.BrokerID, sizeof(pInputOrder.BrokerID));
		::strcpy(pInputOrder.InvestorID, loginW->userName);     /* 投资者号 */
		::strcpy(pInputOrder.InstrumentID, selectInstr->curInstr->InstrumentID);     /* 合约号 */
		::strcpy(pInputOrder.ExchangeID, selectInstr->curInstr->ExchangeID);   /* 交易所号 */
        sprintf( pInputOrder.OrderRef, "%12i", nRequestID);
        pInputOrder.Direction = THOST_FTDC_D_Buy; /* 买卖标志 */
        pInputOrder.LimitPrice = price;	/* 价格 */
        pInputOrder.VolumeTotalOriginal = udLot;	/* 数量 */
        pInputOrder.CombHedgeFlag[0] = THOST_FTDC_BHF_Speculation;
        pInputOrder.TimeCondition = THOST_FTDC_TC_GFD;
        pInputOrder.VolumeCondition = THOST_FTDC_VC_AV;
        pInputOrder.MinVolume = 1;
        pInputOrder.CombHedgeFlag[0] = THOST_FTDC_BHF_Speculation;
        pInputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Open; // 组合开平标志
        pInputOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;   /* 限价 */
        pInputOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
        pInputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;	/* 限价单模式 */
		TradeInfo & ti = tradeInfoLst[loginW->userName];
        if(ti.api->ReqOrderInsert(&pInputOrder, nRequestID) == 0) // 录入订单
		{
			Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("订单已提交！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
			nt.exec();
		}
		else
		{
			Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("订单提交失败！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
			nt.exec();
		}
	}
	if(sellRect.contains(event->pos())) // Sell
	{
		if(price < 0.001)
		{
			// 暂且行情
			Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("缺少行情！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
			nt.exec();
			return;
		}
		if(!udLot)
		{
			// 数量不能为0
			Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("数量不能为0！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
			nt.exec();
			return;
		}
		int nRequestID = CreateNewRequestID();
        CThostFtdcInputOrderField pInputOrder;
        ::memset(&pInputOrder,0,sizeof(CThostFtdcInputOrderField));
        strncpy(pInputOrder.BrokerID, loginW->m_users.BrokerID, sizeof(pInputOrder.BrokerID));
		::strcpy(pInputOrder.InvestorID, loginW->userName);     /* 投资者号 */
		::strcpy(pInputOrder.InstrumentID, selectInstr->curInstr->InstrumentID);     /* 合约号 */
		::strcpy(pInputOrder.ExchangeID, selectInstr->curInstr->ExchangeID);   /* 交易所号 */
        sprintf( pInputOrder.OrderRef, "%12i", nRequestID);
        pInputOrder.Direction = THOST_FTDC_D_Sell; /* 买卖标志 */
        pInputOrder.LimitPrice = price;	/* 价格 */
        pInputOrder.VolumeTotalOriginal = udLot;	/* 数量 */
        pInputOrder.CombHedgeFlag[0] = THOST_FTDC_BHF_Speculation;
        pInputOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Open; // 组合开平标志
        pInputOrder.TimeCondition = THOST_FTDC_TC_GFD;
        pInputOrder.VolumeCondition = THOST_FTDC_VC_AV;
        pInputOrder.MinVolume = 1;
        pInputOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
        pInputOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;   /* 限价 */
        pInputOrder.ContingentCondition = THOST_FTDC_CC_Immediately;	/* 限价单模式 */
		TradeInfo & ti = tradeInfoLst[loginW->userName];
        if(ti.api->ReqOrderInsert(&pInputOrder, nRequestID) == 0) // 录入订单
		{
			Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("订单已提交！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
			nt.exec();
		}
		else
		{
			Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("订单提交失败！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
			nt.exec();
		}
	}
	//批量手数的判断
	if(batchLotRect.contains(event->pos()))
	{
		int disx=x-batchLotRect.x();
		int disy=y-batchLotRect.y();
		if(disx <30 && disy < 18) // 5
		{
			udLot = 1;
		}
		if(disx > 30 && disx < 60 && disy < 18) // 10
		{
			udLot = 5;
		}
		if(disx > 60 && disx < 90 && disy < 18) // 50
		{
			udLot = 10;
		}
		if(disx < 45 && disy >18 && disy < 36) // 100
		{
			udLot = 50;
		}
		if(disx < 90 && disx > 60 && disy >18 && disy < 36) // 100
		{
			udLot = 100;
		}
		if(disy > 36) // clear
		{
			udLot = 500;
		}
	}
	if(insRect.contains(event->pos()))
	{
		selectInstr->setGeometry(insRect.x(), insRect.y(), insRect.width(), 5*LINPIX);
		selectInstr->show();
	}
	update();
}
