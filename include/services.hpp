#ifndef SERVICES_HPP
#define SERVICES_HPP

#include "beast.hpp"
#include <string>
#include <sw/redis++/redis++.h>
#include <uuid/uuid.h>
#include <iostream>
#include "dotenv.hpp"

class ClientService {
public:
    ClientService(boost::asio::io_context& io_context);
    std::string get(const std::string& host, const std::string& port, const std::string& target, int version = 11);
    std::string post(const std::string& host, const std::string& port, const std::string& target, const std::string& body, int version = 11);

private:
    tcp::resolver resolver_;
    beast::tcp_stream stream_;
};

class RedisService {
    public:
        RedisService(const std::string& host, int port, const std::string& password);
        std::string get(const std::string& key, const std::string& field);
        bool validate_login(const std::string& username, const std::string& password);
        void register_user(const std::string& username, const std::string& password);
        std::string generate_session_id();
        void create_session(const std::string& session_id, const std::string& username);
        std::optional<std::string> validate_session(const std::string& session_id);
        void destroy_session(const std::string& session_id);
    private:
        sw::redis::Redis redis_;
};

class ClockService {
    public:
        ClockService(boost::asio::io_context& io_context, RedisService& redis_service);

        void start_timer(const std::string& session_id, int duration_seconds);
        void cancel_timer(const std::string& session_id);

    private:
        void on_timer_expire(const std::string& session_id);

        boost::asio::io_context& io_context_;
        RedisService& redis_service_;
        std::unordered_map<std::string, std::shared_ptr<boost::asio::steady_timer>> timers_;
};

#endif // SERVICES_HPP

