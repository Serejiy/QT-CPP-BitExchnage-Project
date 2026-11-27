#include "welcomepage.h"
#include "ui_welcomepage.h"

#include "database.h"
#include "login.h"
#include "registration.h"

welcomepage::welcomepage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::welcomepage)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->statusbar->showMessage("Database: " + Database::filePath());
}

welcomepage::~welcomepage()
{
    delete ui;
}

void welcomepage::on_login_clicked()
{
    auto *loginPage = new login();
    loginPage->show();
    close();
}

void welcomepage::on_registration_clicked()
{
    auto *registrationPage = new registration();
    registrationPage->show();
    close();
}
