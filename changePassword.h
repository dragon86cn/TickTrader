#ifndef CHANGEPASSWORD_H
#define CHANGEPASSWORD_H
#include <QWidget>
#include "ui_changePassword.h"
#include "loginWin.h"

class TradeWidget;
class changePassword : public QWidget
{
	Q_OBJECT

public:
    changePassword(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~changePassword();
    char oldPassword[21];
    char newPassword[21];
	char verifyPassword[21];


private:
	Ui::changePassword ui;

protected:

public slots:
    void changePasswordAction();
    void cancelAction();
	void changePasswordEvent();
	
};

#endif // CHANGEPASSWORD_H
