#include "../include/http_tools.hpp"
#include "../include/application.hpp"
#include "../include/services.hpp"

beast::string_view mime_type(beast::string_view path)
{
    using beast::iequals;
    auto const ext = [&path]
    {
        auto const pos = path.rfind(".");
        if(pos == beast::string_view::npos)
            return beast::string_view{};
        return path.substr(pos);
    }();
    if(iequals(ext, ".htm"))  return "text/html";
    if(iequals(ext, ".html")) return "text/html";
    if(iequals(ext, ".php"))  return "text/html";
    if(iequals(ext, ".css"))  return "text/css";
    if(iequals(ext, ".txt"))  return "text/plain";
    if(iequals(ext, ".js"))   return "application/javascript";
    if(iequals(ext, ".json")) return "application/json";
    if(iequals(ext, ".xml"))  return "application/xml";
    if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if(iequals(ext, ".flv"))  return "video/x-flv";
    if(iequals(ext, ".png"))  return "image/png";
    if(iequals(ext, ".jpe"))  return "image/jpeg";
    if(iequals(ext, ".jpeg")) return "image/jpeg";
    if(iequals(ext, ".jpg"))  return "image/jpeg";
    if(iequals(ext, ".gif"))  return "image/gif";
    if(iequals(ext, ".bmp"))  return "image/bmp";
    if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if(iequals(ext, ".tiff")) return "image/tiff";
    if(iequals(ext, ".tif"))  return "image/tiff";
    if(iequals(ext, ".svg"))  return "image/svg+xml";
    if(iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

std::string path_cat(beast::string_view base, beast::string_view path)
{
    if(base.empty())
        return std::string(path);
    std::string result(base);
#ifdef BOOST_MSVC
    char constexpr path_separator = '\\';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for(auto& c : result)
        if(c == '/')
            c = path_separator;
#else
    char constexpr path_separator = '/';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
}

template <class Body, class Allocator>
http::message_generator handle_request(
    beast::string_view doc_root,
    http::request<Body, http::basic_fields<Allocator>>&& req,
    std::shared_ptr<Application> app)
{
    auto const res_ = [&req](http::status status, const std::string& body, const std::string& content_type = "application/json") {
        http::response<http::string_body> res{status, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, content_type);
        res.keep_alive(req.keep_alive());
        res.body() = body;
        res.prepare_payload();
        return res;
    };

    try {
        if (req.method() == http::verb::post && req.target() == "/") {
            auto svc = app->get_redis_service();
            auto const& body = req.body();
            auto json_obj = json::parse(body);

            std::string command = json_obj.at("command").template get<std::string>();

            if (command == "login") {
                std::string username = json_obj.at("username").template get<std::string>();
                std::string password = json_obj.at("password").template get<std::string>();

                bool valid = svc->validate_login(username, password);

                if (valid) {
                    std::string session_id = svc->generate_session_id();
                    svc->create_session(session_id, username);

                    // Start the session expiration timer (e.g., 30 minutes)
                    app->get_clock_service()->start_timer(session_id, 5);

                    // Set session ID as a secure HTTP-only cookie
                    http::response<http::string_body> res{http::status::ok, req.version()};
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::set_cookie, "session_id=" + session_id + "; HttpOnly; Secure");
                    res.set(http::field::content_type, "application/json");
                    res.body() = R"({"message": "Login successful"})";
                    res.prepare_payload();
                    return res;
                } else {
                    return res_(http::status::unauthorized, R"({"message": "Invalid username or password"})");
                }
            } else if (command == "logout") {
                std::string session_id = req.base()["Cookie"];
                
                // Destroy the session
                svc->destroy_session(session_id);
                
                // Cancel the timer associated with the session
                app->get_clock_service()->cancel_timer(session_id);
                
                return res_(http::status::ok, R"({"message": "Logged out successfully"})");
            } else if (command == "validate_session") {
                std::string session_id = req.base()["Cookie"];
                std::optional<std::string> username = svc->validate_session(session_id);

                if (username) {
                    return res_(http::status::ok, R"({"message": "Session is valid", "username": ")" + *username + "\"}");
                } else {
                    return res_(http::status::unauthorized, R"({"message": "Invalid session"})");
                }
            } else {
                return res_(http::status::bad_request, R"({"message": "Unknown command"})");
            }
        } else if (req.method() != http::verb::get && req.method() != http::verb::head) {
            return res_(http::status::bad_request, "Unknown HTTP-method");
        } else if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != beast::string_view::npos) {
            return res_(http::status::bad_request, "Illegal request-target");
        } else {
            std::string path = path_cat(doc_root, req.target());
            if (req.target().back() == '/') {
                path.append("index.html");
            }

            beast::error_code ec;
            http::file_body::value_type body;
            body.open(path.c_str(), beast::file_mode::scan, ec);

            if (ec == beast::errc::no_such_file_or_directory) {
                return res_(http::status::not_found, "The resource '" + std::string(req.target()) + "' was not found.");
            }

            if (ec) {
                return res_(http::status::internal_server_error, "An error occurred: '" + ec.message() + "'");
            }

            auto const size = body.size();

            if (req.method() == http::verb::head) {
                http::response<http::empty_body> res{http::status::ok, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, mime_type(path));
                res.content_length(size);
                res.keep_alive(req.keep_alive());
                return res;
            }

            http::response<http::file_body> res{
                std::piecewise_construct,
                    std::make_tuple(std::move(body)),
                    std::make_tuple(http::status::ok, req.version())};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, mime_type(path));
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return res;
        }
    } catch (const nlohmann::detail::parse_error& e) {
        return res_(http::status::bad_request, R"({"error": "Invalid JSON format"})");
    } catch (const std::exception& e) {
        return res_(http::status::internal_server_error, R"({"error": ")" + std::string(e.what()) + "\"}");
    }
}

template http::message_generator handle_request<http::string_body, std::allocator<char>>(
    beast::string_view doc_root,
    http::request<http::string_body, http::basic_fields<std::allocator<char>>>&& req,
    std::shared_ptr<Application> app);

