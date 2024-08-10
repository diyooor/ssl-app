#ifndef BOOST_BEAST_EXAMPLE_COMMON_SERVER_CERTIFICATE_HPP
#define BOOST_BEAST_EXAMPLE_COMMON_SERVER_CERTIFICATE_HPP

#include <boost/asio/buffer.hpp>
#include <boost/asio/ssl/context.hpp>
#include <fstream>
#include <sstream>
#include "dotenv.hpp"

std::string load_file_content(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + file_path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

inline void load_server_certificate(boost::asio::ssl::context& ctx)
{
    // Load environment variables from the .env file
    dotenv::init(".env");

    // Retrieve file paths and password from the environment
    const char* cert_path = std::getenv("CERT_PATH");
    const char* key_path = std::getenv("KEY_PATH");
    const char* dh_path = std::getenv("DH_PATH");
    const char* password_cstr = std::getenv("SSL_PASSWORD");

    // Ensure all required environment variables are set
    if (!cert_path || !key_path || !dh_path || !password_cstr) {
        throw std::runtime_error("Missing one or more required environment variables");
    }

    // Load the contents of the certificate, key, and DH parameter files
    std::string cert = load_file_content(cert_path);
    std::string key = load_file_content(key_path);
    std::string dh = load_file_content(dh_path);
    std::string password(password_cstr);

    ctx.set_password_callback(
        [password](std::size_t,
                   boost::asio::ssl::context_base::password_purpose)
        {
            return password;
        });

    ctx.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::single_dh_use);

    ctx.use_certificate_chain(
        boost::asio::buffer(cert.data(), cert.size()));

    ctx.use_private_key(
        boost::asio::buffer(key.data(), key.size()),
        boost::asio::ssl::context::file_format::pem);

    ctx.use_tmp_dh(
        boost::asio::buffer(dh.data(), dh.size()));
}


#endif
