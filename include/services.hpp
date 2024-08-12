#ifndef SERVICES_HPP
#define SERVICES_HPP

#include "beast.hpp"
#include <string>
#include <sw/redis++/redis++.h>
#include <iostream>
#include "dotenv.hpp"

class ClientService {
public:
    ClientService();
    std::string get(const std::string& host, const std::string& port, const std::string& target, int version = 11);
    std::string post(const std::string& host, const std::string& port, const std::string& target, const std::string& body, int version = 11);

private:
    net::io_context ioc_;
    tcp::resolver resolver_;
    beast::tcp_stream stream_;
};

class RedisService {
public:
    RedisService(const std::string& host, int port, const std::string& password);
    std::string get(const std::string& key, const std::string& field);
    bool validate_login(const std::string& username, const std::string& password);
    void register_user(const std::string& username, const std::string& password);
private:
    sw::redis::Redis redis_;
};

#endif // SERVICES_HPP

