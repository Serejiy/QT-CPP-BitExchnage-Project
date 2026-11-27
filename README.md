# 📈 BitExchange — Crypto Chart Viewer with Paper Trading

A desktop application in C++ and Qt that shows live cryptocurrency prices from the
Binance API as candlestick charts. Users register, log in and trade with play money:
every new account starts with 1000 USDT.

## Download (Windows)

**[⬇ Download BitExchange.exe](https://github.com/Serejiy/QT-CPP-BitExchnage-Project/releases/latest/download/BitExchange.exe)**

One file, no installation and no Qt needed: download it and double-click.
Works on Windows 10 and 11 (64-bit).

The file is not digitally signed, so Windows may show "Windows protected your PC".
Click **More info → Run anyway**.

## Features

- Registration and login (passwords are stored as salted PBKDF2-SHA256 hashes)
- Live price for BTC, ETH, BNB, PEPE and DOGE against USDT, updated every second
- Candlestick chart for the last 1, 2, 4, 8 or 24 hours (zoom with the mouse wheel)
- Simulated buy and sell orders at the current price, balances saved in SQLite

## How it works

| Class | Job |
|-------|-----|
| `welcomepage`, `login`, `registration` | Start window, login and registration forms |
| `MainWindow` | Trading window: price, chart, balances, buy/sell |
| `binanceClient` | Sends HTTP requests to the Binance REST API and parses the JSON answers |
| `candleSticksView` | Chart widget with zoom and a tooltip (time and price under the mouse) |
| `Database` | All SQL: creates the table, registration, login check, balances, trades |

1. Two `QTimer`s in `MainWindow` ask `binanceClient` for the price (every second) and
   the candles (every 10 seconds).
2. `binanceClient` sends the request with `QNetworkAccessManager`. The answer comes
   back later through a signal (`priceReceived`, `candlesReceived`), so the window
   never freezes while waiting.
3. `MainWindow` shows the price and turns the candles
   (`[openTime, open, high, low, close, ...]`) into a `QCandlestickSeries`.
4. A buy or sell order calls `Database::trade`, which checks the balance and updates
   USDT and the coin in one SQL transaction.

### Binance API (public market data, no API key)

- `GET https://data-api.binance.vision/api/v3/ticker/price?symbol=BTCUSDT`
- `GET https://data-api.binance.vision/api/v3/klines?symbol=BTCUSDT&interval=1m&limit=60`

### Database (SQLite)

Table `users`: `ID`, `Username` (unique), `Password` (hash), `Role`,
`USDT`, `BTC`, `ETH`, `BNB`, `PEPE`, `DOGE`.

The file is created on the first start in the user's app data folder, e.g.
`C:\Users\<name>\AppData\Roaming\Serejiy\BitExchange\bitexchange.db`.
The start window shows the exact path.

## Tech Stack

| Layer | Technology |
|-------|-----------|
| Language | C++17 |
| GUI | Qt 6 Widgets, Qt Charts |
| Network | Qt Network (`QNetworkAccessManager`), JSON |
| Database | SQLite through Qt SQL |
| Build | qmake (`.pro`) |
| Tests | Qt Test |

## Build & Run

1. Install Qt 6 with the Qt Online Installer. In the component list, also select
   **Qt Charts** (under "Additional Libraries"), otherwise the project does not build.
2. Open `Bit_Exchange.pro` in Qt Creator, choose a kit (e.g. MinGW 64-bit) and press **Run**.

### Tests

Open `tests/tests.pro` in Qt Creator and press **Run**. The tests use a separate
database, so your accounts are not touched.

## Security

- Passwords are hashed with PBKDF2-SHA256 and a random salt, never stored in plain text.
- All SQL uses prepared statements with placeholders, so input from the login form
  cannot change the SQL command (no SQL injection).
- The database file is not part of the repository.

## What I Learned

- Building desktop GUI applications with Qt and C++ (widgets, forms, signals and slots)
- Asynchronous HTTP requests to a REST API and parsing JSON
- Drawing candlestick charts with Qt Charts
- SQLite with Qt SQL: prepared statements, transactions, password hashing
- Finding and fixing bugs, and writing automatic tests with Qt Test
