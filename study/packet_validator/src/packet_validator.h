#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <mutex>

class packet_validator
{
public:
    using time_point = uint64_t; // time point of a monotonic clock in nanoseconds
    struct config
    {
        int timeout_sec; // timeout in seconds between packets when connection considered closed
        size_t connections_per_ip; // opening extra connection is invalid
        size_t bytes_per_connection; // data packet exceeding the limit is invalid
    };

    packet_validator(const config& cfg);

    // return: 'true' if packet is valid
    bool handle_packet(time_point now, std::string_view pkt);

    // Periodically called often enough for the validator to detect connection timeouts to properly handle further packets
    // return: number of connection timeouts detected in this call
    int handle_timeouts(time_point now);

private:
    struct Connection {
        time_point last_active;
        size_t data_bytes;
    };

    config cfg_;
    std::mutex mtx_;
    std::unordered_map<std::string, Connection> connections_;
    std::unordered_map<std::string, std::unordered_set<std::string>> open_connections_per_ip_;

    void cleanup_connections(time_point now);
};
