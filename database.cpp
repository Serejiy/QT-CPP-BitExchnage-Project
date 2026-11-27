#include "database.h"

#include <QCryptographicHash>
#include <QDir>
#include <QPasswordDigestor>
#include <QRandomGenerator>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

namespace {

const int kIterations = 100000;   // PBKDF2 rounds: makes guessing passwords slow
const double kStartBalance = 1000; // every new user starts with 1000 USDT

QByteArray deriveKey(const QString &password, const QByteArray &salt, int iterations)
{
    return QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256, password.toUtf8(),
                                              salt, iterations, 32);
}

// Stored format: pbkdf2_sha256$<iterations>$<salt hex>$<hash hex>
QString hashPassword(const QString &password)
{
    QByteArray salt;
    for (int i = 0; i < 4; ++i) {          // 16 random bytes
        quint32 value = QRandomGenerator::system()->generate();
        salt.append(reinterpret_cast<const char *>(&value), sizeof(value));
    }
    return QString("pbkdf2_sha256$%1$%2$%3")
        .arg(kIterations)
        .arg(QString::fromLatin1(salt.toHex()))
        .arg(QString::fromLatin1(deriveKey(password, salt, kIterations).toHex()));
}

bool verifyPassword(const QString &password, const QString &stored)
{
    const QStringList parts = stored.split('$');
    if (parts.size() != 4 || parts.at(0) != "pbkdf2_sha256")
        return false;
    const int iterations = parts.at(1).toInt();
    const QByteArray salt = QByteArray::fromHex(parts.at(2).toLatin1());
    const QByteArray expected = QByteArray::fromHex(parts.at(3).toLatin1());
    return iterations > 0 && deriveKey(password, salt, iterations) == expected;
}

} // namespace

namespace Database {

QString filePath()
{
    const QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(folder);
    return folder + "/bitexchange.db";
}

QStringList currencies()
{
    return {"USDT", "BTC", "ETH", "BNB", "PEPE", "DOGE"};
}

bool open(QString *error)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filePath());
    if (!db.open()) {
        *error = db.lastError().text();
        return false;
    }

    QSqlQuery query;
    const bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        " ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        " Username TEXT NOT NULL UNIQUE,"
        " Password TEXT NOT NULL,"
        " Role TEXT,"
        " USDT REAL NOT NULL DEFAULT 0,"
        " BTC REAL NOT NULL DEFAULT 0,"
        " ETH REAL NOT NULL DEFAULT 0,"
        " BNB REAL NOT NULL DEFAULT 0,"
        " PEPE REAL NOT NULL DEFAULT 0,"
        " DOGE REAL NOT NULL DEFAULT 0)");
    if (!ok)
        *error = query.lastError().text();
    return ok;
}

bool createUser(const QString &username, const QString &password, QString *error)
{
    if (username.trimmed().length() < 3 || password.length() < 4) {
        *error = "Username needs at least 3 characters, password at least 4.";
        return false;
    }

    QSqlQuery query;
    // ":username" etc. are placeholders. The values are sent separately, so text
    // from the user can never change the SQL command itself (no SQL injection).
    query.prepare("INSERT INTO users (Username, Password, Role, USDT) "
                  "VALUES (:username, :password, 'user', :usdt)");
    query.bindValue(":username", username.trimmed());
    query.bindValue(":password", hashPassword(password));
    query.bindValue(":usdt", kStartBalance);

    if (!query.exec()) {
        // The UNIQUE constraint on Username fails if the name is taken
        *error = "This username is already taken.";
        return false;
    }
    return true;
}

bool checkPassword(const QString &username, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT Password FROM users WHERE Username = :username");
    query.bindValue(":username", username.trimmed());
    if (!query.exec() || !query.next())
        return false;
    return verifyPassword(password, query.value(0).toString());
}

QMap<QString, double> balances(const QString &username)
{
    QMap<QString, double> result;
    QSqlQuery query;
    query.prepare("SELECT USDT, BTC, ETH, BNB, PEPE, DOGE FROM users WHERE Username = :username");
    query.bindValue(":username", username);
    if (query.exec() && query.next()) {
        const QStringList names = currencies();
        for (int i = 0; i < names.size(); ++i)
            result[names.at(i)] = query.value(i).toDouble();
    }
    return result;
}

bool trade(const QString &username, const QString &coin, double quantity, double price,
           QString *error)
{
    // A column name cannot be a placeholder, so only names from our own list are allowed
    if (coin == "USDT" || !currencies().contains(coin)) {
        *error = "Unknown currency.";
        return false;
    }
    if (quantity == 0 || price <= 0) {
        *error = "Please enter a quantity and wait for the current price.";
        return false;
    }

    // Check and update in one transaction: either everything is saved or nothing
    QSqlDatabase db = QSqlDatabase::database();
    db.transaction();

    const QMap<QString, double> current = balances(username);
    const double cost = quantity * price;          // negative when selling
    if (quantity > 0 && current.value("USDT") + 1e-9 < cost) {
        db.rollback();
        *error = "Not enough USDT.";
        return false;
    }
    if (quantity < 0 && current.value(coin) + 1e-12 < -quantity) {
        db.rollback();
        *error = "Not enough " + coin + ".";
        return false;
    }

    QSqlQuery query;
    query.prepare(QString("UPDATE users SET USDT = USDT - :cost, %1 = %1 + :quantity "
                          "WHERE Username = :username").arg(coin));
    query.bindValue(":cost", cost);
    query.bindValue(":quantity", quantity);
    query.bindValue(":username", username);
    if (!query.exec() || query.numRowsAffected() != 1) {
        db.rollback();
        *error = "The order could not be saved.";
        return false;
    }
    return db.commit();
}

} // namespace Database
