#include "../include/application.hpp"
#include "../include/services.hpp"

Application::Application() {
    // Initialize RedisService
    std::string password = get_db_password();
    redis_service_ = std::make_shared<RedisService>("127.0.0.1", 6379, password);

    // Initialize ClientService
    client_service_ = std::make_shared<ClientService>();
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


