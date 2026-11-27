#include "binanceclient.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

// Binance's address for public market data (same data as api.binance.com)
static const char *const API_URL = "https://data-api.binance.vision/api/v3";

binanceClient::binanceClient(QObject *parent)
    : QObject(parent)
    , _network(new QNetworkAccessManager(this))
{
}

void binanceClient::requestPrice(const QString &symbol)
{
    QUrl url(QString(API_URL) + "/ticker/price");
    QUrlQuery query;
    query.addQueryItem("symbol", symbol);
    url.setQuery(query);

    QNetworkReply *reply = _network->get(QNetworkRequest(url));
    // This lambda runs when the answer has arrived, e.g. {"symbol":"BTCUSDT","price":"91234.56"}
    connect(reply, &QNetworkReply::finished, this, [this, reply, symbol]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit requestFailed(reply->errorString());
            return;
        }
        const QJsonObject json = QJsonDocument::fromJson(reply->readAll()).object();
        const double price = json.value("price").toString().toDouble();
        if (price > 0)
            emit priceReceived(symbol, price);
    });
}

void binanceClient::requestCandles(const QString &symbol, const QString &interval, int limit)
{
    QUrl url(QString(API_URL) + "/klines");
    QUrlQuery query;
    query.addQueryItem("symbol", symbol);
    query.addQueryItem("interval", interval);
    query.addQueryItem("limit", QString::number(limit));
    url.setQuery(query);

    QNetworkReply *reply = _network->get(QNetworkRequest(url));
    // Answer: an array of candles, each [openTime, open, high, low, close, volume, ...]
    connect(reply, &QNetworkReply::finished, this, [this, reply, symbol]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit requestFailed(reply->errorString());
            return;
        }
        const QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
        if (json.isArray())
            emit candlesReceived(symbol, json.array());
    });
}
