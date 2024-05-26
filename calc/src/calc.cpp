#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unistd.h>
#include <optional>

#include "calc.h"
#include "write2db.h"

using namespace std;

/****************************
 * Client 端的log要先做前處理
 * 委託異動 => OrderUpdate
 * 委託成功 => OrderSuccess
 * 刪單成功 => CancelSuccess
 * 盤中零股 => IntraDayOdd
 * [成交]   => [MatchReport]
 * 委託回報 => OrderReport
 *
 * 前處理完後再將檔案放到Linux上
 * 然後再用grep來過濾掉不要的log
 * $ grep -a -E '.*OrderUpdate.*C70|.*OrderReport.*C70|.*\[MatchReport\].*IntraDayOdd.*Buy' 20240520-19396.txt > log
 * *************************/

using ActiveOrderPair = std::pair<int, double>; // <number of active orders, total order amount>

/*
 * Given a single log entry and a key that is contained, return the next n-th entry.
 * For simplicity, user must ensure that the key provided exists in the entry
 * and that the next n-th entry is not passed the end.
 *
 * There are two types of input, one is OrderReport and the other is OrderUpdate
 * Example on OrderReport:
 * 09:13:06.012430 11 [Trace][][OrderReport]Tradetron 09:13:06.011 779c0098490 g01Ot C703002017 5299 IntraDayOdd ROD Buy 109.5 223 0000=OrderSuccess RR 
 *
 * Example on OrderUpdate:
 * 09:13:01.021246 11 [Trace][][OrderUpdate]Tradetron 09:13:01.003 779c0098490 g01Cs C703001699 2615 IntraDayOdd ROD Buy 63.8 999 0000=CancelSuccess RR 
*/

LogStats computeStats(string filename, vector<ActiveOrderPair> &data, const vector<string> &checkTimes, int count, int secondsPerInterval)
{
    size_t size = data.size();
    size_t dotPos = filename.find_last_of('.'); // we might have relative path in fron (../dir/filename) hence use last of 
    LogStats ret;
    ret.date = dotPos == string::npos ? 0 : stoi(filename.substr(dotPos - 8, 8));
    ret.tt = dotPos == string::npos ? "" : filename.substr(dotPos + 1);
    ret.numberOfLogs = count;

    // find max and its index then display max
    auto maxIter = std::max_element(data.begin(), data.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    int maxIdx = std::distance(data.begin(), maxIter);
    ret.maxActiveOrders = data[maxIdx].first;

    double mean_orders = 0.0;
    double variance_order = 0.0;
    int n = 0;
    for (const auto& pair : data) {
        ++n;
        double delta = pair.first - mean_orders;
        mean_orders += delta / n;
        variance_order += delta * (pair.first - mean_orders);
    }
    variance_order /= n;
    double stddev_orders = std::sqrt(variance_order);
    ret.meanActiveOrders = mean_orders;
    ret.stddevActiveOrders = stddev_orders;
    double median_order = 0.0;
    std::sort(data.begin(), data.end(), [](const ActiveOrderPair& a, const ActiveOrderPair& b) { return a.first < b.first; });
    if (size % 2 == 0) {
        median_order = (data[size / 2].first + data[size / 2 + 1].first) / 2.0;
    } else {
        median_order = data[size / 2 + 1].first;
    }
    ret.medianActiveOrders = median_order;

    // Calculate mean and standard deviation incrementally to avoid overflow
    double mean_amount = 0.0;
    double variance_amount = 0.0;
    n = 0;
    for (const auto& pair : data) {
        ++n;
        double delta = pair.second - mean_amount;
        mean_amount += delta / n;
        variance_amount += delta * (pair.second - mean_amount);
    }
    // finalize variance and calculate standard deviation
    variance_amount /= n;
    double stddev_amount = std::sqrt(variance_amount);
    ret.meanAmount = mean_amount;
    ret.stddevAmount = stddev_amount;

    // Now we sort the array in order to find median
    double median_amount = 0.0;
    std::sort(data.begin(), data.end(), [](const ActiveOrderPair& a, const ActiveOrderPair& b) { return a.second < b.second; });
    if (size % 2 == 0) {
        median_amount = (data[size / 2].second + data[size / 2 + 1].second) / 2.0;
    } else {
        median_amount = data[size / 2 + 1].second;
    }
    ret.maxAmount = data[size-1].second;
    ret.medianAmount = median_amount;
    ret.minAmount = data[0].second;
    return ret;
} 

void showStats(const LogStats &stats, int sampleRate) {
    cout << "date = " << stats.date << endl;
    cout << "sampling every " << sampleRate << " second(s)" << endl;
    cout << "processed " << stats.numberOfLogs << " entries" << endl;
    cout << "activeOrders: [max=" << stats.maxActiveOrders << "] ";
    cout << "[mean=" << stats.meanActiveOrders << "] ";
    cout << "[median=" << stats.medianActiveOrders << "] ";
    cout << "[stddev=" << stats.stddevActiveOrders << "]" << endl;
    cout << "orderAmount: [max=" << stats.maxAmount << "] ";
    cout << "[mean=" << stats.meanAmount << "] ";
    cout << "[median=" << stats.medianAmount << "] ";
    cout << "[min=" << stats.minAmount << "] ";
    cout << "[stddev=" << stats.stddevAmount << "]" << endl;
}

int printUsage(char *progname)
{
    cerr << "Usage: " << progname << " [-v] [-f filename] [-s intervalSeconds] [-o outputDB]" << endl;
    return 1;
}

int main(int argc, char* argv[])
{
    bool verbose = false;
    string filename;
    string outputDB;
    int opt;
    int secondsPerInterval = 0;

    while ((opt = getopt(argc, argv, "vf:s:o:h")) != -1) {
        switch(opt) {
            case 'v':
                verbose = true;
                break;
            case 'f':
                filename = optarg;
                break;
            case 'o':
                outputDB = optarg;
                break;
            case 's':
                secondsPerInterval = stoi(optarg);
                break;
            case 'h':
                return printUsage(argv[0]);
            default:
                return printUsage(argv[0]);
        }
    }

    if (filename.empty() || outputDB.empty()) {
        return printUsage(argv[0]);
    }
    if (secondsPerInterval == 0) {
        secondsPerInterval = 30; // default use 30 seconds
    }

    unordered_map<string, int> activeOrders;  // <orderId, qty>
    vector<string> checkTimes = genCheckPoints("09:10:00", "13:24:50", secondsPerInterval);
    size_t totalCheckPoints = checkTimes.size();

    int idx = 0; // the index of time points, e.g. checkPoints has time1, time2, time3, ... etc in crono order. time1 has index 0
    vector<bool> indexStatus = vector<bool>(totalCheckPoints, false);
    vector<ActiveOrderPair> activeOrdersAtCheckTime = vector<ActiveOrderPair>(checkTimes.size(), ActiveOrderPair(0, 0.0));

    ifstream inputLog(filename);
    if (!inputLog)
    {
        cerr << "Failed to open log (filename from user input = " << filename << endl;
        return 1;
    }
    string line;
    int count = 0;
    double amount = 0;
    float price = 0;
    int shares = 0;

    while (getline(inputLog, line)) {
        if (line.length() <= 80)
            continue;
        string currSec = line.substr(0, 8);
        string orderId = line.substr(76, 5);
        double prz = 0.0;
        double rounded = 0.0;
        char status;
        if (line.find("MatchReport") != string::npos) {
            status = 'M';
        } else {
            size_t statusPos = line.find('=');
            if (statusPos == string::npos)
            {
                cout << "[WARN] parsing error in file " << filename << " line: " << line << endl;
                continue;
            }
            status = line[statusPos+1];
        }
        switch (status)
        {
            case 'O': // new order
            case 'C': // cancel order
            case 'M': // match report
            {
                std::optional<string> ret_price = getNextNthEntry(line, "Buy", 1);
                std::optional<string> ret_shares = getNextNthEntry(line, "Buy", 2);
                if (ret_price.has_value()) {
                    prz = stod(ret_price.value());
                    rounded = std::round(prz * 100) / 100.0;
                    price = static_cast<float>(rounded);
                } else {
                    cout << "[WARN] unable to find matching price in line: " << line << endl;
                    break;
                }
                if (ret_shares.has_value()) {
                    shares = stoi(ret_shares.value());
                } else {
                    cout << "[WARN] unable to find matching shares in line: " << line << endl;
                    break;
                }
                if (status == 'O') {
                    amount += price * shares;
                    activeOrders.insert(std::make_pair(orderId, shares));
                } else if (status == 'C') {
                    amount -= price * shares;
                    activeOrders.erase(orderId);
                } else {
                    if (activeOrders.find(orderId) != activeOrders.end()) {
                        amount -= price * shares;
                        activeOrders[orderId] -= shares;
                        if (activeOrders[orderId] == 0) {
                            activeOrders.erase(orderId); // no qty remaining, remove it
                        }
                    }
                }
                ++count;
                break;
            }
            default:
                cout << "[WARN] unable to find matching status in line: " << line << endl;
                break;
        }
        // if this log entry is ahead of the checkTimePoint, use it until the checkTimePoint is ahead
        while (idx < checkTimes.size() && currSec.compare(checkTimes[idx]) > 0 && indexStatus[idx] == false)
        {
            if (verbose) {
                cout << "currSec=" << currSec << " checkTime=" << checkTimes[idx] <<
                " [Active orders: " << activeOrders.size() << "] [Amount: " << amount << "]" << endl;
            }
            activeOrdersAtCheckTime[idx] = make_pair(activeOrders.size(), amount);
            indexStatus[idx] = true;
            idx++;
        }
        if (indexStatus[totalCheckPoints - 1])
            break; // if done no need to keep parsing the remaining log entries 
    }
    auto ret = computeStats(filename, activeOrdersAtCheckTime, checkTimes, count, secondsPerInterval);
    showStats(ret, secondsPerInterval);
    write2db(ret, ret.tt, outputDB);
    return 0;
}
