#include <QApplication>
#include <QMessageBox>

#include "database.h"
#include "welcomepage.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Used for the database folder, e.g. AppData/Roaming/Serejiy/BitExchange on Windows
    QApplication::setOrganizationName("Serejiy");
    QApplication::setApplicationName("BitExchange");

    QString error;
    if (!Database::open(&error)) {
        QMessageBox::critical(nullptr, "BitExchange", "Database error: " + error);
        return 1;
    }

    auto *window = new welcomepage();
    window->show();
    return app.exec();
}
