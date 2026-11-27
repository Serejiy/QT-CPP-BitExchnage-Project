#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QJsonArray>
#include <QMainWindow>
#include <QTimer>
#include <QtCharts>

#include "binanceclient.h"
#include "candlesticksview.h"

namespace Ui {
class MainWindow;
}

// The trading window: live price, candlestick chart, balances and buy/sell.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &username, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Answers from the Binance API
    void onPriceReceived(const QString &symbol, double price);
    void onCandlesReceived(const QString &symbol, const QJsonArray &candles);

    // Called by the timers
    void refreshPrice();
    void refreshCandles();

    // Buttons from mainwindow.ui (connected automatically by their names)
    void on_Hour_clicked();
    void on_TwoHours_clicked();
    void on_FourHours_clicked();
    void on_EightHours_clicked();
    void on_TwentyFourHours_clicked();
    void on_radioButton_clicked();     // BTC
    void on_radioButton_2_clicked();   // ETH
    void on_radioButton_3_clicked();   // BNB
    void on_radioButton_4_clicked();   // PEPE
    void on_radioButton_5_clicked();   // DOGE
    void on_Buybtn_clicked();
    void on_Selbtn_clicked();
    void on_Execbtn_clicked();
    void on_QuantityLine_textChanged(const QString &text);

private:
    QString symbol() const { return _currency + "USDT"; }  // e.g. "BTCUSDT"
    void setCurrency(const QString &currency);
    void setRange(const QString &interval, int limit);
    void updateBalances();
    void updateOrderValue();
    double quantity() const;

    Ui::MainWindow *ui;
    binanceClient *_client;
    QTimer *_priceTimer;
    QTimer *_candleTimer;

    QChart *_chart;
    QCandlestickSeries *_candles;
    candleSticksView *_chartView;

    QString _username;
    QString _currency = "BTC";   // selected coin
    QString _interval = "1m";    // one candle per minute ...
    int _limit = 60;             // ... 60 candles = the last hour
    double _currentPrice = 0.0;
    bool _buying = true;         // Buy or Sell mode
};

#endif // MAINWINDOW_H
