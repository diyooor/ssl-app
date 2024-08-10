#include "../include/session.hpp"
#include "../include/http_tools.hpp"
#include "../include/utils.hpp"

session::session(
    tcp::socket&& socket,
    ssl::context& ctx,
    std::shared_ptr<std::string const> const& doc_root,
    std::shared_ptr<Application> app)

    : stream_(std::move(socket), ctx)
    , doc_root_(doc_root)
    , app_(app)
{
}

void session::run()
{
    net::dispatch(
        stream_.get_executor(),
        beast::bind_front_handler(
            &session::on_run,
            shared_from_this()));
}

void session::on_run()
{
    beast::get_lowest_layer(stream_).expires_after(
        std::chrono::seconds(30));

    stream_.async_handshake(
        ssl::stream_base::server,
        beast::bind_front_handler(
            &session::on_handshake,
            shared_from_this()));
}

void session::on_handshake(beast::error_code ec)
{
    if(ec)
        return fail(ec, "handshake");

    do_read();
}

void session::do_read()
{
    req_ = {};

    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    http::async_read(stream_, buffer_, req_,
        beast::bind_front_handler(
            &session::on_read,
            shared_from_this()));
}

void session::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if(ec == http::error::end_of_stream)
        return do_close();

    if(ec)
        return fail(ec, "read");

    send_response(
        handle_request(*doc_root_, std::move(req_), app_));
}

void session::send_response(http::message_generator&& msg)
{
    bool keep_alive = msg.keep_alive();

    beast::async_write(
        stream_,
        std::move(msg),
        beast::bind_front_handler(
            &session::on_write, this->shared_from_this(), keep_alive));
}

void session::on_write(bool keep_alive, beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if(ec)
        return fail(ec, "write");

    if(! keep_alive)
    {
        return do_close();
    }

    do_read();
}

void session::do_close()
{
    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    stream_.async_shutdown(
        beast::bind_front_handler(
            &session::on_shutdown,
            shared_from_this()));
}

void session::on_shutdown(beast::error_code ec)
{
    if(ec)
        return fail(ec, "shutdown");
}

