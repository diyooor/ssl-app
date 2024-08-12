#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>
#include <string>
#include <boost/asio.hpp>
#include "dotenv.hpp"

class ClientService;
class RedisService;
class ClockService;

class Application {
public:
    Application(boost::asio::io_context& io_context);

    std::string get_db_password() const;
    std::shared_ptr<RedisService> get_redis_service() const;
    std::shared_ptr<ClockService> get_clock_service() const;

private:
    std::shared_ptr<ClientService> client_service_;
    std::shared_ptr<RedisService> redis_service_;
    std::shared_ptr<ClockService> clock_service_;
};

#endif // APPLICATION_HPP

