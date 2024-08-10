#ifndef SESSION_HPP
#define SESSION_HPP

#include "http_tools.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <string>

class session : public std::enable_shared_from_this<session>
{
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;
    boost::beast::flat_buffer buffer_;
    std::shared_ptr<std::string const> doc_root_;
    boost::beast::http::request<boost::beast::http::string_body> req_;
    std::shared_ptr<Application> app_;
    public:
    session(
            boost::asio::ip::tcp::socket&& socket,
            boost::asio::ssl::context& ctx,
            std::shared_ptr<std::string const> const& doc_root,
            std::shared_ptr<Application> app);

    void run();

    private:
    void on_run();
    void on_handshake(boost::beast::error_code ec);
    void do_read();
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    void send_response(boost::beast::http::message_generator&& msg);
    void on_write(bool keep_alive, boost::beast::error_code ec, std::size_t bytes_transferred);
    void do_close();
    void on_shutdown(boost::beast::error_code ec);
};

#endif // SESSION_HPP

