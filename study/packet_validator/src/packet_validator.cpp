#include <iostream>

#include "packet_validator.h"

// Define a logging macro or function
#define LOG_ERROR(message) std::cerr << message << std::endl;

packet_validator::packet_validator(const config& cfg) : cfg_(cfg) {}

bool packet_validator::handle_packet(time_point now, std::string_view pkt) {
    ParsedData parsed = parse_packet(pkt, ':');
    std::cout << parsed << std::endl;

    std::unique_lock<std::mutex> lock(mtx_);

    switch (parsed.message_type) {
        case 'O':
            return handle_open(now, parsed);
        case 'A':
            return handle_ack(now, parsed);
        case 'D':
            return handle_data(now, parsed);
        case 'C':
            return handle_close(parsed);
        default:
            LOG_ERROR("Error: Unrecognized message type '" << parsed.message_type << "'");
            return false;
    }
}

bool packet_validator::handle_open(time_point now, const ParsedData& parsed) {
    std::string src_ip {parsed.src_ip};
    std::string connection_id = std::string{parsed.src_ip} + ":" + std::string{parsed.dst_ip};

    if (open_connections_per_ip_[src_ip].size() >= cfg_.connections_per_ip) {
        LOG_ERROR("Error: Exceeds connections per IP");
        return false;
    }
    if (connections_.find(connection_id) != connections_.end()) {
        LOG_ERROR("Error: Connection already exists");
        return false;
    }
    connections_[connection_id] = {now, 0};
    open_connections_per_ip_[src_ip].insert(connection_id);
    return true;
}

bool packet_validator::handle_ack(time_point now, const ParsedData& parsed) {
    std::string dst_ip {parsed.dst_ip};
    std::string src_ip {parsed.src_ip};
    std::string connection_id = dst_ip + ":" + src_ip;

    if (connections_.find(connection_id) == connections_.end()) {
        LOG_ERROR("Error: Connection not found");
        return false;
    }
    connections_[connection_id].last_active = now;
    return true;
}

bool packet_validator::handle_data(time_point now, const ParsedData& parsed) {
    std::string src_ip {parsed.src_ip};
    std::string dst_ip {parsed.dst_ip};
    std::string connection_id = src_ip + ":" + dst_ip;

    if (connections_.find(connection_id) == connections_.end()) {
        LOG_ERROR("Error: Connection not found");
        return false;
    }
    if (parsed.payload) {
        if (parsed.payload->size() + connections_[connection_id].bytes_sent > cfg_.bytes_per_connection) {
            LOG_ERROR("Error: Exceeds bytes per connection");
            return false;
        } else {
            connections_[connection_id].bytes_sent += parsed.payload->size();
            connections_[connection_id].last_active = now;
        }
    }
    return true;
}

bool packet_validator::handle_close(const ParsedData& parsed) {
    std::string src_ip {parsed.src_ip};
    std::string dst_ip {parsed.dst_ip};
    std::string connection_id = src_ip + ":" + dst_ip;

    if (connections_.find(connection_id) == connections_.end()) {
        LOG_ERROR("Error: Connection not found");
        return false;
    }
    connections_.erase(connection_id);
    open_connections_per_ip_[src_ip].erase(connection_id);
    return true;
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
