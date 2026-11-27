#include "login.h"
#include "ui_login.h"

#include <QMessageBox>

#include "database.h"
#include "mainwindow.h"
#include "welcomepage.h"

login::login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);             // free the window when it is closed
    ui->txtPass->setEchoMode(QLineEdit::Password);  // show dots instead of the password
    connect(ui->txtPass, &QLineEdit::returnPressed, this, &login::on_loginBtn_clicked);
}

login::~login()
{
    delete ui;
}

void login::on_loginBtn_clicked()
{
    const QString username = ui->txtUser->text().trimmed();
    const QString password = ui->txtPass->text();

    if (!Database::checkPassword(username, password)) {
        QMessageBox::information(this, "Login", "Wrong username or password.");
        return;
    }

    auto *mainWindow = new MainWindow(username);
    mainWindow->show();
    close();
}

void login::on_pushButton_2_clicked()
{
    auto *welcomePage = new welcomepage();
    welcomePage->show();
    close();
}

void login::on_pushButton_3_clicked()
{
    ui->txtPass->clear();
    ui->txtUser->clear();
}
