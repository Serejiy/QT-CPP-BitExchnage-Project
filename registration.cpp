#include "registration.h"
#include "ui_registration.h"

#include <QMessageBox>

#include "database.h"
#include "login.h"
#include "welcomepage.h"

registration::registration(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::registration)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->txtPass->setEchoMode(QLineEdit::Password);
}

registration::~registration()
{
    delete ui;
}

void registration::on_regButton_clicked()
{
    QString error;
    if (!Database::createUser(ui->txtUser->text(), ui->txtPass->text(), &error)) {
        QMessageBox::warning(this, "Registration", error);
        return;
    }

    QMessageBox::information(this, "Registration",
                             "Account created. You start with 1000 USDT of play money.");
    auto *loginPage = new login();
    loginPage->show();
    close();
}

void registration::on_clearButton_clicked()
{
    ui->txtPass->clear();
    ui->txtUser->clear();
}

void registration::on_backButton_clicked()
{
    auto *welcomePage = new welcomepage();
    welcomePage->show();
    close();
}
