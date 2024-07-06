#include <iostream>

#include "packet_validator.h"

packet_validator::packet_validator(const config& cfg) : cfg_(cfg) {}

bool packet_validator::handle_packet(time_point now, std::string_view pkt) {

    struct ParsedData parsed = parse_packet(pkt, ':');
    std::cout << parsed << std::endl;
    std::string src_ip {parsed.src_ip};
    std::string dst_ip {parsed.dst_ip};

    std::string connection_id = std::string{parsed.src_ip} + ":" + std::string{parsed.dst_ip};
    std::unique_lock<std::mutex> lock(mtx_);

    switch (parsed.message_type) {
        case 'O':
        {
            if (open_connections_per_ip_[src_ip].size() >= cfg_.connections_per_ip) {
                std::cout << "Error exceeds connections per ip" << std::endl;
                return false;
            }
            if (connections_.find(connection_id) != connections_.end()) {
                std::cout << "Error connection exists" << std::endl;
                return false;
            }
            connections_[connection_id] = {now, 0};
            open_connections_per_ip_[src_ip].insert(connection_id);
            return true;
        }
        case 'A':
        {
            connection_id = dst_ip + ":" + src_ip;
            if (connections_.find(connection_id) == connections_.end()) {
                std::cout << "Error connection not found" << std::endl;
                return false;
            }
            connections_[connection_id].last_active = now;
            return true;
        }
        case 'D':
        {
            if (connections_.find(connection_id) == connections_.end()) {
                std::cout << "Error connection not found" << std::endl;
                return false;
            }
            if (parsed.payload) {
                if (parsed.payload->size() + connections_[connection_id].bytes_sent > cfg_.bytes_per_connection) {
                    std::cout << "Error exceeds bytes per connection" << std::endl;
                    return false;
                } else {
                    connections_[connection_id].bytes_sent += parsed.payload->size();
                    connections_[connection_id].last_active = now;
                }
            }
            return true;
        }
        case 'C':
        {
            if (connections_.find(connection_id) == connections_.end()) {
                std::cout << "Error connection not found" << std::endl;
                return false;
            }
            connections_.erase(connection_id);
            open_connections_per_ip_[src_ip].erase(connection_id);
            return true;
        }
        default:
            std::cout << "Error unrecognized message type '" << parsed.message_type << "'" << std::endl;
            return false;
    }
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

packet_validator::ParsedData packet_validator::parse_packet(std::string_view str, char delimiter) {
    size_t start = 0;
    size_t end = str.find(delimiter);
    ParsedData result;

    result.src_ip = str.substr(start, end - start);
    start = end + 1;
    end = str.find(delimiter, start);

    result.dst_ip = str.substr(start, end - start);
    start = end + 1;
    end = str.find(delimiter, start);

    result.message_type = str[start];
    start = end + 1;
    if (start < str.size()) {
        result.payload = str.substr(start);
    } else {
        result.payload = std::nullopt;
    }
    //result.payload = start < str.size() ? str.substr(start) : std::nullopt;
    return result;
}

std::ostream& operator<<(std::ostream& os, const packet_validator::ParsedData& data) {
    os << "[src_ip=" << data.src_ip << "][dst_ip=" << data.dst_ip << "][message_type=" <<
        data.message_type << "]";
    if (data.payload) {
        os << "[payload=" << *data.payload << "]";
    } else {
        os << "[payload=null]";
    }
    return os;
}
