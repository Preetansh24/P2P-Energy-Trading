<div align="center">

<!-- Animated Banner -->
<img src="https://capsule-render.vercel.app/api?type=waving&color=gradient&customColorList=6,11,20&height=200&section=header&text=⚡%20NEXUS%20NETWORK&fontSize=70&fontColor=fff&animation=twinkling&fontAlignY=35&desc=Next-Generation%20P2P%20Energy%20Trading%20Platform&descAlignY=58&descSize=20" width="100%"/>

<!-- Typing Animation -->
<a href="https://git.io/typing-svg">
  <img src="https://readme-typing-svg.demolab.com?font=Orbitron&weight=700&size=22&pause=1000&color=00F2FE&center=true&vCenter=true&multiline=true&repeat=true&width=800&height=100&lines=⚡+Decentralized+Energy+Trading+at+Scale;🌐+Graph-Powered+P2P+Network+Topology;🤖+AI-Driven+Smart+Trade+Suggestions;📊+Real-Time+Market+Analytics+Dashboard" alt="Typing SVG" />
</a>

<br/>

<!-- Badges Row 1 -->
[![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Cross--Platform-00F2FE?style=for-the-badge&logo=linux&logoColor=white)](/)
[![HTML5](https://img.shields.io/badge/Frontend-HTML5%20%2F%20CSS3%20%2F%20JS-E34F26?style=for-the-badge&logo=html5&logoColor=white)](/)
[![Chart.js](https://img.shields.io/badge/Charts-Chart.js-FF6384?style=for-the-badge&logo=chartdotjs&logoColor=white)](https://www.chartjs.org/)

<!-- Badges Row 2 -->
[![License](https://img.shields.io/badge/License-MIT-00FF88?style=for-the-badge)](LICENSE)
[![Status](https://img.shields.io/badge/Status-Active-00FF88?style=for-the-badge&logo=statuspage&logoColor=white)](/)
[![PRs Welcome](https://img.shields.io/badge/PRs-Welcome-4FACFE?style=for-the-badge&logo=github&logoColor=white)](/)
[![Stars](https://img.shields.io/badge/⭐_Give_a_Star-FFD700?style=for-the-badge)](/)

<br/>

```
╔══════════════════════════════════════════════════════════════════════╗
║                                                                      ║
║    ███╗   ██╗███████╗██╗  ██╗██╗   ██╗███████╗                      ║
║    ████╗  ██║██╔════╝╚██╗██╔╝██║   ██║██╔════╝                      ║
║    ██╔██╗ ██║█████╗   ╚███╔╝ ██║   ██║███████╗                      ║
║    ██║╚██╗██║██╔══╝   ██╔██╗ ██║   ██║╚════██║                      ║
║    ██║ ╚████║███████╗██╔╝ ██╗╚██████╔╝███████║                      ║
║    ╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝  NETWORK  v1.0       ║
║                                                                      ║
║         ⚡  P2P Energy Trading  •  Built with C++ & HTML5  ⚡         ║
╚══════════════════════════════════════════════════════════════════════╝
```

</div>

---

<div align="center">

## 🌊 What is NEXUS?

</div>

> **NEXUS Network** is a blazing-fast, graph-powered **Peer-to-Peer Energy Trading Platform** built in modern **C++17**. It simulates a fully decentralized smart grid where energy producers, consumers, and storage nodes trade electricity directly — without any middlemen — backed by real-time analytics, AI-powered suggestions, and a stunning animated HTML5 dashboard.

---

<div align="center">

## ✨ Features at a Glance

</div>

<table align="center">
<tr>
<td align="center" width="33%">

### ⚡ Real-Time Trading
Live P2P energy transactions with instant balance updates, fee calculations, and transaction ledger

</td>
<td align="center" width="33%">

### 🌐 Network Topology
Interactive animated graph visualization with BFS pathfinding and energy flow animations

</td>
<td align="center" width="33%">

### 🤖 AI Suggestions
Smart trade-matching engine that scores opportunities by energy fit, price, proximity & balance

</td>
</tr>
<tr>
<td align="center" width="33%">

### 📊 Live Analytics
Auto-updating Chart.js price & volume charts with market volatility and liquidity metrics

</td>
<td align="center" width="33%">

### 🔍 Path Analysis
BFS shortest-path routing between nodes with real-time hop count and efficiency scores

</td>
<td align="center" width="33%">

### 🌱 Network Growth
One-click node simulation to dynamically grow the energy grid with new producers/consumers

</td>
</tr>
</table>

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     NEXUS PLATFORM ENGINE                       │
│                                                                 │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────────┐  │
│  │   User DB    │◄──►│ Energy Graph │◄──►│  Transaction     │  │
│  │  (HashMap)   │    │ (Adj. List)  │    │    Manager       │  │
│  └──────────────┘    └──────────────┘    └──────────────────┘  │
│          │                  │                     │             │
│          ▼                  ▼                     ▼             │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────────┐  │
│  │    Trade     │    │     BFS      │    │    Market        │  │
│  │  Suggestion  │    │  Pathfinder  │    │   Analytics      │  │
│  │   Engine     │    │  & Clusters  │    │  (Volatility)    │  │
│  └──────────────┘    └──────────────┘    └──────────────────┘  │
│                                │                                │
│                                ▼                                │
│                    ┌────────────────────┐                       │
│                    │  HTML GUI Generator│                       │
│                    │  (Full Dashboard)  │                       │
│                    └────────────────────┘                       │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🧠 Data Structures & Algorithms

| Component | Structure / Algorithm | Purpose |
|---|---|---|
| `EnergyGraph` | **Adjacency List** (`unordered_map`) | Efficient O(1) neighbor lookup |
| `findShortestPath()` | **BFS** | Minimum-hop path between nodes |
| `findAllPaths()` | **BFS with backtracking** | All trading routes (max depth) |
| `getNetworkClusters()` | **BFS traversal** | Detect connected components |
| `TradeSuggestionEngine` | **Greedy scoring** | Match producers ↔ consumers |
| `MarketAnalytics` | **Running variance (Welford-style)** | Real-time price volatility |
| `TransactionManager` | **Vector + HashMap** | O(1) lookup, O(n) history |
| `EnergyTradingPlatform` | **Multi-threaded engine** | Background analytics refresh |

---

## 🚀 Getting Started

### Prerequisites

```bash
# You need a C++17 compatible compiler
g++ --version     # GCC 7+ recommended
clang++ --version # Clang 5+ works too
```

### ⚙️ Build & Run

```bash
# 1. Clone the repository
git clone https://github.com/yourusername/nexus-network.git
cd nexus-network

# 2. Compile with C++17 and threading support
g++ -std=c++17 -O2 -pthread -o nexus main.cpp

# 3. Run the platform
./nexus
```

```bash
# Windows (MinGW)
g++ -std=c++17 -O2 -o nexus.exe main.cpp -lpthread
nexus.exe
```

> ✅ The program auto-generates `energy_trading_platform.html` and opens it in your default browser!

---

## 🖥️ Dashboard Walkthrough

```
┌──────────────────────────────────────────────────────┐
│  ⚡ NEXUS NETWORK                                     │
│  Advanced P2P Energy Trading Platform                 │
├────────┬──────────┬────────────┬──────┬──────┬───────┤
│ 📊 DASH│ ⚡ TRADE  │ 💡 SUGGEST │ 👥  │ 📈   │ 🌐    │
│ BOARD  │ CONSOLE  │ ENGINE     │ NODES│ LEDGER│ GRAPH │
└────────┴──────────┴────────────┴──────┴──────┴───────┘
    │
    ├── 📊 DASHBOARD
    │     ├── 6 live stat cards (energy, revenue, nodes...)
    │     ├── Real-time price line chart (Chart.js)
    │     ├── Trade volume bar chart (auto-updates every 2s)
    │     └── Recent transaction feed (live)
    │
    ├── ⚡ EXECUTE TRADE
    │     ├── Seller/buyer dropdowns with live data
    │     ├── Real-time validation (energy + balance checks)
    │     ├── Network path analysis (BFS)
    │     └── One-click trade execution
    │
    ├── 💡 SMART SUGGESTIONS
    │     ├── AI-scored trade opportunities
    │     ├── Match score, path hops, price range
    │     └── "Use This" → auto-fills trade form
    │
    ├── 👥 NETWORK NODES
    │     ├── Producers card grid (surplus, balance)
    │     └── Consumers card grid (demand, balance)
    │
    ├── 📈 TRANSACTION LEDGER
    │     └── Full table: ID, seller, buyer, kWh, price, total
    │
    └── 🌐 NETWORK TOPOLOGY
          ├── Animated canvas with pulsing nodes
          ├── Dashed energy-flow connections
          ├── Color-coded: 🟢 Producer / 🔵 Consumer / 🔴 Storage
          └── "Simulate Growth" adds live nodes
```

---

## 📦 Sample Network (Pre-loaded)

| Node ID | Name | Type | Surplus/Demand | Balance |
|---|---|---|---|---|
| `SOLAR_001` | Quantum Solar Farm | 🟢 Producer | 450 kWh | ₹12,000 |
| `HYDRO_001` | Hydro Electric Station | 🟢 Producer | 680 kWh | ₹18,000 |
| `WIND_001` | Wind Turbine Array | 🟢 Producer | 320 kWh | ₹9,000 |
| `NUCLEAR_001` | Nuclear Power Plant | 🟢 Producer | 950 kWh | ₹22,000 |
| `RES_001` | Residential Complex A | 🔵 Consumer | 280 kWh | ₹15,000 |
| `TECH_001` | Tech Campus | 🔵 Consumer | 520 kWh | ₹30,000 |
| `IND_001` | Industrial Park | 🔵 Consumer | 750 kWh | ₹45,000 |
| `GRID_001` | Smart Grid Hub | 🔴 Storage | 180 / 80 kWh | ₹15,000 |
| `BATT_001` | Battery Storage A | 🔴 Storage | 120 / 40 kWh | ₹10,000 |
| `COMM_001` | Commercial District | 🔵 Consumer | 320 kWh | ₹25,000 |

---

## 🔬 Core Classes

<details>
<summary><b>📌 Click to expand class reference</b></summary>

### `EnergyTradingPlatform`
The central engine. Manages users, the graph, transactions, and spawns the background analytics thread.

```cpp
platform.addUser(make_shared<User>(...));        // Register node
platform.connectUsers("SOLAR_001", "RES_001");   // Create edge
platform.executeTrade(sid, bid, kWh, price);     // Atomic trade
platform.getTradeSuggestions();                  // AI suggestions
platform.getNetworkJSON();                       // Serialize to JSON
```

### `EnergyGraph`
Undirected graph backed by an adjacency list with BFS utilities.

```cpp
graph.addEdge(u, v);                // Connect two nodes
graph.findShortestPath(src, dst);   // BFS → vector<string>
graph.getNetworkClusters();         // BFS → connected components
graph.calculateNodePositions();     // Circular layout for canvas
```

### `TradeSuggestionEngine`
Scores every (producer, consumer) pair across 4 dimensions and returns the top 5.

```cpp
// Score = 40% energy match + 30% balance + 20% proximity + 10% price
engine.generateSuggestions();  // → vector<TradeSuggestion>
```

### `MarketAnalytics`
Maintains rolling price/volume history and computes volatility.

```cpp
analytics.recordTrade(kWh, price, time);
analytics.getAveragePrice();
analytics.getPriceVolatility();    // stddev of price history
analytics.getMarketLiquidity();
```

</details>

---

## 📈 Market Stats Computed

```cpp
map<string, double> stats = platform.getMarketStats();
// Keys:
//  total_energy_traded  •  total_revenue  •  transaction_fees
//  average_price        •  price_volatility
//  active_sellers       •  active_buyers  •  total_users
//  total_connections    •  network_efficiency  (avg 1/path_length)
```

---

## 🗺️ Project Structure

```
nexus-network/
│
├── 📄 main.cpp                      ← Full platform source (C++17)
├── 📄 energy_trading_platform.html  ← Auto-generated dashboard (output)
└── 📄 README.md                     ← You are here
```

---

## 🔮 Roadmap

- [x] Core P2P trading engine
- [x] BFS pathfinding & cluster detection
- [x] AI trade suggestion scoring
- [x] Real-time animated HTML5 dashboard
- [x] Multi-threaded analytics background thread
- [ ] Persistent storage (SQLite / JSON)
- [ ] WebSocket-based live updates
- [ ] Blockchain transaction ledger
- [ ] REST API layer (Crow / Drogon)
- [ ] Dynamic pricing auction engine
- [ ] Mobile-responsive PWA dashboard

---

## 🤝 Contributing

```bash
# Fork → Clone → Branch → Code → PR
git checkout -b feature/your-amazing-feature
git commit -m "⚡ Add: your amazing feature"
git push origin feature/your-amazing-feature
# Open a Pull Request 🎉
```

All contributions are welcome — bug fixes, new features, UI improvements, or docs!

---

<div align="center">

## 📜 License

Released under the **MIT License** — free to use, modify, and distribute.

---

### Built with ⚡ and a lot of `cout <<` statements

<img src="https://capsule-render.vercel.app/api?type=waving&color=gradient&customColorList=6,11,20&height=120&section=footer&animation=twinkling" width="100%"/>

</div>
