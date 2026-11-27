// Automatic tests for the database logic and the trading window.
// They use a separate test database, your real accounts are not touched.
#include <QFile>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QtTest>

#include "database.h"
#include "mainwindow.h"

class TestBitExchange : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        QStandardPaths::setTestModeEnabled(true);  // database goes to a test folder
        QFile::remove(Database::filePath());
        QString error;
        QVERIFY2(Database::open(&error), qPrintable(error));
        QVERIFY(Database::createUser("serhii", "secret", &error));
    }

    void passwordIsHashed()
    {
        QSqlQuery query("SELECT Password FROM users WHERE Username = 'serhii'");
        QVERIFY(query.next());
        QVERIFY(query.value(0).toString() != "secret");
        QVERIFY(query.value(0).toString().startsWith("pbkdf2_sha256$"));
    }

    void login()
    {
        QVERIFY(Database::checkPassword("serhii", "secret"));
        QVERIFY(!Database::checkPassword("serhii", "wrong"));
        QVERIFY(!Database::checkPassword("nobody", "secret"));
        // The old version built the SQL from the text fields: this used to log in without a password
        QVERIFY(!Database::checkPassword("serhii' OR '1'='1", "x' OR '1'='1"));
    }

    void registrationRules()
    {
        QString error;
        QVERIFY(!Database::createUser("serhii", "another", &error));  // name taken
        QVERIFY(!Database::createUser("ab", "secret", &error));       // name too short
        QVERIFY(!Database::createUser("anna", "123", &error));        // password too short
    }

    void startBalance()
    {
        const auto balance = Database::balances("serhii");
        QCOMPARE(balance.value("USDT"), 1000.0);
        QCOMPARE(balance.value("BTC"), 0.0);
    }

    void buyAndSell()
    {
        QString error;
        QVERIFY(Database::createUser("trader", "secret", &error));

        QVERIFY(Database::trade("trader", "BTC", 0.01, 50000, &error));    // buy for 500 USDT
        QCOMPARE(Database::balances("trader").value("USDT"), 500.0);
        QCOMPARE(Database::balances("trader").value("BTC"), 0.01);

        QVERIFY(Database::trade("trader", "BTC", -0.005, 60000, &error));  // sell for 300 USDT
        QCOMPARE(Database::balances("trader").value("USDT"), 800.0);

        QVERIFY(!Database::trade("trader", "BTC", 1, 50000, &error));      // not enough USDT
        QVERIFY(!Database::trade("trader", "ETH", -1, 3000, &error));      // no ETH to sell
        QVERIFY(!Database::trade("trader", "BTC; DROP TABLE users", 1, 1, &error));
        QCOMPARE(Database::balances("trader").value("USDT"), 800.0);       // nothing changed
    }

    void tradingWindow()
    {
        MainWindow window("serhii");
        window.show();

        // Pretend Binance answered: price 50000 and three candles
        QMetaObject::invokeMethod(&window, "onPriceReceived",
                                  Q_ARG(QString, "BTCUSDT"), Q_ARG(double, 50000.0));
        QJsonArray candles;
        const qint64 start = 1764000000000;
        candles.append(QJsonArray{start, "49000", "50500", "48800", "50200"});
        candles.append(QJsonArray{start + 60000, "50200", "50400", "49500", "49700"});
        candles.append(QJsonArray{start + 120000, "49700", "50100", "49600", "50000"});
        QMetaObject::invokeMethod(&window, "onCandlesReceived",
                                  Q_ARG(QString, "BTCUSDT"), Q_ARG(QJsonArray, candles));

        auto *quantity = window.findChild<QLineEdit *>("QuantityLine");
        auto *value = window.findChild<QLineEdit *>("ValueLine");
        quantity->setText("0,01");                       // comma works as well
        QCOMPARE(value->text(), QString("500.00 USDT"));

        QTest::mouseClick(window.findChild<QPushButton *>("Execbtn"), Qt::LeftButton);
        QCOMPARE(window.findChild<QLabel *>("USDTOutput")->text(), QString("500.00"));
        QCOMPARE(window.findChild<QLabel *>("BTCValue")->text(), QString("0.01"));

        const QString screenshot = qEnvironmentVariable("SCREENSHOT");
        if (!screenshot.isEmpty()) {
            QTest::qWait(300);  // let the chart finish its layout before taking the picture
            window.grab().save(screenshot);
        }
    }
};

QTEST_MAIN(TestBitExchange)
#include "tst_bitexchange.moc"
