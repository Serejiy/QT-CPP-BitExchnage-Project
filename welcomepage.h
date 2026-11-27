#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class welcomepage;
}
QT_END_NAMESPACE

// First window: choose Login or Registration.
// A QMainWindow, so the status bar with the database path sits at the bottom.
class welcomepage : public QMainWindow
{
    Q_OBJECT

public:
    welcomepage(QWidget *parent = nullptr);
    ~welcomepage();

private slots:
    void on_login_clicked();
    void on_registration_clicked();

private:
    Ui::welcomepage *ui;
};

#endif // WELCOMEPAGE_H
