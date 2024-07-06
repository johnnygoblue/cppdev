#include "packet_validator.h"
#include <sstream>

packet_validator::packet_validator(const config& cfg) : cfg_(cfg) {}

bool packet_validator::handle_packet(time_point now, std::string_view pkt) {
    std::unique_lock<std::mutex> lock(mtx_);

    std::istringstream ss{std::string(pkt)};  // Use curly braces instead of parentheses to avoid most vexing parse(unified initialization)
    std::string src_ip, dst_ip, message_type, payload;
    std::getline(ss, src_ip, ':');
    std::getline(ss, dst_ip, ':');
    std::getline(ss, message_type, ':');
    std::getline(ss, payload, ':');

    std::string connection_id = src_ip + ":" + dst_ip;

    if (message_type == "O") {
        if (open_connections_per_ip_[src_ip].size() >= cfg_.connections_per_ip) {
            return false;
        }
        if (connections_.find(connection_id) != connections_.end()) {
            return false;
        }
        connections_[connection_id] = {now, 0};
        open_connections_per_ip_[src_ip].insert(connection_id);
        return true;
    } else if (message_type == "A") {
        connection_id = dst_ip + ":" + src_ip;
        if (connections_.find(connection_id) == connections_.end()) {
            return false;
        }
        connections_[connection_id].last_active = now;
        return true;
    } else if (message_type == "D") {
        if (connections_.find(connection_id) == connections_.end()) {
            return false;
        }
        if (connections_[connection_id].bytes_sent + payload.size() > cfg_.bytes_per_connection) {
            return false;
        }
        connections_[connection_id].bytes_sent += payload.size();
        connections_[connection_id].last_active = now;
        return true;
    } else if (message_type == "C") {
        if (connections_.find(connection_id) == connections_.end()) {
            return false;
        }
        connections_.erase(connection_id);
        open_connections_per_ip_[src_ip].erase(connection_id);
        return true;
    }
    return false;
}

int packet_validator::handle_timeouts(time_point now) {
    std::unique_lock<std::mutex> lock(mtx_);
    int timeout_count = 0;

    std::deque<std::string> to_remove;
    for (const auto& [connection_id, conn] : connections_) {
        if (now - conn.last_active > cfg_.timeout_sec * 1000000000LL) {
            to_remove.push_back(connection_id);
        }
    }

    for (const auto& connection_id : to_remove) {
        connections_.erase(connection_id);
        size_t pos = connection_id.find(':');
        std::string src_ip = connection_id.substr(0, pos);
        open_connections_per_ip_[src_ip].erase(connection_id);
        timeout_count++;
    }

    return timeout_count;
}
