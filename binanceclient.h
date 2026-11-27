#ifndef BINANCECLIENT_H
#define BINANCECLIENT_H

#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

// Talks to the public Binance REST API (market data only, no API key needed).
// The requests are asynchronous: the methods return at once and the result
// arrives later through a signal, so the window never freezes.
class binanceClient : public QObject
{
    Q_OBJECT
public:
    explicit binanceClient(QObject *parent = nullptr);

    // GET /api/v3/ticker/price?symbol=BTCUSDT  ->  priceReceived()
    void requestPrice(const QString &symbol);

    // GET /api/v3/klines?symbol=BTCUSDT&interval=1m&limit=60  ->  candlesReceived()
    // Returns the newest "limit" candles, one per "interval".
    void requestCandles(const QString &symbol, const QString &interval, int limit);

signals:
    void priceReceived(const QString &symbol, double price);
    void candlesReceived(const QString &symbol, const QJsonArray &candles);
    void requestFailed(const QString &message);

private:
    QNetworkAccessManager *_network;
};

#endif // BINANCECLIENT_H
