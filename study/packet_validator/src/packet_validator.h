#pragma once

/*
Consider an abstract peer-to-peer network. Peers are identified by IP addresses (IPv4).
There is a protocol allowing peers to send data chunks. In order to send data, a sender must
open a connection and a receiver must acknowledge the connection. Network traffic is
represented as a set of packets in the following format:

<src_ip>:<dst_ip>:<message_type>[:<payload>]

E.g.:
127.0.0.1:127.0.0.2:O
127.0.0.2:127.0.0.1:A
127.0.0.1:127.0.0.2:D:abcde
127.0.0.1:127.0.0.2:D:abababababa
127.0.0.1:127.0.0.2:C

The message types are:
O - open connection
A - acknowledge connection
D - data chunk
C - close connection

The rules for the connections are as follows:
Connections are unidirectional, i.e. a connection opened by peer A to peer B is unrelated
to a connection opened by B to A. A sender must not open a second connection to the same
destination until the first one is closed. A sender may open concurrent connections to
different destinations, but not more than a `connections_per_ip` limit. Only the sender
can close the connection. The connection is considered implicitly closed if there is no
packet from the sender within a `timeout_sec` period. No more than `bytes_per_connection`
amount of data are allowed within a connection.
*/

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <mutex>
#include <string_view>
#include <optional>

class packet_validator {
public:
    using time_point = uint64_t; // time point of a monotonic clock in nanoseconds
    struct config {
        int timeout_sec; // timeout in seconds between packets when connection considered closed
        size_t connections_per_ip; // opening extra connection is invalid
        size_t bytes_per_connection; // data packet exceeding the limit is invalid
    };

    packet_validator(const config& cfg);

    bool handle_packet(time_point now, std::string_view pkt);
    int handle_timeouts(time_point now);

private:
    using connection_id = std::string; // connection_id is of format "<src_ip>:<dst_ip>"
    using sender_ip = std::string;     // cannot open connections more than `connections_per_ip`
    struct Connection {
        time_point last_active;        // time point from last packet
        size_t bytes_sent;             // number of data bytes already sent by this connection
    };
    struct ParsedData {                // parsed data in the given packet format
        std::string_view src_ip;
        std::string_view dst_ip;
        char message_type;
        std::optional<std::string_view> payload;
    };

    friend std::ostream& operator<<(std::ostream& os, const ParsedData& data);

    config cfg_;
    std::mutex mtx_;
    std::unordered_map<connection_id, Connection> connections_;
    std::unordered_map<sender_ip, std::unordered_set<std::string>> open_connections_per_ip_;

    void cleanup_connections(time_point now);
    ParsedData parse_packet(std::string_view pkt, char delimiter) const;
    bool handle_open(time_point now, const ParsedData& parsed);
    bool handle_ack(time_point now, const ParsedData& parsed);
    bool handle_data(time_point now, const ParsedData& parsed);
    bool handle_close(const ParsedData& parsed);
};
