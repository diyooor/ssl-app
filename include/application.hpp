#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <memory>
#include <string>
#include "dotenv.hpp"
class ClientService;
class RedisService;
class Application {
    public:
        Application();
        
        std::string get_db_password() const;

        std::shared_ptr<RedisService> get_redis_service() const;
    private:
        std::shared_ptr<ClientService> client_service_;
        std::shared_ptr<RedisService> redis_service_;
};
#endif
