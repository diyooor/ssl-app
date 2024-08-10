#include "../include/services.hpp"
#include "../include/http_tools.hpp"
#include "../include/application.hpp"
#include <boost/json.hpp>
ClientService::ClientService() : resolver_(ioc_), stream_(ioc_) {}

std::string ClientService::get(const std::string& host, const std::string& port, const std::string& target, int version) {
    try {
        auto const results = resolver_.resolve(host, port);
        net::connect(stream_.socket(), results.begin(), results.end());

        http::request<http::string_body> req{http::verb::get, target, version};
        req.set(http::field::host, host);

        http::write(stream_, req);

        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream_, buffer, res);

        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

        if (ec && ec != beast::errc::not_connected) {
            throw beast::system_error{ec};
        }

        return beast::buffers_to_string(res.body().data());
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}

RedisService::RedisService(const std::string& host, int port, const std::string& password)
    : redis_({.host = host, .port = port, .password = password}) {
    
    try {
        // Perform a PING command to check if the connection is successful
        auto pong = redis_.ping();

        // Check if the PING command returned a successful response
        if (pong == "PONG") {
            std::cout << "Successfully connected to Redis at " << host << ":" << port << std::endl;
        } else {
            std::cerr << "Failed to connect to Redis at " << host << ":" << port << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error connecting to Redis: " << e.what() << std::endl;
    }
}

std::string RedisService::get(const std::string& key, const std::string& field) {
    try {
        auto val = redis_.hget(key, field);
        if (val) {
            return *val;  // Convert std::optional to string
        } else {
            return "";  // Return empty string if field doesn't exist
        }
    } catch (const std::exception &e) {
        std::cerr << "Error fetching value from Redis: " << e.what() << std::endl;
        return "";
    }
}

bool RedisService::validateLogin(const std::string& username, const std::string& password) {
    std::string storedPassword = get("user:" + username, "password");
    return storedPassword == password;
}

