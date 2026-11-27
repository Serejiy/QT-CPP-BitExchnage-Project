#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QMessageBox>

#include "database.h"

// Prices above 1 with 2 decimals, small prices (PEPE) with 8
static QString formatPrice(double price)
{
    return QString::number(price, 'f', price >= 1 ? 2 : 8);
}

MainWindow::MainWindow(const QString &username, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _client(new binanceClient(this)),
    _priceTimer(new QTimer(this)),
    _candleTimer(new QTimer(this)),
    _chart(new QChart()),
    _candles(new QCandlestickSeries()),
    _username(username)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("BitExchange - " + _username);
    ui->UserLabel->setText(_username);

    // Chart: green candle = price went up, red = price went down
    _candles->setIncreasingColor(QColor(Qt::green));
    _candles->setDecreasingColor(QColor(Qt::red));
    _chart->addSeries(_candles);
    _chart->setAnimationOptions(QChart::NoAnimation);
    _chart->setTheme(QChart::ChartThemeDark);
    _chart->legend()->setVisible(false);
    _chartView = new candleSticksView(_chart);
    _chartView->setRenderHint(QPainter::Antialiasing);
    ui->gridLayout_2->addWidget(_chartView);

    // Order panel: price comes from Binance, value is calculated
    ui->lcdNumberWTCETH->setDigitCount(12);
    ui->OrderPticeLine->setReadOnly(true);
    ui->ValueLine->setReadOnly(true);
    ui->QuantityLine->setPlaceholderText("0.001");
    ui->radioButton_7->hide();  // "Active orders": orders are executed at once, none stay open
    ui->radioButton->setChecked(true);
    for (QLabel *label : {ui->USDTOutput, ui->BTCValue, ui->ETHValue, ui->BNBValue,
                          ui->PEPEValue, ui->DOGEValue})
        label->resize(160, label->height());  // room for long numbers

    // Answers from the API arrive through signals
    connect(_client, &binanceClient::priceReceived, this, &MainWindow::onPriceReceived);
    connect(_client, &binanceClient::candlesReceived, this, &MainWindow::onCandlesReceived);
    connect(_client, &binanceClient::requestFailed, this, [this](const QString &message) {
        statusBar()->showMessage("Network error: " + message, 5000);
    });

    // Timers instead of an endless thread: price every second, candles every 10 seconds
    connect(_priceTimer, &QTimer::timeout, this, &MainWindow::refreshPrice);
    connect(_candleTimer, &QTimer::timeout, this, &MainWindow::refreshCandles);
    _priceTimer->start(1000);
    _candleTimer->start(10000);

    updateBalances();
    refreshPrice();
    refreshCandles();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshPrice()
{
    _client->requestPrice(symbol());
}

void MainWindow::refreshCandles()
{
    _client->requestCandles(symbol(), _interval, _limit);
}

void MainWindow::onPriceReceived(const QString &symbol, double price)
{
    if (symbol != this->symbol())
        return;  // late answer for a coin that is no longer selected
    _currentPrice = price;
    ui->lcdNumberWTCETH->display(formatPrice(price));
    ui->labelPairPrice->setText(_currency + "/USDT:");
    ui->OrderPticeLine->setText(formatPrice(price));
    updateOrderValue();
}

void MainWindow::onCandlesReceived(const QString &symbol, const QJsonArray &candles)
{
    if (symbol != this->symbol())
        return;

    _candles->clear();
    QStringList times;
    double lowest = 0, highest = 0;
    for (const QJsonValue &value : candles) {
        // [openTime, open, high, low, close, ...]; prices come as text, e.g. "91234.56"
        const QJsonArray candle = value.toArray();
        auto *set = new QCandlestickSet(candle.at(0).toDouble());
        set->setOpen(candle.at(1).toString().toDouble());
        set->setHigh(candle.at(2).toString().toDouble());
        set->setLow(candle.at(3).toString().toDouble());
        set->setClose(candle.at(4).toString().toDouble());
        _candles->append(set);
        times << QDateTime::fromMSecsSinceEpoch(qint64(set->timestamp())).toString("hh:mm");

        lowest = (times.size() == 1) ? set->low() : qMin(lowest, set->low());
        highest = (times.size() == 1) ? set->high() : qMax(highest, set->high());
    }
    if (times.isEmpty())
        return;

    // X axis: one time label per candle. Y axis: from the lowest to the highest price.
    _chart->createDefaultAxes();
    const QList<QAbstractAxis *> axesX = _chart->axes(Qt::Horizontal);
    const QList<QAbstractAxis *> axesY = _chart->axes(Qt::Vertical);
    if (auto *axisX = qobject_cast<QBarCategoryAxis *>(axesX.isEmpty() ? nullptr : axesX.first()))
        axisX->setCategories(times);
    if (!axesY.isEmpty()) {
        const double margin = (highest - lowest) * 0.05;
        axesY.first()->setRange(lowest - margin, highest + margin);
    }
    _chartView->setReady(true);
}

void MainWindow::setRange(const QString &interval, int limit)
{
    _interval = interval;
    _limit = limit;
    refreshCandles();
}

// Time range buttons: candle size x number of candles = time span
void MainWindow::on_Hour_clicked()           { setRange("1m", 60); }   // 60 x 1 min
void MainWindow::on_TwoHours_clicked()       { setRange("1m", 120); }  // 120 x 1 min
void MainWindow::on_FourHours_clicked()      { setRange("5m", 48); }   // 48 x 5 min
void MainWindow::on_EightHours_clicked()     { setRange("5m", 96); }   // 96 x 5 min
void MainWindow::on_TwentyFourHours_clicked(){ setRange("15m", 96); }  // 96 x 15 min

void MainWindow::setCurrency(const QString &currency)
{
    _currency = currency;
    _currentPrice = 0.0;  // the old price belongs to another coin
    ui->OrderPticeLine->clear();
    updateOrderValue();
    refreshPrice();
    refreshCandles();
}

void MainWindow::on_radioButton_clicked()   { setCurrency("BTC"); }
void MainWindow::on_radioButton_2_clicked() { setCurrency("ETH"); }
void MainWindow::on_radioButton_3_clicked() { setCurrency("BNB"); }
void MainWindow::on_radioButton_4_clicked() { setCurrency("PEPE"); }
void MainWindow::on_radioButton_5_clicked() { setCurrency("DOGE"); }

void MainWindow::updateBalances()
{
    const QMap<QString, double> balance = Database::balances(_username);
    ui->USDTOutput->setText(QString::number(balance.value("USDT"), 'f', 2));
    ui->BTCValue->setText(QString::number(balance.value("BTC"), 'g', 10));
    ui->ETHValue->setText(QString::number(balance.value("ETH"), 'g', 10));
    ui->BNBValue->setText(QString::number(balance.value("BNB"), 'g', 10));
    ui->PEPEValue->setText(QString::number(balance.value("PEPE"), 'g', 10));
    ui->DOGEValue->setText(QString::number(balance.value("DOGE"), 'g', 10));
}

void MainWindow::on_Buybtn_clicked()
{
    _buying = true;
    ui->Execbtn->setText("Buy");
}

void MainWindow::on_Selbtn_clicked()
{
    _buying = false;
    ui->Execbtn->setText("Sell");
}

void MainWindow::on_QuantityLine_textChanged(const QString &)
{
    updateOrderValue();
}

// Reads the quantity field; accepts "0.5" and "0,5". Returns 0 for invalid input.
double MainWindow::quantity() const
{
    bool ok = false;
    const double value = ui->QuantityLine->text().trimmed().replace(',', '.').toDouble(&ok);
    return ok && value > 0 ? value : 0.0;
}

// Order value = quantity x current price (in USDT)
void MainWindow::updateOrderValue()
{
    const double quantity = this->quantity();
    if (quantity > 0 && _currentPrice > 0)
        ui->ValueLine->setText(QString::number(quantity * _currentPrice, 'f', 2) + " USDT");
    else
        ui->ValueLine->clear();
}

// Simulated market order: the balances in the database change, no real money is involved
void MainWindow::on_Execbtn_clicked()
{
    const double quantity = this->quantity();
    if (quantity <= 0) {
        QMessageBox::information(this, "Order", "Please enter a quantity.");
        return;
    }

    QString error;
    const double signedQuantity = _buying ? quantity : -quantity;
    if (Database::trade(_username, _currency, signedQuantity, _currentPrice, &error)) {
        statusBar()->showMessage(QString("%1 %2 %3 at %4 USDT")
                                     .arg(_buying ? "Bought" : "Sold")
                                     .arg(quantity, 0, 'g', 10)
                                     .arg(_currency, formatPrice(_currentPrice)), 5000);
        ui->QuantityLine->clear();
        updateBalances();
    } else {
        QMessageBox::warning(this, "Order", error);
    }
}
