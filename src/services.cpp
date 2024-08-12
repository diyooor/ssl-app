#include "../include/services.hpp"
#include "../include/http_tools.hpp"
#include "../include/application.hpp"
#include <boost/json.hpp>
ClientService::ClientService() : resolver_(ioc_), stream_(ioc_) {}

std::string ClientService::get(const std::string& host, const std::string& port, const std::string& target, int version) {
    try {
        std::cout << "[GET Request] Host: " << host << ", Port: " << port << ", Target: " << target << ", Version: " << version << std::endl;

        auto const results = resolver_.resolve(host, port);
        net::connect(stream_.socket(), results.begin(), results.end());

        http::request<http::string_body> req{http::verb::get, target, version};
        req.version(version == 11 ? 11 : 10);  // Correctly setting the HTTP version
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

        std::string response_body = beast::buffers_to_string(res.body().data());
        std::cout << "[GET Response] Status: " << res.result_int() << ", Body: " << response_body << std::endl;

        return response_body;
    } catch (std::exception const& e) {
        std::cerr << "[GET Error] " << e.what() << std::endl;
        return "";
    }
}

std::string ClientService::post(const std::string& host, const std::string& port, const std::string& target, const std::string& body, int version) {
    try {
        std::cout << "[POST Request] Host: " << host << ", Port: " << port << ", Target: " << target << ", Version: " << version << ", Body: " << body << std::endl;

        auto const results = resolver_.resolve(host, port);
        net::connect(stream_.socket(), results.begin(), results.end());

        http::request<http::string_body> req{http::verb::post, target, version};
        req.version(version == 11 ? 11 : 10);  // Correctly setting the HTTP version
        req.set(http::field::host, host);
        req.set(http::field::content_type, "application/json");
        req.set(http::field::content_length, std::to_string(body.size()));
        req.body() = body;
        req.prepare_payload();

        http::write(stream_, req);

        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream_, buffer, res);

        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

        if (ec && ec != beast::errc::not_connected) {
            throw beast::system_error{ec};
        }

        std::string response_body = beast::buffers_to_string(res.body().data());
        std::cout << "[POST Response] Status: " << res.result_int() << ", Body: " << response_body << std::endl;

        return response_body;
    } catch (std::exception const& e) {
        std::cerr << "[POST Error] " << e.what() << std::endl;
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

bool RedisService::validate_login(const std::string& username, const std::string& password) {
    std::string storedPassword = get("user:" + username, "password");
    return storedPassword == password;
}

void RedisService::register_user(const std::string& username, const std::string& password) {
    try {
        // Check if the user already exists
        auto existingUser = redis_.hget("user:" + username, "password");
        if (existingUser) {
            std::cerr << "Error: User with username '" << username << "' already exists." << std::endl;
            return;
        }

        // Register the new user by storing the username and password in Redis
        redis_.hset("user:" + username, "password", password);
        std::cout << "User '" << username << "' successfully registered." << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error registering user '" << username << "': " << e.what() << std::endl;
    }
}

