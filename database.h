#ifndef DATABASE_H
#define DATABASE_H

#include <QMap>
#include <QString>
#include <QStringList>

// All SQL of the app lives here: one SQLite connection, prepared statements only.
namespace Database {

// Where the SQLite file is stored, e.g. C:/Users/<name>/AppData/Roaming/Serejiy/BitExchange/bitexchange.db
QString filePath();

// Opens the database. On the first start the file and the "users" table are created.
bool open(QString *error);

// The currencies that have a balance column in the "users" table.
QStringList currencies();

// Registration: stores the user with a password hash and a start balance of 1000 USDT.
bool createUser(const QString &username, const QString &password, QString *error);

// Login: true if the user exists and the password matches the stored hash.
bool checkPassword(const QString &username, const QString &password);

// Balances of one user, e.g. {"USDT": 1000.0, "BTC": 0.0, ...}
QMap<QString, double> balances(const QString &username);

// Simulated market order at the current price.
// quantity > 0 buys the coin with USDT, quantity < 0 sells it for USDT.
bool trade(const QString &username, const QString &coin, double quantity, double price,
           QString *error);

} // namespace Database

#endif // DATABASE_H
