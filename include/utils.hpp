#ifndef UTILS_HPP
#define UTILS_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <iostream>

inline void fail(boost::beast::error_code ec, char const* what)
{
    if(ec == net::ssl::error::stream_truncated)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}

#endif // UTILS_HPP

