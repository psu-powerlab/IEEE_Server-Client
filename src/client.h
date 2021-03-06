#ifndef CLIENT_H
#define CLIENT_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>

// Singleton Design Pattern
class Client
{
public:
    static Client* Instance ();
    void SetHost (std::string& host);
    void SetPort (std::string& port);

    boost::beast::http::response <boost::beast::http::string_body> Get
    (
        const std::string& target, const std::string& query = ""
    );

    boost::beast::http::response <boost::beast::http::string_body> Post
    (
        const std::string& target, const std::string& resource
    );

    boost::beast::http::response <boost::beast::http::string_body> Put
    (
        const std::string& target, const std::string& resource
    );

    boost::beast::http::response <boost::beast::http::string_body> Delete
    (
        const std::string& target
    );

private:
    Client ();
    void Initialize ();

    boost::beast::http::response <boost::beast::http::string_body> Send
    (
        boost::beast::http::request <boost::beast::http::string_body>& req
    );

private:
    // destination of server
    std::string host_;
    std::string port_;

    // required for all boost beast I/O
    boost::asio::io_context ioc_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::tcp_stream stream_;
};

#endif // CLIENT_H
