#ifndef HTTP_TOOLS_HPP
#define HTTP_TOOLS_HPP
#include "application.hpp"
#include "http_tools.hpp"
#include <boost/config.hpp>
#include <boost/json.hpp>  // Include Boost.JSON
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

// Function declarations
beast::string_view mime_type(beast::string_view path);
std::string path_cat(beast::string_view base, beast::string_view path);

template <class Body, class Allocator>
boost::beast::http::message_generator handle_request(
    beast::string_view doc_root,
    boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& req,
    std::shared_ptr<Application> app);

#endif // HTTP_TOOLS_HPP

