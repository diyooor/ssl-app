#include "../include/application.hpp"
#include "../include/services.hpp"

Application::Application(boost::asio::io_context& io_context) {
    // Initialize RedisService
    std::string password = get_db_password();
    redis_service_ = std::make_shared<RedisService>("127.0.0.1", 6379, password);

    // Initialize ClientService with the shared io_context
    client_service_ = std::make_shared<ClientService>(io_context);

    // Initialize ClockService with the shared io_context and RedisService
    clock_service_ = std::make_shared<ClockService>(io_context, *redis_service_);
}

std::shared_ptr<ClockService> Application::get_clock_service() const {
    return clock_service_;
}

std::shared_ptr<RedisService> Application::get_redis_service() const { 
    return redis_service_; 
}

std::string Application::get_db_password() const { 
    dotenv::init(".env");
    const char* password_cstr = std::getenv("DATABASE_PASSWORD");
    if (password_cstr == nullptr) {
        std::cerr << "Error: DATABASE_PASSWORD environment variable is not set or is null" << std::endl;
        return "";
    }
    return std::string(password_cstr);
}

