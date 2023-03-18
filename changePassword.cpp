#include "changePassword.h"
#include "coolsubmit.h"
#include "tradewidget.h"

// 修改密码界面
extern changePassword *cpView;
extern TradeWidget *g_tw;
// 构建登录窗口
extern loginWin * loginW;
extern quint32 CreateNewRequestID();

changePassword::changePassword(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
	ui.setupUi(this);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/image/images/pass.png"), QSize(), QIcon::Normal, QIcon::Off);
	this->setWindowIcon(icon);
	// 关闭时自动释放
	setAttribute(Qt::WA_DeleteOnClose, true);
	connect(ui.finishButton, SIGNAL(clicked()),this, SLOT(changePasswordAction()));
	connect(ui.cancelButton, SIGNAL(clicked()),this, SLOT(cancelAction()));
}

changePassword::~changePassword()
{
	cpView = NULL;
}

void changePassword::changePasswordAction() {
	// 更改密码
	strncpy(oldPassword, ui.oldPasswordEdit->text().toLatin1().data(),sizeof(oldPassword));
	strncpy(newPassword, ui.newPasswordEdit->text().toLatin1().data(),sizeof(newPassword));
	strncpy(verifyPassword, ui.verifyPasswordEdit->text().toLatin1().data(),sizeof(verifyPassword));
	if (ui.oldPasswordEdit->text().isEmpty() || ui.newPasswordEdit->text().isEmpty() || ui.verifyPasswordEdit->text().isEmpty()) {
		//QMessageBox::about(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("密码栏不能为空！"));
		Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("KDJ密码栏不能为空！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
		nt.exec();
		return;
	}

	if (!strcmp(oldPassword, newPassword)) {
		//QMessageBox::about(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("不能与原密码相同！"));
		Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("不能与原密码相同！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
		nt.exec();
		return;
	} else if (strcmp(newPassword, verifyPassword)) {
		//QMessageBox::about(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("两次键入的新密码不一致！"));
		Notice nt(Notice::NOTICE_TYPE_NOTIFICATION, QString::fromLocal8Bit("两次键入的新密码不一致！"), false, QString::fromLocal8Bit("提示"), NULL, 0);
		nt.exec();
		return;
	} else {
		changePasswordEvent();
		this->close();
	}
}

void changePassword::cancelAction() {
	// 取消按钮 关闭窗口
	this->close();
}

// 更改密码请求事件
void changePassword::changePasswordEvent() {
	TradeInfo & ti = tradeInfoLst[loginW->userName];
    CThostFtdcUserPasswordUpdateField pChangeOrder;
    ::memset(&pChangeOrder, 0,sizeof(CThostFtdcUserPasswordUpdateField));
	int nRequestID = CreateNewRequestID();
    strncpy(pChangeOrder.BrokerID,loginW->m_users.BrokerID, sizeof(pChangeOrder.BrokerID));
    strncpy(pChangeOrder.UserID,loginW->userName, sizeof(pChangeOrder.UserID));
    strncpy(pChangeOrder.OldPassword, oldPassword, sizeof(pChangeOrder.OldPassword));
    strncpy(pChangeOrder.NewPassword, newPassword,sizeof(pChangeOrder.NewPassword));
    ti.api->ReqUserPasswordUpdate(&pChangeOrder, nRequestID);
	//if(ti.api->ChangePasswordReq(&pChangeOrder, nRequestID) == 0)
	//{
	//	QMessageBox::about(NULL,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("已发送密码修改请求！"));
	//}
	//else
	//{
	//	QMessageBox::about(NULL,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("密码修改请求失败！"));
	//}
}
