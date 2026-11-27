#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class login;
}
QT_END_NAMESPACE

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private slots:
    void on_loginBtn_clicked();     // "Login"
    void on_pushButton_2_clicked(); // "Back"
    void on_pushButton_3_clicked(); // "Clear"

private:
    Ui::login *ui;
};

#endif // LOGIN_H
