# 📈 BitExchange — Crypto Chart Viewer

A desktop application for visualizing real-time cryptocurrency price data using candlestick charts, built with C++ and Qt.

## Features

- Connects to the **Binance API** to fetch live market data
- Displays **candlestick charts** for selected trading pairs (BTC, ETH, etc.)
- User **login and registration** system
- Local database for storing user data

## Tech Stack

| Layer | Technology |
|-------|-----------|
| Language | C++ |
| GUI Framework | Qt (QMake) |
| API | Binance REST API |
| Database | Local DB (SQLite) |
| Build System | QMake (.pro) |

## Project Structure

```
NG_BitExchnage_Project/
├── BinanceClient/       # API connection and data fetching
├── CandlesticksView/    # Chart rendering logic
├── DB/                  # Database layer
├── UI/                  # Qt .ui interface files
└── main.cpp             # Entry point
```

## Getting Started

### Prerequisites
- Qt 5.x or 6.x
- Qt Creator (recommended)
- C++ compiler (MSVC / GCC / Clang)

### Build & Run

1. Open the `.pro` file in Qt Creator
2. Configure your kit (compiler + Qt version)
3. Click **Run** (Ctrl+R)

> Note: A valid Binance API connection is required to fetch live data.

## What I Learned

- Building desktop GUI applications with Qt and C++
- Connecting to a real REST API (Binance) and parsing JSON responses
- Rendering financial chart data (candlestick visualization)
- Implementing a login/registration system with local database storage
- Object-oriented design in C++
