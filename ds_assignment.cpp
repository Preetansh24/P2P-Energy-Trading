#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <memory>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <queue>
#include <stack>
#include <set>
#include <thread>
#include <chrono>
#include <atomic>

using namespace std;

// ==================== DATA STRUCTURES ====================

struct Transaction {
    string id;
    string sellerId;
    string buyerId;
    double energyAmount;
    double pricePerUnit;
    double totalPrice;
    time_t timestamp;

    Transaction(const string& sid, const string& bid, double energy, double price)
        : sellerId(sid), buyerId(bid), energyAmount(energy), pricePerUnit(price) {
        timestamp = time(nullptr);
        totalPrice = energyAmount * pricePerUnit;
        id = generateId();
    }

    string generateId() {
        stringstream ss;
        ss << "TXN" << timestamp << "_" << (rand() % 10000);
        return ss.str();
    }

    string getFormattedTime() const {
        char buffer[80];
        struct tm* timeinfo = localtime(&timestamp);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        return string(buffer);
    }
};

struct User {
    string id;
    string name;
    double energySurplus;
    double energyDemand;
    double balance;
    vector<string> transactionHistory;
    string type; // "producer", "consumer", "storage"

    User(const string& userId, const string& userName, double surplus, double demand, double bal, string userType = "producer")
        : id(userId), name(userName), energySurplus(surplus),
          energyDemand(demand), balance(bal), type(userType) {}

    bool canSell(double amount) const {
        return energySurplus >= amount && balance >= 0;
    }

    bool canBuy(double amount, double price) const {
        return energyDemand >= amount && balance >= amount * price;
    }

    string getStatus() const {
        if (energySurplus > 0 && energyDemand > 0) return "storage";
        if (energySurplus > 0) return "producer";
        return "consumer";
    }
};

// ==================== GRAPH FOR USER CONNECTIONS ====================

class EnergyGraph {
private:
    unordered_map<string, vector<string>> adjList;
    unordered_map<string, pair<double, double>> nodePositions;

public:
    void addEdge(const string& user1, const string& user2) {
        if (find(adjList[user1].begin(), adjList[user1].end(), user2) == adjList[user1].end()) {
            adjList[user1].push_back(user2);
        }
        if (find(adjList[user2].begin(), adjList[user2].end(), user1) == adjList[user2].end()) {
            adjList[user2].push_back(user1);
        }
    }

    void removeEdge(const string& user1, const string& user2) {
        auto& neighbors1 = adjList[user1];
        neighbors1.erase(remove(neighbors1.begin(), neighbors1.end(), user2), neighbors1.end());

        auto& neighbors2 = adjList[user2];
        neighbors2.erase(remove(neighbors2.begin(), neighbors2.end(), user1), neighbors2.end());
    }

    vector<string> getNeighbors(const string& userId) {
        return adjList.count(userId) ? adjList[userId] : vector<string>();
    }

    bool areConnected(const string& user1, const string& user2) {
        if (!adjList.count(user1)) return false;
        auto& neighbors = adjList[user1];
        return find(neighbors.begin(), neighbors.end(), user2) != neighbors.end();
    }

    int getTotalConnections() const {
        int total = 0;
        for (const auto& pair : adjList) {
            total += pair.second.size();
        }
        return total / 2;
    }

    unordered_map<string, vector<string>>& getAdjList() {
        return adjList;
    }

    // BFS for finding shortest path between users
    vector<string> findShortestPath(const string& start, const string& end) {
        if (!adjList.count(start) || !adjList.count(end) || start == end) return {};

        unordered_map<string, string> parent;
        queue<string> q;
        q.push(start);
        parent[start] = "";

        while (!q.empty()) {
            string current = q.front();
            q.pop();

            if (current == end) {
                vector<string> path;
                string node = end;
                while (!node.empty()) {
                    path.push_back(node);
                    node = parent[node];
                }
                reverse(path.begin(), path.end());
                return path;
            }

            for (const string& neighbor : adjList[current]) {
                if (!parent.count(neighbor)) {
                    parent[neighbor] = current;
                    q.push(neighbor);
                }
            }
        }
        return {};
    }

    // Find all possible trading paths
    vector<vector<string>> findAllPaths(const string& start, const string& end, int maxDepth = 3) {
        vector<vector<string>> paths;
        if (!adjList.count(start) || !adjList.count(end)) return paths;

        queue<vector<string>> q;
        q.push({start});
        set<string> visited;

        while (!q.empty()) {
            vector<string> path = q.front();
            q.pop();
            string current = path.back();

            if (current == end) {
                paths.push_back(path);
                continue;
            }

            if (path.size() >= maxDepth) continue;

            for (const string& neighbor : adjList[current]) {
                if (find(path.begin(), path.end(), neighbor) == path.end()) {
                    vector<string> newPath = path;
                    newPath.push_back(neighbor);
                    q.push(newPath);
                }
            }
        }
        return paths;
    }

    void calculateNodePositions(int canvasWidth = 800, int canvasHeight = 600) {
        nodePositions.clear();
        vector<string> nodes;
        for (const auto& pair : adjList) {
            nodes.push_back(pair.first);
        }

        double centerX = canvasWidth / 2.0;
        double centerY = canvasHeight / 2.0;
        double radius = min(canvasWidth, canvasHeight) * 0.35;

        for (int i = 0; i < nodes.size(); i++) {
            double angle = (2 * 3.14159 * i) / nodes.size();
            double x = centerX + radius * cos(angle);
            double y = centerY + radius * sin(angle);
            nodePositions[nodes[i]] = {x, y};
        }
    }

    unordered_map<string, pair<double, double>>& getNodePositions() {
        return nodePositions;
    }

    // Get network clusters using BFS
    vector<vector<string>> getNetworkClusters() {
        vector<vector<string>> clusters;
        set<string> visited;

        for (const auto& pair : adjList) {
            string node = pair.first;
            if (visited.find(node) == visited.end()) {
                vector<string> cluster;
                queue<string> q;
                q.push(node);
                visited.insert(node);

                while (!q.empty()) {
                    string current = q.front();
                    q.pop();
                    cluster.push_back(current);

                    for (const string& neighbor : adjList[current]) {
                        if (visited.find(neighbor) == visited.end()) {
                            visited.insert(neighbor);
                            q.push(neighbor);
                        }
                    }
                }
                clusters.push_back(cluster);
            }
        }
        return clusters;
    }
};

// ==================== TRADE SUGGESTION ENGINE ====================

class TradeSuggestionEngine {
private:
    EnergyGraph& graph;
    unordered_map<string, shared_ptr<User>>& users;

public:
    TradeSuggestionEngine(EnergyGraph& g, unordered_map<string, shared_ptr<User>>& u)
        : graph(g), users(u) {}

    struct TradeSuggestion {
        string sellerId;
        string buyerId;
        double suggestedEnergy;
        double suggestedPrice;
        double matchScore;
        vector<string> path;
        string reason;
    };

    vector<TradeSuggestion> generateSuggestions() {
        vector<TradeSuggestion> suggestions;

        vector<string> producers, consumers;
        for (const auto& pair : users) {
            if (pair.second->energySurplus > 0) {
                producers.push_back(pair.first);
            }
            if (pair.second->energyDemand > 0) {
                consumers.push_back(pair.first);
            }
        }

        for (const string& sellerId : producers) {
            auto seller = users[sellerId];

            for (const string& buyerId : consumers) {
                auto buyer = users[buyerId];

                double maxEnergy = min(seller->energySurplus, buyer->energyDemand);
                double maxCost = maxEnergy * 0.15;

                if (maxEnergy > 0 && buyer->balance >= maxCost) {
                    TradeSuggestion suggestion;
                    suggestion.sellerId = sellerId;
                    suggestion.buyerId = buyerId;
                    suggestion.suggestedEnergy = maxEnergy * 0.8; // 80% of max
                    suggestion.suggestedPrice = 0.12 + (rand() % 8) * 0.01;
                    suggestion.path = graph.findShortestPath(sellerId, buyerId);
                    suggestion.matchScore = calculateMatchScore(seller, buyer, maxEnergy);
                    suggestion.reason = generateReason(seller, buyer, maxEnergy);

                    suggestions.push_back(suggestion);
                }
            }
        }

        sort(suggestions.begin(), suggestions.end(),
             [](const TradeSuggestion& a, const TradeSuggestion& b) {
                 return a.matchScore > b.matchScore;
             });

        if (suggestions.size() > 5) {
            suggestions.resize(5);
        }

        return suggestions;
    }

private:
    double calculateMatchScore(shared_ptr<User> seller, shared_ptr<User> buyer, double energy) {
        double score = 0.0;

        // Energy match (40%)
        score += (energy / 100.0) * 0.4;

        // Balance adequacy (30%)
        double requiredBalance = energy * 0.15;
        if (buyer->balance >= requiredBalance * 2) score += 0.3;
        else if (buyer->balance >= requiredBalance) score += 0.2;
        else score += 0.1;

        // Connection proximity (20%)
        vector<string> path = graph.findShortestPath(seller->id, buyer->id);
        if (!path.empty()) {
            score += (1.0 / path.size()) * 0.2;
        }

        // Price compatibility (10%)
        double sellerMinPrice = 0.10;
        double buyerMaxPrice = 0.20;
        double suggestedPrice = 0.15;
        if (suggestedPrice >= sellerMinPrice && suggestedPrice <= buyerMaxPrice) {
            score += 0.1;
        }

        return min(score, 1.0);
    }

    string generateReason(shared_ptr<User> seller, shared_ptr<User> buyer, double energy) {
        vector<string> reasons = {
            "High energy surplus matches demand perfectly",
            "Optimal network path with minimal hops",
            "Balanced pricing for both parties",
            "Strong financial capacity for transaction",
            "Efficient energy transfer opportunity",
            "Complementary peak production/consumption cycles"
        };
        return reasons[rand() % reasons.size()];
    }
};

// ==================== MARKET ANALYTICS ====================

class MarketAnalytics {
private:
    vector<double> energyPrices;
    vector<double> tradeVolumes;
    vector<time_t> timestamps;
    double priceVolatility;
    double totalTradedEnergy;

public:
    MarketAnalytics() : priceVolatility(0.0), totalTradedEnergy(0.0) {}

    void recordTrade(double energyAmount, double price, time_t timestamp) {
        energyPrices.push_back(price);
        tradeVolumes.push_back(energyAmount);
        timestamps.push_back(timestamp);
        totalTradedEnergy += energyAmount;

        if (energyPrices.size() >= 2) {
            double mean = 0.0;
            for (double p : energyPrices) mean += p;
            mean /= energyPrices.size();

            double variance = 0.0;
            for (double p : energyPrices) variance += (p - mean) * (p - mean);
            priceVolatility = sqrt(variance / energyPrices.size());
        }
    }

    double getAveragePrice() const {
        if (energyPrices.empty()) return 0.15;
        double sum = 0.0;
        for (double price : energyPrices) sum += price;
        return sum / energyPrices.size();
    }

    double getTotalVolume() const {
        return totalTradedEnergy;
    }

    double getPriceVolatility() const {
        return priceVolatility;
    }

    vector<pair<time_t, double>> getPriceHistory(int maxPoints = 20) const {
        vector<pair<time_t, double>> history;
        int startIdx = max(0, (int)energyPrices.size() - maxPoints);

        for (size_t i = startIdx; i < energyPrices.size(); i++) {
            history.push_back({timestamps[i], energyPrices[i]});
        }
        return history;
    }

    vector<pair<time_t, double>> getVolumeHistory(int maxPoints = 20) const {
        vector<pair<time_t, double>> history;
        int startIdx = max(0, (int)tradeVolumes.size() - maxPoints);

        for (size_t i = startIdx; i < tradeVolumes.size(); i++) {
            history.push_back({timestamps[i], tradeVolumes[i]});
        }
        return history;
    }

    double getMarketLiquidity() const {
        if (energyPrices.empty()) return 0.0;
        return (totalTradedEnergy / energyPrices.size()) * 100.0;
    }
};

// ==================== TRANSACTION MANAGER ====================

class TransactionManager {
private:
    unordered_map<string, shared_ptr<Transaction>> transactions;
    vector<shared_ptr<Transaction>> transactionHistory;
    MarketAnalytics analytics;

public:
    void addTransaction(shared_ptr<Transaction> txn) {
        transactions[txn->id] = txn;
        transactionHistory.push_back(txn);
        analytics.recordTrade(txn->energyAmount, txn->pricePerUnit, txn->timestamp);
    }

    vector<shared_ptr<Transaction>> getAllTransactions() const {
        return transactionHistory;
    }

    vector<shared_ptr<Transaction>> getUserTransactions(const string& userId) {
        vector<shared_ptr<Transaction>> userTxns;
        for (const auto& txn : transactionHistory) {
            if (txn->sellerId == userId || txn->buyerId == userId) {
                userTxns.push_back(txn);
            }
        }
        return userTxns;
    }

    double getTotalVolume() const {
        return analytics.getTotalVolume();
    }

    double getTotalRevenue() const {
        double total = 0.0;
        for (const auto& txn : transactionHistory) {
            total += txn->totalPrice;
        }
        return total;
    }

    MarketAnalytics& getAnalytics() {
        return analytics;
    }

    vector<shared_ptr<Transaction>> getRecentTransactions(int count = 10) {
        if (transactionHistory.size() <= count) {
            return transactionHistory;
        }
        return vector<shared_ptr<Transaction>>(
            transactionHistory.end() - count,
            transactionHistory.end()
        );
    }
};

// ==================== CORE PLATFORM ENGINE ====================

class EnergyTradingPlatform {
private:
    unordered_map<string, shared_ptr<User>> users;
    EnergyGraph connectionGraph;
    TransactionManager txnManager;
    TradeSuggestionEngine suggestionEngine;
    double transactionFeeRate = 0.02;
    atomic<bool> isRunning{false};
    thread analyticsThread;

public:
    EnergyTradingPlatform() : suggestionEngine(connectionGraph, users) {
        startAnalyticsThread();
    }

    ~EnergyTradingPlatform() {
        stopAnalyticsThread();
    }

    void addUser(shared_ptr<User> user) {
        users[user->id] = user;
        updateNetworkVisualization();
    }

    void connectUsers(const string& user1, const string& user2) {
        connectionGraph.addEdge(user1, user2);
        updateNetworkVisualization();
    }

    bool executeTrade(const string& sellerId, const string& buyerId,
                      double energyAmount, double pricePerUnit) {
        if (!users.count(sellerId) || !users.count(buyerId)) {
            return false;
        }

        auto seller = users[sellerId];
        auto buyer = users[buyerId];

        if (!seller->canSell(energyAmount) || !buyer->canBuy(energyAmount, pricePerUnit)) {
            return false;
        }

        double totalCost = energyAmount * pricePerUnit;
        double transactionFee = totalCost * transactionFeeRate;
        double sellerReceives = totalCost - transactionFee;

        seller->energySurplus -= energyAmount;
        buyer->energyDemand -= energyAmount;
        seller->balance += sellerReceives;
        buyer->balance -= totalCost;

        auto txn = make_shared<Transaction>(sellerId, buyerId, energyAmount, pricePerUnit);
        txnManager.addTransaction(txn);

        seller->transactionHistory.push_back(txn->id);
        buyer->transactionHistory.push_back(txn->id);

        if (!connectionGraph.areConnected(sellerId, buyerId)) {
            connectUsers(sellerId, buyerId);
        }

        return true;
    }

    vector<shared_ptr<User>> getSellers() {
        vector<shared_ptr<User>> sellers;
        for (const auto& pair : users) {
            if (pair.second->energySurplus > 0) {
                sellers.push_back(pair.second);
            }
        }
        return sellers;
    }

    vector<shared_ptr<User>> getBuyers() {
        vector<shared_ptr<User>> buyers;
        for (const auto& pair : users) {
            if (pair.second->energyDemand > 0) {
                buyers.push_back(pair.second);
            }
        }
        return buyers;
    }

    shared_ptr<User> getUser(const string& id) {
        return users.count(id) ? users[id] : nullptr;
    }

    vector<shared_ptr<Transaction>> getTransactionHistory() {
        return txnManager.getAllTransactions();
    }

    double getTotalTradedEnergy() {
        return txnManager.getTotalVolume();
    }

    double getTotalRevenue() {
        return txnManager.getTotalRevenue();
    }

    double getTransactionFees() {
        return txnManager.getTotalRevenue() * transactionFeeRate;
    }

    EnergyGraph& getGraph() {
        return connectionGraph;
    }

    unordered_map<string, shared_ptr<User>>& getAllUsers() {
        return users;
    }

    MarketAnalytics& getMarketAnalytics() {
        return txnManager.getAnalytics();
    }

    vector<TradeSuggestionEngine::TradeSuggestion> getTradeSuggestions() {
        return suggestionEngine.generateSuggestions();
    }

    void updateNetworkVisualization() {
        connectionGraph.calculateNodePositions();
    }

    map<string, double> getMarketStats() {
        map<string, double> stats;
        stats["total_energy_traded"] = getTotalTradedEnergy();
        stats["total_revenue"] = getTotalRevenue();
        stats["transaction_fees"] = getTransactionFees();
        stats["average_price"] = getMarketAnalytics().getAveragePrice();
        stats["price_volatility"] = getMarketAnalytics().getPriceVolatility();
        stats["active_sellers"] = getSellers().size();
        stats["active_buyers"] = getBuyers().size();
        stats["total_users"] = users.size();
        stats["total_connections"] = connectionGraph.getTotalConnections();

        double efficiency = 0.0;
        int pathCount = 0;
        auto& allUsers = getAllUsers();
        vector<string> nodeIds;
        for (const auto& pair : allUsers) {
            nodeIds.push_back(pair.first);
        }

        for (int i = 0; i < nodeIds.size(); i++) {
            for (int j = i + 1; j < nodeIds.size(); j++) {
                vector<string> path = connectionGraph.findShortestPath(nodeIds[i], nodeIds[j]);
                if (!path.empty()) {
                    efficiency += 1.0 / path.size();
                    pathCount++;
                }
            }
        }

        stats["network_efficiency"] = pathCount > 0 ? efficiency / pathCount : 0.0;
        return stats;
    }

    string getNetworkJSON() {
        stringstream ss;
        auto& positions = connectionGraph.getNodePositions();
        auto& adjList = connectionGraph.getAdjList();

        ss << "{\n";
        ss << "  \"nodes\": [\n";

        int nodeCount = 0;
        for (const auto& pair : users) {
            auto user = pair.second;
            auto pos = positions.count(user->id) ? positions[user->id] : make_pair(400.0, 300.0);

            ss << "    {\n";
            ss << "      \"id\": \"" << user->id << "\",\n";
            ss << "      \"name\": \"" << user->name << "\",\n";
            ss << "      \"type\": \"" << user->getStatus() << "\",\n";
            ss << "      \"surplus\": " << user->energySurplus << ",\n";
            ss << "      \"demand\": " << user->energyDemand << ",\n";
            ss << "      \"balance\": " << user->balance << ",\n";
            ss << "      \"x\": " << pos.first << ",\n";
            ss << "      \"y\": " << pos.second << "\n";
            ss << "    }";

            if (++nodeCount < users.size()) ss << ",";
            ss << "\n";
        }

        ss << "  ],\n";
        ss << "  \"connections\": [\n";

        int connCount = 0;
        set<pair<string, string>> addedConnections;

        for (const auto& pair : adjList) {
            for (const string& neighbor : pair.second) {
                string key1 = pair.first + "-" + neighbor;
                string key2 = neighbor + "-" + pair.first;

                if (addedConnections.find({pair.first, neighbor}) == addedConnections.end() &&
                    addedConnections.find({neighbor, pair.first}) == addedConnections.end()) {

                    ss << "    {\"from\": \"" << pair.first << "\", \"to\": \"" << neighbor << "\"}";

                    if (++connCount < connectionGraph.getTotalConnections()) ss << ",";
                    ss << "\n";

                    addedConnections.insert({pair.first, neighbor});
                }
            }
        }

        ss << "  ]\n";
        ss << "}";

        return ss.str();
    }

private:
    void startAnalyticsThread() {
        isRunning = true;
        analyticsThread = thread([this]() {
            while (isRunning) {
                this_thread::sleep_for(chrono::seconds(2));
                updateNetworkVisualization();
            }
        });
    }

    void stopAnalyticsThread() {
        isRunning = false;
        if (analyticsThread.joinable()) {
            analyticsThread.join();
        }
    }
};

// ==================== HTML GUI GENERATOR ====================

class HTMLGUIGenerator {
private:
    EnergyTradingPlatform& platform;

public:
    HTMLGUIGenerator(EnergyTradingPlatform& plat) : platform(plat) {}

    void generateHTML() {
        ofstream file("energy_trading_platform.html");

        file << "<!DOCTYPE html>\n";
        file << "<html lang=\"en\">\n";
        file << "<head>\n";
        file << "    <meta charset=\"UTF-8\">\n";
        file << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        file << "    <title>⚡ NEXUS | P2P Energy Trading Platform</title>\n";
        file << "    <link href=\"https://fonts.googleapis.com/css2?family=Orbitron:wght@400;500;700;900&family=Exo+2:wght@300;400;500;600;700&display=swap\" rel=\"stylesheet\">\n";
        file << "    <script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n";
        file << "    <style>\n";
        file << "        * { margin: 0; padding: 0; box-sizing: border-box; }\n";
        file << "        :root {\n";
        file << "            --primary: #00f2fe; --secondary: #4facfe; --accent: #00ff88;\n";
        file << "            --danger: #ff2d75; --dark: #0a0a1a; --darker: #050510;\n";
        file << "            --card-bg: rgba(16, 18, 27, 0.8); --glass: rgba(255, 255, 255, 0.05);\n";
        file << "            --neon-glow: 0 0 20px var(--primary);\n";
        file << "        }\n";
        file << "        body {\n";
        file << "            font-family: 'Exo 2', sans-serif;\n";
        file << "            background: linear-gradient(135deg, var(--darker) 0%, var(--dark) 50%, #0f1b2b 100%);\n";
        file << "            color: #ffffff; min-height: 100vh; overflow-x: hidden; position: relative;\n";
        file << "        }\n";
        file << "        .container { max-width: 1800px; margin: 0 auto; padding: 20px; }\n";
        file << "        .header {\n";
        file << "            text-align: center; padding: 40px 20px; background: var(--card-bg);\n";
        file << "            backdrop-filter: blur(20px); border: 1px solid var(--glass);\n";
        file << "            border-radius: 24px; margin-bottom: 40px; position: relative;\n";
        file << "            overflow: hidden; box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);\n";
        file << "        }\n";
        file << "        .header h1 {\n";
        file << "            font-family: 'Orbitron', monospace; font-size: 4em; font-weight: 900;\n";
        file << "            margin-bottom: 10px; background: linear-gradient(135deg, var(--primary), var(--secondary), var(--accent));\n";
        file << "            -webkit-background-clip: text; -webkit-text-fill-color: transparent;\n";
        file << "            text-shadow: var(--neon-glow); letter-spacing: 2px;\n";
        file << "        }\n";
        file << "        .tabs { display: flex; gap: 15px; margin-bottom: 40px; flex-wrap: wrap; justify-content: center; }\n";
        file << "        .tab {\n";
        file << "            padding: 18px 35px; background: var(--card-bg); backdrop-filter: blur(10px);\n";
        file << "            border: 1px solid var(--glass); border-radius: 15px; cursor: pointer;\n";
        file << "            font-family: 'Orbitron', monospace; font-size: 16px; font-weight: 500;\n";
        file << "            color: #fff; transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);\n";
        file << "            position: relative; overflow: hidden;\n";
        file << "        }\n";
        file << "        .tab:hover { transform: translateY(-5px) scale(1.05); border-color: var(--primary); box-shadow: var(--neon-glow); }\n";
        file << "        .tab.active { background: linear-gradient(135deg, var(--primary), var(--secondary)); border-color: transparent; box-shadow: var(--neon-glow); transform: translateY(-2px); }\n";
        file << "        .tab-content { display: none; animation: fadeIn 0.5s ease-out; }\n";
        file << "        .tab-content.active { display: block; }\n";
        file << "        @keyframes fadeIn { from { opacity: 0; transform: translateY(20px); } to { opacity: 1; transform: translateY(0); } }\n";
        file << "        .stats-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(280px, 1fr)); gap: 25px; margin-bottom: 40px; }\n";
        file << "        .stat-card {\n";
        file << "            background: var(--card-bg); backdrop-filter: blur(20px); padding: 30px;\n";
        file << "            border-radius: 20px; border: 1px solid var(--glass); position: relative;\n";
        file << "            overflow: hidden; transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);\n";
        file << "            cursor: pointer;\n";
        file << "        }\n";
        file << "        .stat-card:hover { transform: translateY(-10px) scale(1.02); border-color: var(--primary); box-shadow: 0 20px 40px rgba(0, 242, 254, 0.2); }\n";
        file << "        .stat-card h3 { font-size: 14px; opacity: 0.7; margin-bottom: 15px; font-weight: 400; text-transform: uppercase; letter-spacing: 1px; }\n";
        file << "        .stat-card .value { font-size: 42px; font-weight: 700; font-family: 'Orbitron', monospace; background: linear-gradient(135deg, var(--primary), var(--accent)); -webkit-background-clip: text; -webkit-text-fill-color: transparent; }\n";
        file << "        .card { background: var(--card-bg); backdrop-filter: blur(20px); padding: 40px; border-radius: 24px; border: 1px solid var(--glass); margin-bottom: 30px; position: relative; overflow: hidden; transition: all 0.3s ease; }\n";
        file << "        .card:hover { border-color: var(--primary); box-shadow: 0 15px 30px rgba(0, 242, 254, 0.1); }\n";
        file << "        .card h2 { margin-bottom: 30px; font-size: 28px; font-family: 'Orbitron', monospace; color: var(--primary); display: flex; align-items: center; gap: 15px; }\n";
        file << "        .grid-2 { display: grid; grid-template-columns: 1fr 1fr; gap: 30px; }\n";
        file << "        .form-group { margin-bottom: 25px; position: relative; }\n";
        file << "        .form-group label { display: block; margin-bottom: 12px; font-weight: 600; color: var(--primary); font-family: 'Orbitron', monospace; }\n";
        file << "        .form-group input, .form-group select {\n";
        file << "            width: 100%; padding: 18px 20px; border-radius: 15px; border: 2px solid var(--glass);\n";
        file << "            background: rgba(255, 255, 255, 0.05); color: #fff; font-size: 16px;\n";
        file << "            font-family: 'Exo 2', sans-serif; transition: all 0.3s ease;\n";
        file << "        }\n";
        file << "        .form-group input:focus, .form-group select:focus {\n";
        file << "            outline: none; border-color: var(--primary); box-shadow: 0 0 20px rgba(0, 242, 254, 0.3);\n";
        file << "            background: rgba(255, 255, 255, 0.08);\n";
        file << "        }\n";
        file << "        .btn {\n";
        file << "            padding: 20px 50px; background: linear-gradient(135deg, var(--primary), var(--secondary));\n";
        file << "            border: none; border-radius: 15px; color: #fff; font-size: 18px;\n";
        file << "            font-weight: 600; font-family: 'Orbitron', monospace; cursor: pointer;\n";
        file << "            transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1); position: relative;\n";
        file << "            overflow: hidden; text-transform: uppercase; letter-spacing: 1px;\n";
        file << "        }\n";
        file << "        .btn:hover { transform: translateY(-5px) scale(1.05); box-shadow: 0 10px 25px rgba(0, 242, 254, 0.4); }\n";
        file << "        .notification {\n";
        file << "            position: fixed; top: 30px; right: 30px; padding: 25px 35px; border-radius: 15px;\n";
        file << "            font-weight: 600; display: none; z-index: 10000; animation: slideInRight 0.5s cubic-bezier(0.4, 0, 0.2, 1);\n";
        file << "            backdrop-filter: blur(20px); border: 1px solid var(--glass); font-family: 'Orbitron', monospace;\n";
        file << "        }\n";
        file << "        .notification.success { background: linear-gradient(135deg, var(--accent), #00cc6a); box-shadow: 0 10px 30px rgba(0, 255, 136, 0.3); }\n";
        file << "        .notification.error { background: linear-gradient(135deg, var(--danger), #ff1a6c); box-shadow: 0 10px 30px rgba(255, 45, 117, 0.3); }\n";
        file << "        .network-canvas { width: 100%; height: 600px; background: rgba(0, 0, 0, 0.3); border-radius: 20px; border: 1px solid var(--glass); }\n";
        file << "        .user-card { background: linear-gradient(135deg, var(--card-bg), rgba(16, 18, 27, 0.6)); padding: 25px; border-radius: 20px; border: 1px solid var(--glass); position: relative; overflow: hidden; transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1); cursor: pointer; margin: 15px 0; }\n";
        file << "        .user-card:hover { transform: translateY(-8px) scale(1.03); border-color: var(--primary); box-shadow: 0 15px 30px rgba(0, 242, 254, 0.2); }\n";
        file << "        .user-info { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; margin-top: 20px; }\n";
        file << "        .user-info span { background: rgba(255, 255, 255, 0.05); padding: 12px; border-radius: 12px; text-align: center; transition: all 0.3s ease; }\n";
        file << "        .user-info span:hover { background: rgba(0, 242, 254, 0.1); transform: scale(1.05); }\n";
        file << "        table { width: 100%; border-collapse: collapse; margin-top: 20px; }\n";
        file << "        th, td { padding: 15px; text-align: left; border-bottom: 1px solid rgba(255, 255, 255, 0.1); }\n";
        file << "        th { background: rgba(0, 242, 254, 0.1); font-weight: 600; font-family: 'Orbitron', monospace; color: var(--primary); }\n";
        file << "        tr:hover { background: rgba(0, 242, 254, 0.05); }\n";
        file << "        @media (max-width: 1200px) { .grid-2 { grid-template-columns: 1fr; } .header h1 { font-size: 3em; } }\n";
        file << "    </style>\n";
        file << "</head>\n";
        file << "<body>\n";
        file << "    <div class=\"container\">\n";
        file << "        <div class=\"header\">\n";
        file << "            <h1>⚡ NEXUS NETWORK</h1>\n";
        file << "            <p>Advanced P2P Energy Trading Platform | Decentralized Power Grid</p>\n";
        file << "        </div>\n";

        file << "        <div class=\"tabs\">\n";
        file << "            <button class=\"tab active\" onclick=\"showTab('dashboard')\">📊 DASHBOARD</button>\n";
        file << "            <button class=\"tab\" onclick=\"showTab('trade')\">⚡ EXECUTE TRADE</button>\n";
        file << "            <button class=\"tab\" onclick=\"showTab('suggestions')\">💡 SMART SUGGESTIONS</button>\n";
        file << "            <button class=\"tab\" onclick=\"showTab('users')\">👥 NETWORK NODES</button>\n";
        file << "            <button class=\"tab\" onclick=\"showTab('transactions')\">📈 TRANSACTION LEDGER</button>\n";
        file << "            <button class=\"tab\" onclick=\"showTab('network')\">🌐 NETWORK TOPOLOGY</button>\n";
        file << "        </div>\n";

        file << "        <div id=\"notification\" class=\"notification\"></div>\n";

        // Dashboard Tab
        file << "        <div id=\"dashboard\" class=\"tab-content active\">\n";
        file << "            <div class=\"stats-grid\">\n";

        auto stats = platform.getMarketStats();
        file << "                <div class=\"stat-card\">\n";
        file << "                    <h3>Total Energy Traded</h3>\n";
        file << "                    <div class=\"value\">" << fixed << setprecision(2) << stats["total_energy_traded"] << " kWh</div>\n";
        file << "                </div>\n";

        file << "                <div class=\"stat-card\">\n";
        file << "                    <h3>Platform Revenue</h3>\n";
        file << "                    <div class=\"value\">₹" << fixed << setprecision(2) << stats["total_revenue"] << "</div>\n";
        file << "                </div>\n";

        file << "                <div class=\"stat-card\">\n";
        file << "                    <h3>Active Producers</h3>\n";
        file << "                    <div class=\"value\">" << stats["active_sellers"] << "</div>\n";
        file << "                </div>\n";

        file << "                <div class=\"stat-card\">\n";
        file << "                    <h3>Active Consumers</h3>\n";
        file << "                    <div class=\"value\">" << stats["active_buyers"] << "</div>\n";
        file << "                </div>\n";

        file << "                <div class=\"stat-card\">\n";
        file << "                    <h3>Network Connections</h3>\n";
        file << "                    <div class=\"value\">" << stats["total_connections"] << "</div>\n";
        file << "                </div>\n";

        file << "                <div class=\"stat-card\">\n";
        file << "                    <h3>Avg Energy Price</h3>\n";
        file << "                    <div class=\"value\">₹" << fixed << setprecision(3) << stats["average_price"] << "</div>\n";
        file << "                </div>\n";

        file << "            </div>\n";
        file << "            \n";
        file << "            <div class=\"grid-2\">\n";
        file << "                <div class=\"card\">\n";
        file << "                    <h2>📈 REAL-TIME PRICE TRACKING</h2>\n";
        file << "                    <canvas id=\"priceChart\" width=\"400\" height=\"300\"></canvas>\n";
        file << "                </div>\n";
        file << "                <div class=\"card\">\n";
        file << "                    <h2>📊 TRADE VOLUME ANALYSIS</h2>\n";
        file << "                    <canvas id=\"volumeChart\" width=\"400\" height=\"300\"></canvas>\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "            \n";
        file << "            <div class=\"card\">\n";
        file << "                <h2>🔄 RECENT TRANSACTIONS</h2>\n";
        file << "                <div id=\"recentTransactions\">\n";
        file << "                    " << generateRecentTransactionsHTML() << "\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        // Trade Tab
        file << "        <div id=\"trade\" class=\"tab-content\">\n";
        file << "            <div class=\"card\">\n";
        file << "                <h2>⚡ ENERGY TRANSACTION CONSOLE</h2>\n";
        file << "                <form id=\"tradeForm\">\n";
        file << "                    <div class=\"grid-2\">\n";
        file << "                        <div class=\"form-group\">\n";
        file << "                            <label>SOURCE NODE (SELLER)</label>\n";
        file << "                            <select id=\"sellerId\" onchange=\"updateSellerInfo(); updatePathAnalysis();\">\n";
        file << "                                <option value=\"\">SELECT ENERGY SOURCE</option>\n";

        for (const auto& seller : platform.getSellers()) {
            file << "                                <option value=\"" << seller->id << "\" data-surplus=\"" << seller->energySurplus << "\" data-balance=\"" << seller->balance << "\">"
                 << seller->name << " [" << seller->id << "] - " << seller->energySurplus << " kWh</option>\n";
        }

        file << "                            </select>\n";
        file << "                            <div id=\"sellerInfo\" class=\"user-info\" style=\"margin-top: 10px; display: none;\"></div>\n";
        file << "                        </div>\n";
        file << "                        <div class=\"form-group\">\n";
        file << "                            <label>DESTINATION NODE (BUYER)</label>\n";
        file << "                            <select id=\"buyerId\" onchange=\"updateBuyerInfo(); updatePathAnalysis();\">\n";
        file << "                                <option value=\"\">SELECT POWER CONSUMER</option>\n";

        for (const auto& buyer : platform.getBuyers()) {
            file << "                                <option value=\"" << buyer->id << "\" data-demand=\"" << buyer->energyDemand << "\" data-balance=\"" << buyer->balance << "\">"
                 << buyer->name << " [" << buyer->id << "] - " << buyer->energyDemand << " kWh needed</option>\n";
        }

        file << "                            </select>\n";
        file << "                            <div id=\"buyerInfo\" class=\"user-info\" style=\"margin-top: 10px; display: none;\"></div>\n";
        file << "                        </div>\n";
        file << "                    </div>\n";
        file << "                    <div class=\"grid-2\">\n";
        file << "                        <div class=\"form-group\">\n";
        file << "                            <label>ENERGY QUANTITY (kWh)</label>\n";
        file << "                            <input type=\"number\" id=\"energy\" step=\"0.01\" placeholder=\"ENTER ENERGY AMOUNT\" oninput=\"calculateTotal(); validateTrade();\">\n";
        file << "                        </div>\n";
        file << "                        <div class=\"form-group\">\n";
        file << "                            <label>PRICE PER kWh (₹)</label>\n";
        file << "                            <input type=\"number\" id=\"price\" step=\"0.01\" placeholder=\"ENTER PRICE PER UNIT\" oninput=\"calculateTotal(); validateTrade();\">\n";
        file << "                        </div>\n";
        file << "                    </div>\n";
        file << "                    <div class=\"form-group\">\n";
        file << "                        <label>TOTAL COST: ₹<span id=\"totalCost\">0.00</span></label>\n";
        file << "                    </div>\n";
        file << "                    <div id=\"tradeValidation\" style=\"margin: 20px 0; padding: 15px; border-radius: 10px; display: none;\"></div>\n";
        file << "                    <button type=\"button\" class=\"btn\" onclick=\"executeRealTrade()\">INITIATE ENERGY TRANSFER</button>\n";
        file << "                </form>\n";
        file << "            </div>\n";
        file << "            \n";
        file << "            <div class=\"card\">\n";
        file << "                <h2>🔍 LIVE NETWORK PATH ANALYSIS</h2>\n";
        file << "                <div id=\"pathAnalysis\" style=\"padding: 20px; background: rgba(0, 242, 254, 0.1); border-radius: 15px; margin-top: 15px;\">\n";
        file << "                    <p>Select seller and buyer to analyze optimal trading path</p>\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        // Smart Suggestions Tab
        file << "        <div id=\"suggestions\" class=\"tab-content\">\n";
        file << "            <div class=\"card\">\n";
        file << "                <h2>💡 AI-POWERED TRADE SUGGESTIONS</h2>\n";
        file << "                <p>Smart algorithm analyzes network patterns and user needs to suggest optimal trades</p>\n";
        file << "                <div id=\"suggestionsList\">\n";
        file << "                    " << generateSuggestionsHTML() << "\n";
        file << "                </div>\n";
        file << "                <button class=\"btn\" style=\"margin-top: 20px;\" onclick=\"refreshSuggestions()\">🔄 Refresh Suggestions</button>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        // Users Tab
        file << "        <div id=\"users\" class=\"tab-content\">\n";
        file << "            <div class=\"grid-2\">\n";
        file << "                <div class=\"card\">\n";
        file << "                    <h2>🔋 ENERGY PRODUCERS</h2>\n";
        file << "                    <div id=\"producersList\">\n";
        file << "                        " << generateProducersHTML() << "\n";
        file << "                    </div>\n";
        file << "                </div>\n";
        file << "                <div class=\"card\">\n";
        file << "                    <h2>💡 ENERGY CONSUMERS</h2>\n";
        file << "                    <div id=\"consumersList\">\n";
        file << "                        " << generateConsumersHTML() << "\n";
        file << "                    </div>\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        // Transactions Tab
        file << "        <div id=\"transactions\" class=\"tab-content\">\n";
        file << "            <div class=\"card\">\n";
        file << "                <h2>📈 TRANSACTION LEDGER</h2>\n";
        file << "                <div id=\"transactionTable\">\n";
        file << "                    " << generateTransactionTableHTML() << "\n";
        file << "                </div>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        // Network Topology Tab
        file << "        <div id=\"network\" class=\"tab-content\">\n";
        file << "            <div class=\"card\">\n";
        file << "                <h2>🌐 LIVE NETWORK TOPOLOGY</h2>\n";
        file << "                <div style=\"display: flex; gap: 20px; margin-bottom: 20px;\">\n";
        file << "                    <button class=\"btn\" onclick=\"refreshNetwork()\">🔄 Refresh Network</button>\n";
        file << "                    <button class=\"btn\" onclick=\"simulateNetworkGrowth()\">🌱 Simulate Growth</button>\n";
        file << "                    <div style=\"display: flex; gap: 15px; align-items: center;\">\n";
        file << "                        <div style=\"display: flex; align-items: center; gap: 5px;\">\n";
        file << "                            <div style=\"width: 12px; height: 12px; background: #00ff88; border-radius: 50%;\"></div>\n";
        file << "                            <span>Producers</span>\n";
        file << "                        </div>\n";
        file << "                        <div style=\"display: flex; align-items: center; gap: 5px;\">\n";
        file << "                            <div style=\"width: 12px; height: 12px; background: #4facfe; border-radius: 50%;\"></div>\n";
        file << "                            <span>Consumers</span>\n";
        file << "                        </div>\n";
        file << "                        <div style=\"display: flex; align-items: center; gap: 5px;\">\n";
        file << "                            <div style=\"width: 12px; height: 12px; background: #ff2d75; border-radius: 50%;\"></div>\n";
        file << "                            <span>Storage</span>\n";
        file << "                        </div>\n";
        file << "                    </div>\n";
        file << "                </div>\n";
        file << "                <canvas id=\"networkCanvas\" class=\"network-canvas\"></canvas>\n";
        file << "            </div>\n";
        file << "        </div>\n";

        file << "    </div>\n";

        // JavaScript
        file << "    <script>\n";
        file << "        let networkData = " << platform.getNetworkJSON() << ";\n";
        file << "        let marketData = {\n";
        file << "            prices: " << generatePriceHistoryJSON() << ",\n";
        file << "            volumes: " << generateVolumeHistoryJSON() << ",\n";
        file << "            transactions: " << generateTransactionsJSON() << "\n";
        file << "        };\n";
        file << "        \n";
        file << "        let priceChart, volumeChart;\n";
        file << "        let animationId = null;\n";
        file << "        \n";
        file << "        document.addEventListener('DOMContentLoaded', function() {\n";
        file << "            initCharts();\n";
        file << "            drawNetwork();\n";
        file << "            startRealTimeUpdates();\n";
        file << "            updateUserLists();\n";
        file << "        });\n";
        file << "        \n";
        file << "        function initCharts() {\n";
        file << "            const priceCtx = document.getElementById('priceChart').getContext('2d');\n";
        file << "            priceChart = new Chart(priceCtx, {\n";
        file << "                type: 'line',\n";
        file << "                data: {\n";
        file << "                    labels: marketData.prices.map(p => new Date(p.timestamp * 1000).toLocaleTimeString()),\n";
        file << "                    datasets: [{\n";
        file << "                        label: 'Energy Price (₹/kWh)',\n";
        file << "                        data: marketData.prices.map(p => p.price),\n";
        file << "                        borderColor: '#00f2fe',\n";
        file << "                        backgroundColor: 'rgba(0, 242, 254, 0.1)',\n";
        file << "                        borderWidth: 3,\n";
        file << "                        fill: true,\n";
        file << "                        tension: 0.4\n";
        file << "                    }]\n";
        file << "                },\n";
        file << "                options: {\n";
        file << "                    responsive: true,\n";
        file << "                    animation: {\n";
        file << "                        duration: 1000,\n";
        file << "                        easing: 'easeOutQuart'\n";
        file << "                    },\n";
        file << "                    plugins: {\n";
        file << "                        legend: { labels: { color: '#fff', font: { family: 'Orbitron', size: 14 } } },\n";
        file << "                        tooltip: { backgroundColor: 'rgba(0, 0, 0, 0.8)', titleColor: '#00f2fe', bodyColor: '#fff' }\n";
        file << "                    },\n";
        file << "                    scales: {\n";
        file << "                        y: { \n";
        file << "                            beginAtZero: false, \n";
        file << "                            ticks: { color: '#fff', font: { size: 12 } },\n";
        file << "                            grid: { color: 'rgba(255,255,255,0.1)' },\n";
        file << "                            title: { display: true, text: 'Price (₹/kWh)', color: '#fff' }\n";
        file << "                        },\n";
        file << "                        x: { \n";
        file << "                            ticks: { color: '#fff', maxTicksLimit: 8, font: { size: 11 } },\n";
        file << "                            grid: { color: 'rgba(255,255,255,0.1)' },\n";
        file << "                            title: { display: true, text: 'Time', color: '#fff' }\n";
        file << "                        }\n";
        file << "                    }\n";
        file << "                }\n";
        file << "            });\n";
        file << "            \n";
        file << "            const volumeCtx = document.getElementById('volumeChart').getContext('2d');\n";
        file << "            volumeChart = new Chart(volumeCtx, {\n";
        file << "                type: 'bar',\n";
        file << "                data: {\n";
        file << "                    labels: marketData.volumes.map(v => new Date(v.timestamp * 1000).toLocaleTimeString()),\n";
        file << "                    datasets: [{\n";
        file << "                        label: 'Trade Volume (kWh)',\n";
        file << "                        data: marketData.volumes.map(v => v.volume),\n";
        file << "                        backgroundColor: 'rgba(0, 255, 136, 0.7)',\n";
        file << "                        borderColor: '#00ff88',\n";
        file << "                        borderWidth: 2,\n";
        file << "                        borderRadius: 5\n";
        file << "                    }]\n";
        file << "                },\n";
        file << "                options: {\n";
        file << "                    responsive: true,\n";
        file << "                    animation: {\n";
        file << "                        duration: 1000,\n";
        file << "                        easing: 'easeOutQuart'\n";
        file << "                    },\n";
        file << "                    plugins: {\n";
        file << "                        legend: { labels: { color: '#fff', font: { family: 'Orbitron', size: 14 } } },\n";
        file << "                        tooltip: { backgroundColor: 'rgba(0, 0, 0, 0.8)', titleColor: '#00ff88', bodyColor: '#fff' }\n";
        file << "                    },\n";
        file << "                    scales: {\n";
        file << "                        y: { \n";
        file << "                            beginAtZero: true, \n";
        file << "                            ticks: { color: '#fff', font: { size: 12 } },\n";
        file << "                            grid: { color: 'rgba(255,255,255,0.1)' },\n";
        file << "                            title: { display: true, text: 'Volume (kWh)', color: '#fff' }\n";
        file << "                        },\n";
        file << "                        x: { \n";
        file << "                            ticks: { color: '#fff', maxTicksLimit: 8, font: { size: 11 } },\n";
        file << "                            grid: { color: 'rgba(255,255,255,0.1)' },\n";
        file << "                            title: { display: true, text: 'Time', color: '#fff' }\n";
        file << "                        }\n";
        file << "                    }\n";
        file << "                }\n";
        file << "            });\n";
        file << "        }\n";
        file << "        \n";
        file << "        function startRealTimeUpdates() {\n";
        file << "            setInterval(updateCharts, 2000);\n";
        file << "            setInterval(updateRecentTransactions, 3000);\n";
        file << "            setInterval(animateNetwork, 50);\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateCharts() {\n";
        file << "            if (priceChart && marketData.prices.length > 0) {\n";
        file << "                const newPrice = marketData.prices[marketData.prices.length - 1].price * (0.95 + Math.random() * 0.1);\n";
        file << "                const newVolume = Math.random() * 100 + 20;\n";
        file << "                \n";
        file << "                marketData.prices.push({\n";
        file << "                    timestamp: Math.floor(Date.now() / 1000),\n";
        file << "                    price: newPrice\n";
        file << "                });\n";
        file << "                \n";
        file << "                marketData.volumes.push({\n";
        file << "                    timestamp: Math.floor(Date.now() / 1000),\n";
        file << "                    volume: newVolume\n";
        file << "                });\n";
        file << "                \n";
        file << "                if (marketData.prices.length > 20) {\n";
        file << "                    marketData.prices.shift();\n";
        file << "                    marketData.volumes.shift();\n";
        file << "                }\n";
        file << "                \n";
        file << "                priceChart.data.labels = marketData.prices.map(p => new Date(p.timestamp * 1000).toLocaleTimeString());\n";
        file << "                priceChart.data.datasets[0].data = marketData.prices.map(p => p.price);\n";
        file << "                priceChart.update('none');\n";
        file << "                \n";
        file << "                volumeChart.data.labels = marketData.volumes.map(v => new Date(v.timestamp * 1000).toLocaleTimeString());\n";
        file << "                volumeChart.data.datasets[0].data = marketData.volumes.map(v => v.volume);\n";
        file << "                volumeChart.update('none');\n";
        file << "            }\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateRecentTransactions() {\n";
        file << "            const container = document.getElementById('recentTransactions');\n";
        file << "            if (!container) return;\n";
        file << "            \n";
        file << "            const transactions = marketData.transactions.slice(-5).reverse();\n";
        file << "            let html = '';\n";
        file << "            \n";
        file << "            transactions.forEach(txn => {\n";
        file << "                html += `\n";
        file << "                    <div style='padding: 15px; margin: 10px 0; background: rgba(255,255,255,0.05); border-radius: 10px; border-left: 4px solid #00f2fe;'>\n";
        file << "                        <div style='display: flex; justify-content: space-between; align-items: center;'>\n";
        file << "                            <div>\n";
        file << "                                <strong>${txn.sellerId} → ${txn.buyerId}</strong>\n";
        file << "                                <div style='font-size: 0.9em; opacity: 0.8;'>${txn.energyAmount.toFixed(1)} kWh • ₹${txn.pricePerUnit.toFixed(3)}/kWh</div>\n";
        file << "                            </div>\n";
        file << "                            <div style='text-align: right;'>\n";
        file << "                                <strong>₹${txn.totalPrice.toFixed(2)}</strong>\n";
        file << "                                <div style='font-size: 0.8em; opacity: 0.7;'>${new Date(txn.timestamp * 1000).toLocaleTimeString()}</div>\n";
        file << "                            </div>\n";
        file << "                        </div>\n";
        file << "                    </div>\n";
        file << "                `;\n";
        file << "            });\n";
        file << "            \n";
        file << "            container.innerHTML = html;\n";
        file << "        }\n";
        file << "        \n";
        file << "        function drawNetwork() {\n";
        file << "            const canvas = document.getElementById('networkCanvas');\n";
        file << "            if (!canvas) return;\n";
        file << "            \n";
        file << "            const ctx = canvas.getContext('2d');\n";
        file << "            canvas.width = canvas.offsetWidth;\n";
        file << "            canvas.height = canvas.offsetHeight;\n";
        file << "            \n";
        file << "            ctx.clearRect(0, 0, canvas.width, canvas.height);\n";
        file << "            \n";
        file << "            // Draw connections with animation\n";
        file << "            ctx.strokeStyle = 'rgba(0, 242, 254, 0.6)';\n";
        file << "            ctx.lineWidth = 2;\n";
        file << "            ctx.setLineDash([5, 3]);\n";
        file << "            \n";
        file << "            networkData.connections.forEach(conn => {\n";
        file << "                const fromNode = networkData.nodes.find(n => n.id === conn.from);\n";
        file << "                const toNode = networkData.nodes.find(n => n.id === conn.to);\n";
        file << "                \n";
        file << "                if (fromNode && toNode) {\n";
        file << "                    ctx.beginPath();\n";
        file << "                    ctx.moveTo(fromNode.x, fromNode.y);\n";
        file << "                    ctx.lineTo(toNode.x, toNode.y);\n";
        file << "                    ctx.stroke();\n";
        file << "                    \n";
        file << "                    // Draw energy flow animation\n";
        file << "                    const progress = (Date.now() / 1000) % 1;\n";
        file << "                    const x = fromNode.x + (toNode.x - fromNode.x) * progress;\n";
        file << "                    const y = fromNode.y + (toNode.y - fromNode.y) * progress;\n";
        file << "                    \n";
        file << "                    ctx.beginPath();\n";
        file << "                    ctx.arc(x, y, 3, 0, 2 * Math.PI);\n";
        file << "                    ctx.fillStyle = '#00ff88';\n";
        file << "                    ctx.fill();\n";
        file << "                }\n";
        file << "            });\n";
        file << "            \n";
        file << "            ctx.setLineDash([]);\n";
        file << "            \n";
        file << "            // Draw nodes with pulsing animation\n";
        file << "            networkData.nodes.forEach(node => {\n";
        file << "                const pulse = Math.sin(Date.now() / 1000) * 0.2 + 1;\n";
        file << "                const size = 20 * pulse;\n";
        file << "                \n";
        file << "                const gradient = ctx.createRadialGradient(node.x, node.y, 0, node.x, node.y, size);\n";
        file << "                \n";
        file << "                if (node.type === 'producer') {\n";
        file << "                    gradient.addColorStop(0, '#00ff88');\n";
        file << "                    gradient.addColorStop(1, 'rgba(0, 255, 136, 0.3)');\n";
        file << "                } else if (node.type === 'consumer') {\n";
        file << "                    gradient.addColorStop(0, '#4facfe');\n";
        file << "                    gradient.addColorStop(1, 'rgba(79, 172, 254, 0.3)');\n";
        file << "                } else {\n";
        file << "                    gradient.addColorStop(0, '#ff2d75');\n";
        file << "                    gradient.addColorStop(1, 'rgba(255, 45, 117, 0.3)');\n";
        file << "                }\n";
        file << "                \n";
        file << "                ctx.beginPath();\n";
        file << "                ctx.arc(node.x, node.y, size, 0, 2 * Math.PI);\n";
        file << "                ctx.fillStyle = gradient;\n";
        file << "                ctx.fill();\n";
        file << "                \n";
        file << "                ctx.strokeStyle = '#ffffff';\n";
        file << "                ctx.lineWidth = 2;\n";
        file << "                ctx.stroke();\n";
        file << "                \n";
        file << "                // Node label\n";
        file << "                ctx.fillStyle = '#ffffff';\n";
        file << "                ctx.font = 'bold 12px Orbitron';\n";
        file << "                ctx.textAlign = 'center';\n";
        file << "                ctx.textBaseline = 'middle';\n";
        file << "                ctx.fillText(node.id, node.x, node.y);\n";
        file << "                \n";
        file << "                // Energy info\n";
        file << "                ctx.font = '10px Arial';\n";
        file << "                if (node.surplus > 0) {\n";
        file << "                    ctx.fillText(node.surplus.toFixed(1) + ' kWh', node.x, node.y + 20);\n";
        file << "                } else if (node.demand > 0) {\n";
        file << "                    ctx.fillText('Need: ' + node.demand.toFixed(1) + ' kWh', node.x, node.y + 20);\n";
        file << "                }\n";
        file << "            });\n";
        file << "        }\n";
        file << "        \n";
        file << "        function animateNetwork() {\n";
        file << "            drawNetwork();\n";
        file << "        }\n";
        file << "        \n";
        file << "        function executeRealTrade() {\n";
        file << "            const sellerId = document.getElementById('sellerId').value;\n";
        file << "            const buyerId = document.getElementById('buyerId').value;\n";
        file << "            const energy = parseFloat(document.getElementById('energy').value);\n";
        file << "            const price = parseFloat(document.getElementById('price').value);\n";
        file << "            \n";
        file << "            if (!sellerId || !buyerId || !energy || !price) {\n";
        file << "                showNotification('Please fill all fields with valid values!', 'error');\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const tradeResult = simulateTrade(sellerId, buyerId, energy, price);\n";
        file << "            \n";
        file << "            if (tradeResult.success) {\n";
        file << "                showNotification(`⚡ Trade Successful! ${energy}kWh transferred from ${sellerId} to ${buyerId} for ₹${(energy*price).toFixed(2)}`, 'success');\n";
        file << "                updateMarketData(tradeResult.transaction);\n";
        file << "                updateNetworkConnections(sellerId, buyerId);\n";
        file << "                resetTradeForm();\n";
        file << "                refreshAllData();\n";
        file << "            } else {\n";
        file << "                showNotification(`Trade Failed: ${tradeResult.message}`, 'error');\n";
        file << "            }\n";
        file << "        }\n";
        file << "        \n";
        file << "        function simulateTrade(sellerId, buyerId, energy, price) {\n";
        file << "            const seller = networkData.nodes.find(n => n.id === sellerId);\n";
        file << "            const buyer = networkData.nodes.find(n => n.id === buyerId);\n";
        file << "            \n";
        file << "            if (!seller || !buyer) {\n";
        file << "                return { success: false, message: 'Invalid users' };\n";
        file << "            }\n";
        file << "            \n";
        file << "            if (energy > seller.surplus) {\n";
        file << "                return { success: false, message: 'Insufficient seller energy' };\n";
        file << "            }\n";
        file << "            \n";
        file << "            if (energy > buyer.demand) {\n";
        file << "                return { success: false, message: 'Exceeds buyer demand' };\n";
        file << "            }\n";
        file << "            \n";
        file << "            const totalCost = energy * price;\n";
        file << "            if (totalCost > buyer.balance) {\n";
        file << "                return { success: false, message: 'Insufficient buyer funds' };\n";
        file << "            }\n";
        file << "            \n";
        file << "            seller.surplus -= energy;\n";
        file << "            buyer.demand -= energy;\n";
        file << "            seller.balance += totalCost * 0.98;\n";
        file << "            buyer.balance -= totalCost;\n";
        file << "            \n";
        file << "            const newTransaction = {\n";
        file << "                id: 'TXN' + Date.now(),\n";
        file << "                sellerId,\n";
        file << "                buyerId,\n";
        file << "                energyAmount: energy,\n";
        file << "                pricePerUnit: price,\n";
        file << "                totalPrice: totalCost,\n";
        file << "                timestamp: Math.floor(Date.now() / 1000)\n";
        file << "            };\n";
        file << "            \n";
        file << "            return { success: true, transaction: newTransaction };\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateMarketData(transaction) {\n";
        file << "            marketData.prices.push({\n";
        file << "                timestamp: transaction.timestamp,\n";
        file << "                price: transaction.pricePerUnit\n";
        file << "            });\n";
        file << "            marketData.volumes.push({\n";
        file << "                timestamp: transaction.timestamp,\n";
        file << "                volume: transaction.energyAmount\n";
        file << "            });\n";
        file << "            marketData.transactions.push(transaction);\n";
        file << "            \n";
        file << "            updateCharts();\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateNetworkConnections(sellerId, buyerId) {\n";
        file << "            let connectionExists = networkData.connections.some(conn => \n";
        file << "                (conn.from === sellerId && conn.to === buyerId) ||\n";
        file << "                (conn.from === buyerId && conn.to === sellerId)\n";
        file << "            );\n";
        file << "            \n";
        file << "            if (!connectionExists) {\n";
        file << "                networkData.connections.push({from: sellerId, to: buyerId});\n";
        file << "            }\n";
        file << "            \n";
        file << "            drawNetwork();\n";
        file << "        }\n";
        file << "        \n";
        file << "        function refreshAllData() {\n";
        file << "            updateCharts();\n";
        file << "            drawNetwork();\n";
        file << "            refreshSuggestions();\n";
        file << "            updateUserLists();\n";
        file << "            updateTransactionTable();\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateSellerInfo() {\n";
        file << "            const select = document.getElementById('sellerId');\n";
        file << "            const infoDiv = document.getElementById('sellerInfo');\n";
        file << "            const selectedOption = select.options[select.selectedIndex];\n";
        file << "            \n";
        file << "            if (selectedOption.value) {\n";
        file << "                const surplus = selectedOption.getAttribute('data-surplus');\n";
        file << "                const balance = selectedOption.getAttribute('data-balance');\n";
        file << "                infoDiv.innerHTML = `\n";
        file << "                    <div style='display: grid; grid-template-columns: 1fr 1fr; gap: 10px;'>\n";
        file << "                        <span>Available: <strong>${surplus} kWh</strong></span>\n";
        file << "                        <span>Balance: <strong>₹${parseFloat(balance).toFixed(2)}</strong></span>\n";
        file << "                    </div>\n";
        file << "                `;\n";
        file << "                infoDiv.style.display = 'block';\n";
        file << "            } else {\n";
        file << "                infoDiv.style.display = 'none';\n";
        file << "            }\n";
        file << "            updatePathAnalysis();\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateBuyerInfo() {\n";
        file << "            const select = document.getElementById('buyerId');\n";
        file << "            const infoDiv = document.getElementById('buyerInfo');\n";
        file << "            const selectedOption = select.options[select.selectedIndex];\n";
        file << "            \n";
        file << "            if (selectedOption.value) {\n";
        file << "                const demand = selectedOption.getAttribute('data-demand');\n";
        file << "                const balance = selectedOption.getAttribute('data-balance');\n";
        file << "                infoDiv.innerHTML = `\n";
        file << "                    <div style='display: grid; grid-template-columns: 1fr 1fr; gap: 10px;'>\n";
        file << "                        <span>Required: <strong>${demand} kWh</strong></span>\n";
        file << "                        <span>Balance: <strong>₹${parseFloat(balance).toFixed(2)}</strong></span>\n";
        file << "                    </div>\n";
        file << "                `;\n";
        file << "                infoDiv.style.display = 'block';\n";
        file << "            } else {\n";
        file << "                infoDiv.style.display = 'none';\n";
        file << "            }\n";
        file << "            updatePathAnalysis();\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updatePathAnalysis() {\n";
        file << "            const sellerId = document.getElementById('sellerId').value;\n";
        file << "            const buyerId = document.getElementById('buyerId').value;\n";
        file << "            const analysisDiv = document.getElementById('pathAnalysis');\n";
        file << "            \n";
        file << "            if (!sellerId || !buyerId) {\n";
        file << "                analysisDiv.innerHTML = '<p>Select seller and buyer to analyze optimal trading path</p>';\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const path = findShortestPath(sellerId, buyerId);\n";
        file << "            \n";
        file << "            if (path.length > 0) {\n";
        file << "                analysisDiv.innerHTML = `\n";
        file << "                    <h4>🌐 Optimal Trading Path Found</h4>\n";
        file << "                    <p><strong>Route:</strong> ${path.join(' → ')}</p>\n";
        file << "                    <p><strong>Hops:</strong> ${path.length - 1} | <strong>Efficiency:</strong> ${((1/path.length)*100).toFixed(1)}%</p>\n";
        file << "                    <p style='color: #00ff88; margin-top: 10px;'>✓ Direct peer-to-peer connection available</p>\n";
        file << "                `;\n";
        file << "            } else {\n";
        file << "                analysisDiv.innerHTML = `\n";
        file << "                    <h4>🌐 Network Path Analysis</h4>\n";
        file << "                    <p style='color: #ff2d75;'>⚠️ No direct path found. Users may need intermediate connections.</p>\n";
        file << "                `;\n";
        file << "            }\n";
        file << "        }\n";
        file << "        \n";
        file << "        function findShortestPath(startId, endId) {\n";
        file << "            if (startId === endId) return [startId];\n";
        file << "            \n";
        file << "            const queue = [[startId]];\n";
        file << "            const visited = new Set([startId]);\n";
        file << "            \n";
        file << "            while (queue.length > 0) {\n";
        file << "                const path = queue.shift();\n";
        file << "                const node = path[path.length - 1];\n";
        file << "                \n";
        file << "                const neighbors = [\n";
        file << "                    ...networkData.connections.filter(conn => conn.from === node).map(conn => conn.to),\n";
        file << "                    ...networkData.connections.filter(conn => conn.to === node).map(conn => conn.from)\n";
        file << "                ];\n";
        file << "                \n";
        file << "                for (const neighbor of neighbors) {\n";
        file << "                    if (neighbor === endId) {\n";
        file << "                        return [...path, endId];\n";
        file << "                    }\n";
        file << "                    \n";
        file << "                    if (!visited.has(neighbor)) {\n";
        file << "                        visited.add(neighbor);\n";
        file << "                        queue.push([...path, neighbor]);\n";
        file << "                    }\n";
        file << "                }\n";
        file << "            }\n";
        file << "            \n";
        file << "            return [];\n";
        file << "        }\n";
        file << "        \n";
        file << "        function calculateTotal() {\n";
        file << "            const energy = parseFloat(document.getElementById('energy').value) || 0;\n";
        file << "            const price = parseFloat(document.getElementById('price').value) || 0;\n";
        file << "            const total = energy * price;\n";
        file << "            document.getElementById('totalCost').textContent = total.toFixed(2);\n";
        file << "        }\n";
        file << "        \n";
        file << "        function validateTrade() {\n";
        file << "            const sellerSelect = document.getElementById('sellerId');\n";
        file << "            const buyerSelect = document.getElementById('buyerId');\n";
        file << "            const energy = parseFloat(document.getElementById('energy').value) || 0;\n";
        file << "            const price = parseFloat(document.getElementById('price').value) || 0;\n";
        file << "            const validationDiv = document.getElementById('tradeValidation');\n";
        file << "            \n";
        file << "            if (!sellerSelect.value || !buyerSelect.value || energy <= 0 || price <= 0) {\n";
        file << "                validationDiv.style.display = 'none';\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const sellerOption = sellerSelect.options[sellerSelect.selectedIndex];\n";
        file << "            const buyerOption = buyerSelect.options[buyerSelect.selectedIndex];\n";
        file << "            const sellerSurplus = parseFloat(sellerOption.getAttribute('data-surplus'));\n";
        file << "            const buyerDemand = parseFloat(buyerOption.getAttribute('data-demand'));\n";
        file << "            const buyerBalance = parseFloat(buyerOption.getAttribute('data-balance'));\n";
        file << "            \n";
        file << "            let messages = [];\n";
        file << "            let isValid = true;\n";
        file << "            \n";
        file << "            if (energy > sellerSurplus) {\n";
        file << "                messages.push('❌ Exceeds seller energy surplus');\n";
        file << "                isValid = false;\n";
        file << "            }\n";
        file << "            \n";
        file << "            if (energy > buyerDemand) {\n";
        file << "                messages.push('❌ Exceeds buyer energy demand');\n";
        file << "                isValid = false;\n";
        file << "            }\n";
        file << "            \n";
        file << "            const totalCost = energy * price;\n";
        file << "            if (totalCost > buyerBalance) {\n";
        file << "                messages.push('❌ Insufficient buyer balance');\n";
        file << "                isValid = false;\n";
        file << "            }\n";
        file << "            \n";
        file << "            if (isValid) {\n";
        file << "                messages.push('✅ Trade validation successful! Ready to execute.');\n";
        file << "                validationDiv.style.background = 'rgba(0, 255, 136, 0.1)';\n";
        file << "                validationDiv.style.border = '1px solid #00ff88';\n";
        file << "            } else {\n";
        file << "                validationDiv.style.background = 'rgba(255, 45, 117, 0.1)';\n";
        file << "                validationDiv.style.border = '1px solid #ff2d75';\n";
        file << "            }\n";
        file << "            \n";
        file << "            validationDiv.innerHTML = '<strong>Trade Validation:</strong><br>' + messages.join('<br>');\n";
        file << "            validationDiv.style.display = 'block';\n";
        file << "        }\n";
        file << "        \n";
        file << "        function resetTradeForm() {\n";
        file << "            document.getElementById('tradeForm').reset();\n";
        file << "            document.getElementById('sellerInfo').style.display = 'none';\n";
        file << "            document.getElementById('buyerInfo').style.display = 'none';\n";
        file << "            document.getElementById('tradeValidation').style.display = 'none';\n";
        file << "            document.getElementById('totalCost').textContent = '0.00';\n";
        file << "            document.getElementById('pathAnalysis').innerHTML = '<p>Select seller and buyer to analyze optimal trading path</p>';\n";
        file << "        }\n";
        file << "        \n";
        file << "        function refreshSuggestions() {\n";
        file << "            const newSuggestions = generateNewSuggestions();\n";
        file << "            displaySuggestions(newSuggestions);\n";
        file << "            showNotification('Suggestions refreshed with latest network data!', 'success');\n";
        file << "        }\n";
        file << "        \n";
        file << "        function generateNewSuggestions() {\n";
        file << "            const suggestions = [];\n";
        file << "            const producers = networkData.nodes.filter(n => n.surplus > 0);\n";
        file << "            const consumers = networkData.nodes.filter(n => n.demand > 0);\n";
        file << "            \n";
        file << "            for (let i = 0; i < Math.min(3, producers.length); i++) {\n";
        file << "                for (let j = 0; j < Math.min(3, consumers.length); j++) {\n";
        file << "                    if (producers[i] && consumers[j]) {\n";
        file << "                        const energy = Math.min(producers[i].surplus, consumers[j].demand, 50);\n";
        file << "                        const price = 0.12 + (Math.random() * 0.08);\n";
        file << "                        const score = Math.random() * 0.5 + 0.5;\n";
        file << "                        \n";
        file << "                        suggestions.push({\n";
        file << "                            sellerId: producers[i].id,\n";
        file << "                            buyerId: consumers[j].id,\n";
        file << "                            suggestedEnergy: energy,\n";
        file << "                            suggestedPrice: price,\n";
        file << "                            matchScore: score,\n";
        file << "                            reason: 'AI-optimized trade suggestion'\n";
        file << "                        });\n";
        file << "                    }\n";
        file << "                }\n";
        file << "            }\n";
        file << "            \n";
        file << "            return suggestions.sort((a, b) => b.matchScore - a.matchScore).slice(0, 5);\n";
        file << "        }\n";
        file << "        \n";
        file << "        function displaySuggestions(suggestions) {\n";
        file << "            const container = document.getElementById('suggestionsList');\n";
        file << "            if (!container) return;\n";
        file << "            \n";
        file << "            if (suggestions.length === 0) {\n";
        file << "                container.innerHTML = '<p style=\"text-align: center; padding: 40px; opacity: 0.7;\">No suggestions available. Add more users to the network.</p>';\n";
        file << "                return;\n";
        file << "            }\n";
        file << "            \n";
        file << "            let html = '';\n";
        file << "            suggestions.forEach(suggestion => {\n";
        file << "                html += `\n";
        file << "                    <div class='user-card'>\n";
        file << "                        <div style='display: flex; justify-content: space-between; align-items: start;'>\n";
        file << "                            <div>\n";
        file << "                                <h4>${suggestion.sellerId} → ${suggestion.buyerId}</h4>\n";
        file << "                                <p>${suggestion.reason}</p>\n";
        file << "                                <div style='display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 10px;'>\n";
        file << "                                    <span>Energy: <strong>${suggestion.suggestedEnergy.toFixed(1)} kWh</strong></span>\n";
        file << "                                    <span>Price: <strong>₹${suggestion.suggestedPrice.toFixed(3)}/kWh</strong></span>\n";
        file << "                                    <span>Match Score: <strong>${(suggestion.matchScore * 100).toFixed(1)}%</strong></span>\n";
        file << "                                </div>\n";
        file << "                            </div>\n";
        file << "                            <button class='btn' style='padding: 10px 20px; font-size: 14px;' onclick=\"useSuggestion('${suggestion.sellerId}', '${suggestion.buyerId}', ${suggestion.suggestedEnergy}, ${suggestion.suggestedPrice})\">Use This</button>\n";
        file << "                        </div>\n";
        file << "                    </div>\n";
        file << "                `;\n";
        file << "            });\n";
        file << "            \n";
        file << "            container.innerHTML = html;\n";
        file << "        }\n";
        file << "        \n";
        file << "        function useSuggestion(sellerId, buyerId, energy, price) {\n";
        file << "            document.getElementById('sellerId').value = sellerId;\n";
        file << "            document.getElementById('buyerId').value = buyerId;\n";
        file << "            document.getElementById('energy').value = energy;\n";
        file << "            document.getElementById('price').value = price;\n";
        file << "            \n";
        file << "            updateSellerInfo();\n";
        file << "            updateBuyerInfo();\n";
        file << "            calculateTotal();\n";
        file << "            validateTrade();\n";
        file << "            \n";
        file << "            showTab('trade');\n";
        file << "            \n";
        file << "            showNotification(`Suggestion applied! Ready to execute ${energy}kWh trade.`, 'success');\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateUserLists() {\n";
        file << "            updateProducersList();\n";
        file << "            updateConsumersList();\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateProducersList() {\n";
        file << "            const container = document.getElementById('producersList');\n";
        file << "            const producers = networkData.nodes.filter(n => n.surplus > 0);\n";
        file << "            \n";
        file << "            let html = '';\n";
        file << "            producers.forEach(producer => {\n";
        file << "                html += `\n";
        file << "                    <div class='user-card'>\n";
        file << "                        <h3>${producer.name}</h3>\n";
        file << "                        <p>Node ID: ${producer.id}</p>\n";
        file << "                        <div class='user-info'>\n";
        file << "                            <span>Energy Surplus: <strong>${producer.surplus.toFixed(1)} kWh</strong></span>\n";
        file << "                            <span>Balance: <strong>₹${producer.balance.toFixed(2)}</strong></span>\n";
        file << "                        </div>\n";
        file << "                    </div>\n";
        file << "                `;\n";
        file << "            });\n";
        file << "            \n";
        file << "            container.innerHTML = html;\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateConsumersList() {\n";
        file << "            const container = document.getElementById('consumersList');\n";
        file << "            const consumers = networkData.nodes.filter(n => n.demand > 0);\n";
        file << "            \n";
        file << "            let html = '';\n";
        file << "            consumers.forEach(consumer => {\n";
        file << "                html += `\n";
        file << "                    <div class='user-card'>\n";
        file << "                        <h3>${consumer.name}</h3>\n";
        file << "                        <p>Node ID: ${consumer.id}</p>\n";
        file << "                        <div class='user-info'>\n";
        file << "                            <span>Energy Demand: <strong>${consumer.demand.toFixed(1)} kWh</strong></span>\n";
        file << "                            <span>Balance: <strong>₹${consumer.balance.toFixed(2)}</strong></span>\n";
        file << "                        </div>\n";
        file << "                    </div>\n";
        file << "                `;\n";
        file << "            });\n";
        file << "            \n";
        file << "            container.innerHTML = html;\n";
        file << "        }\n";
        file << "        \n";
        file << "        function updateTransactionTable() {\n";
        file << "            const container = document.getElementById('transactionTable');\n";
        file << "            const transactions = marketData.transactions;\n";
        file << "            \n";
        file << "            let html = `\n";
        file << "                <table>\n";
        file << "                    <thead>\n";
        file << "                        <tr>\n";
        file << "                            <th>Transaction ID</th>\n";
        file << "                            <th>Seller</th>\n";
        file << "                            <th>Buyer</th>\n";
        file << "                            <th>Energy (kWh)</th>\n";
        file << "                            <th>Price (₹)</th>\n";
        file << "                            <th>Total (₹)</th>\n";
        file << "                            <th>Timestamp</th>\n";
        file << "                        </tr>\n";
        file << "                    </thead>\n";
        file << "                    <tbody>\n";
        file << "            `;\n";
        file << "            \n";
        file << "            if (transactions.length === 0) {\n";
        file << "                html += `\n";
        file << "                        <tr>\n";
        file << "                            <td colspan='7' style='text-align: center; padding: 40px;'>No transactions recorded yet</td>\n";
        file << "                        </tr>\n";
        file << "                `;\n";
        file << "            } else {\n";
        file << "                transactions.forEach(txn => {\n";
        file << "                    html += `\n";
        file << "                        <tr>\n";
        file << "                            <td><code>${txn.id}</code></td>\n";
        file << "                            <td>${txn.sellerId}</td>\n";
        file << "                            <td>${txn.buyerId}</td>\n";
        file << "                            <td>${txn.energyAmount.toFixed(2)}</td>\n";
        file << "                            <td>₹${txn.pricePerUnit.toFixed(3)}</td>\n";
        file << "                            <td>₹${txn.totalPrice.toFixed(2)}</td>\n";
        file << "                            <td>${new Date(txn.timestamp * 1000).toLocaleString()}</td>\n";
        file << "                        </tr>\n";
        file << "                    `;\n";
        file << "                });\n";
        file << "            }\n";
        file << "            \n";
        file << "            html += `\n";
        file << "                    </tbody>\n";
        file << "                </table>\n";
        file << "            `;\n";
        file << "            \n";
        file << "            container.innerHTML = html;\n";
        file << "        }\n";
        file << "        \n";
        file << "        function refreshNetwork() {\n";
        file << "            networkData.nodes.forEach(node => {\n";
        file << "                node.x = 100 + Math.random() * 600;\n";
        file << "                node.y = 100 + Math.random() * 400;\n";
        file << "            });\n";
        file << "            drawNetwork();\n";
        file << "            showNotification('Network topology refreshed!', 'success');\n";
        file << "        }\n";
        file << "        \n";
        file << "        function simulateNetworkGrowth() {\n";
        file << "            const newNodeId = 'NODE_' + Date.now();\n";
        file << "            const nodeTypes = ['producer', 'consumer', 'storage'];\n";
        file << "            const type = nodeTypes[Math.floor(Math.random() * nodeTypes.length)];\n";
        file << "            \n";
        file << "            const newNode = {\n";
        file << "                id: newNodeId,\n";
        file << "                name: 'Auto Node ' + newNodeId,\n";
        file << "                type: type,\n";
        file << "                surplus: type === 'producer' ? Math.random() * 100 + 50 : 0,\n";
        file << "                demand: type === 'consumer' ? Math.random() * 80 + 20 : 0,\n";
        file << "                balance: Math.random() * 10000 + 5000,\n";
        file << "                x: 100 + Math.random() * 600,\n";
        file << "                y: 100 + Math.random() * 400\n";
        file << "            };\n";
        file << "            \n";
        file << "            networkData.nodes.push(newNode);\n";
        file << "            \n";
        file << "            const randomExistingNode = networkData.nodes[Math.floor(Math.random() * (networkData.nodes.length - 1))];\n";
        file << "            networkData.connections.push({\n";
        file << "                from: newNodeId,\n";
        file << "                to: randomExistingNode.id\n";
        file << "            });\n";
        file << "            \n";
        file << "            drawNetwork();\n";
        file << "            updateUserLists();\n";
        file << "            showNotification(`New ${type} node ${newNodeId} added to network!`, 'success');\n";
        file << "        }\n";
        file << "        \n";
        file << "        function showTab(tabName) {\n";
        file << "            document.querySelectorAll('.tab-content').forEach(tab => {\n";
        file << "                tab.classList.remove('active');\n";
        file << "            });\n";
        file << "            \n";
        file << "            document.querySelectorAll('.tab').forEach(tab => {\n";
        file << "                tab.classList.remove('active');\n";
        file << "            });\n";
        file << "            \n";
        file << "            document.getElementById(tabName).classList.add('active');\n";
        file << "            event.target.classList.add('active');\n";
        file << "            \n";
        file << "            if (tabName === 'network') {\n";
        file << "                setTimeout(drawNetwork, 100);\n";
        file << "            } else if (tabName === 'transactions') {\n";
        file << "                updateTransactionTable();\n";
        file << "            } else if (tabName === 'users') {\n";
        file << "                updateUserLists();\n";
        file << "            } else if (tabName === 'suggestions') {\n";
        file << "                refreshSuggestions();\n";
        file << "            }\n";
        file << "        }\n";
        file << "        \n";
        file << "        function showNotification(message, type) {\n";
        file << "            const notification = document.getElementById('notification');\n";
        file << "            notification.textContent = message;\n";
        file << "            notification.className = `notification ${type}`;\n";
        file << "            notification.style.display = 'block';\n";
        file << "            \n";
        file << "            setTimeout(() => {\n";
        file << "                notification.style.display = 'none';\n";
        file << "            }, 5000);\n";
        file << "        }\n";
        file << "    </script>\n";
        file << "</body>\n";
        file << "</html>\n";

        file.close();
        cout << "✅ FULLY DYNAMIC ENERGY TRADING PLATFORM GENERATED!" << endl;
    }

private:
    string generateRecentTransactionsHTML() {
        auto transactions = platform.getTransactionHistory();
        if (transactions.empty()) {
            return "<p style='text-align: center; padding: 40px; opacity: 0.7;'>No transactions yet. Execute your first trade!</p>";
        }

        stringstream ss;
        int count = min(5, (int)transactions.size());

        for (int i = transactions.size() - 1; i >= max(0, (int)transactions.size() - 5); i--) {
            auto txn = transactions[i];
            ss << "<div style='padding: 15px; margin: 10px 0; background: rgba(255,255,255,0.05); border-radius: 10px; border-left: 4px solid #00f2fe;'>\n";
            ss << "  <div style='display: flex; justify-content: space-between; align-items: center;'>\n";
            ss << "    <div>\n";
            ss << "      <strong>" << txn->sellerId << " → " << txn->buyerId << "</strong>\n";
            ss << "      <div style='font-size: 0.9em; opacity: 0.8;'>" << fixed << setprecision(1) << txn->energyAmount << " kWh • ₹" << fixed << setprecision(3) << txn->pricePerUnit << "/kWh</div>\n";
            ss << "    </div>\n";
            ss << "    <div style='text-align: right;'>\n";
            ss << "      <strong>₹" << fixed << setprecision(2) << txn->totalPrice << "</strong>\n";
            ss << "      <div style='font-size: 0.8em; opacity: 0.7;'>" << txn->getFormattedTime() << "</div>\n";
            ss << "    </div>\n";
            ss << "  </div>\n";
            ss << "</div>\n";
        }

        return ss.str();
    }

    string generateSuggestionsHTML() {
        auto suggestions = platform.getTradeSuggestions();
        if (suggestions.empty()) {
            return "<p style='text-align: center; padding: 40px; opacity: 0.7;'>No suggestions available. Add more users to the network.</p>";
        }

        stringstream ss;
        for (const auto& suggestion : suggestions) {
            ss << "<div class='user-card' style='margin: 15px 0;'>\n";
            ss << "  <div style='display: flex; justify-content: space-between; align-items: start;'>\n";
            ss << "    <div>\n";
            ss << "      <h4>" << suggestion.sellerId << " → " << suggestion.buyerId << "</h4>\n";
            ss << "      <p>" << suggestion.reason << "</p>\n";
            ss << "      <div style='display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 10px;'>\n";
            ss << "        <span>Energy: <strong>" << fixed << setprecision(1) << suggestion.suggestedEnergy << " kWh</strong></span>\n";
            ss << "        <span>Price: <strong>₹" << fixed << setprecision(3) << suggestion.suggestedPrice << "/kWh</strong></span>\n";
            ss << "        <span>Match Score: <strong>" << fixed << setprecision(1) << (suggestion.matchScore * 100) << "%</strong></span>\n";
            ss << "        <span>Path Length: <strong>" << (suggestion.path.empty() ? 1 : suggestion.path.size() - 1) << " hops</strong></span>\n";
            ss << "      </div>\n";
            ss << "    </div>\n";
            ss << "    <button class='btn' style='padding: 10px 20px; font-size: 14px;' onclick=\"useSuggestion('" << suggestion.sellerId << "', '" << suggestion.buyerId << "', " << suggestion.suggestedEnergy << ", " << suggestion.suggestedPrice << ")\">Use This</button>\n";
            ss << "  </div>\n";
            ss << "</div>\n";
        }

        return ss.str();
    }

    string generateProducersHTML() {
        auto producers = platform.getSellers();
        stringstream ss;

        for (const auto& producer : producers) {
            ss << "<div class='user-card'>\n";
            ss << "    <h3>" << producer->name << "</h3>\n";
            ss << "    <p>Node ID: " << producer->id << "</p>\n";
            ss << "    <div class='user-info'>\n";
            ss << "        <span>Energy Surplus: <strong>" << fixed << setprecision(1) << producer->energySurplus << " kWh</strong></span>\n";
            ss << "        <span>Balance: <strong>₹" << fixed << setprecision(2) << producer->balance << "</strong></span>\n";
            ss << "    </div>\n";
            ss << "</div>\n";
        }

        return ss.str();
    }

    string generateConsumersHTML() {
        auto consumers = platform.getBuyers();
        stringstream ss;

        for (const auto& consumer : consumers) {
            ss << "<div class='user-card'>\n";
            ss << "    <h3>" << consumer->name << "</h3>\n";
            ss << "    <p>Node ID: " << consumer->id << "</p>\n";
            ss << "    <div class='user-info'>\n";
            ss << "        <span>Energy Demand: <strong>" << fixed << setprecision(1) << consumer->energyDemand << " kWh</strong></span>\n";
            ss << "        <span>Balance: <strong>₹" << fixed << setprecision(2) << consumer->balance << "</strong></span>\n";
            ss << "    </div>\n";
            ss << "</div>\n";
        }

        return ss.str();
    }

    string generateTransactionTableHTML() {
        auto transactions = platform.getTransactionHistory();
        stringstream ss;

        ss << "<table>\n";
        ss << "    <thead>\n";
        ss << "        <tr>\n";
        ss << "            <th>Transaction ID</th>\n";
        ss << "            <th>Seller</th>\n";
        ss << "            <th>Buyer</th>\n";
        ss << "            <th>Energy (kWh)</th>\n";
        ss << "            <th>Price (₹)</th>\n";
        ss << "            <th>Total (₹)</th>\n";
        ss << "            <th>Timestamp</th>\n";
        ss << "        </tr>\n";
        ss << "    </thead>\n";
        ss << "    <tbody>\n";

        if (transactions.empty()) {
            ss << "        <tr><td colspan='7' style='text-align: center; padding: 40px;'>No transactions recorded yet</td></tr>\n";
        } else {
            for (const auto& txn : transactions) {
                ss << "        <tr>\n";
                ss << "            <td><code>" << txn->id << "</code></td>\n";
                ss << "            <td>" << txn->sellerId << "</td>\n";
                ss << "            <td>" << txn->buyerId << "</td>\n";
                ss << "            <td>" << fixed << setprecision(2) << txn->energyAmount << "</td>\n";
                ss << "            <td>₹" << fixed << setprecision(3) << txn->pricePerUnit << "</td>\n";
                ss << "            <td>₹" << fixed << setprecision(2) << txn->totalPrice << "</td>\n";
                ss << "            <td>" << txn->getFormattedTime() << "</td>\n";
                ss << "        </tr>\n";
            }
        }

        ss << "    </tbody>\n";
        ss << "</table>\n";
        return ss.str();
    }

    string generatePriceHistoryJSON() {
        auto analytics = platform.getMarketAnalytics();
        auto priceHistory = analytics.getPriceHistory(15);

        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < priceHistory.size(); i++) {
            ss << "{\"timestamp\": " << priceHistory[i].first
               << ", \"price\": " << priceHistory[i].second << "}";
            if (i < priceHistory.size() - 1) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    string generateVolumeHistoryJSON() {
        auto analytics = platform.getMarketAnalytics();
        auto volumeHistory = analytics.getVolumeHistory(15);

        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < volumeHistory.size(); i++) {
            ss << "{\"timestamp\": " << volumeHistory[i].first
               << ", \"volume\": " << volumeHistory[i].second << "}";
            if (i < volumeHistory.size() - 1) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    string generateTransactionsJSON() {
        auto transactions = platform.getTransactionHistory();

        stringstream ss;
        ss << "[";
        for (size_t i = 0; i < transactions.size(); i++) {
            auto txn = transactions[i];
            ss << "{\n";
            ss << "  \"id\": \"" << txn->id << "\",\n";
            ss << "  \"sellerId\": \"" << txn->sellerId << "\",\n";
            ss << "  \"buyerId\": \"" << txn->buyerId << "\",\n";
            ss << "  \"energyAmount\": " << txn->energyAmount << ",\n";
            ss << "  \"pricePerUnit\": " << txn->pricePerUnit << ",\n";
            ss << "  \"totalPrice\": " << txn->totalPrice << ",\n";
            ss << "  \"timestamp\": " << txn->timestamp << "\n";
            ss << "}";
            if (i < transactions.size() - 1) ss << ",";
        }
        ss << "]";
        return ss.str();
    }
};

// ==================== MAIN FUNCTION ====================

int main() {
    srand(time(0));

    cout << "=============================================================\n";
    cout << "    ⚡ NEXUS NETWORK - Fully Dynamic Trading Platform  ⚡\n";
    cout << "=============================================================\n\n";

    EnergyTradingPlatform platform;

    // Initialize comprehensive sample data
    platform.addUser(make_shared<User>("SOLAR_001", "Quantum Solar Farm", 450.0, 0, 12000.0));
    platform.addUser(make_shared<User>("HYDRO_001", "Hydro Electric Station", 680.0, 0, 18000.0));
    platform.addUser(make_shared<User>("WIND_001", "Wind Turbine Array", 320.0, 0, 9000.0));
    platform.addUser(make_shared<User>("NUCLEAR_001", "Nuclear Power Plant", 950.0, 0, 22000.0));
    platform.addUser(make_shared<User>("RES_001", "Residential Complex A", 0, 280.0, 15000.0));
    platform.addUser(make_shared<User>("TECH_001", "Tech Campus", 0, 520.0, 30000.0));
    platform.addUser(make_shared<User>("IND_001", "Industrial Park", 0, 750.0, 45000.0));
    platform.addUser(make_shared<User>("GRID_001", "Smart Grid Hub", 180.0, 80.0, 15000.0, "storage"));
    platform.addUser(make_shared<User>("BATT_001", "Battery Storage A", 120.0, 40.0, 10000.0, "storage"));
    platform.addUser(make_shared<User>("COMM_001", "Commercial District", 0, 320.0, 25000.0));

    // Execute sample trades to populate data
    platform.executeTrade("SOLAR_001", "RES_001", 45.5, 0.18);
    platform.executeTrade("HYDRO_001", "TECH_001", 78.2, 0.16);
    platform.executeTrade("WIND_001", "IND_001", 32.7, 0.22);
    platform.executeTrade("NUCLEAR_001", "GRID_001", 95.0, 0.15);
    platform.executeTrade("GRID_001", "COMM_001", 28.8, 0.20);
    platform.executeTrade("BATT_001", "RES_001", 15.3, 0.19);

    // Generate HTML GUI
    HTMLGUIGenerator guiGen(platform);
    guiGen.generateHTML();

    cout << "\n🎯 REAL-TIME FEATURES ACTIVATED:\n";
    cout << "   ✓ Live price & volume charts (auto-update every 2s)\n";
    cout << "   ✓ Animated network topology with energy flow\n";
    cout << "   ✓ Dynamic trade suggestions\n";
    cout << "   ✓ Real-time transaction updates\n";
    cout << "   ✓ Interactive network growth simulation\n";
    cout << "   ✓ Live path analysis and validation\n\n";

    cout << "💰 CURRENCY: All values displayed in Indian Rupees (₹)\n\n";

    cout << "🌐 LAUNCHING DYNAMIC INTERFACE...\n";

    // Open the HTML file in default browser
#ifdef _WIN32
    system("start energy_trading_platform.html");
#elif __APPLE__
    system("open energy_trading_platform.html");
#else
    system("xdg-open energy_trading_platform.html");
#endif

    cout << "\n✨ FULLY DYNAMIC PLATFORM READY!\n";
    cout << "📁 Interface: energy_trading_platform.html\n";
    cout << "\n🚀 Dynamic Capabilities:\n";
    cout << "   • Real-time market analytics\n";
    cout << "   • Live network visualization\n";
    cout << "   • Automated trade execution\n";
    cout << "   • Smart AI suggestions\n";
    cout << "   • Interactive network growth\n";
    cout << "   • Continuous data updates\n\n";

    cout << "Press Enter to exit...\n";
    cin.get();

    return 0;
}
