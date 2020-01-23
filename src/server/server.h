#ifndef SERVER_H
#define SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>
#include <string>

template <class Stream>
struct

class Server
{
public:
    Server* Instance ();
    virtual ~Server();
    void Run ();

private:
    Server();
    boost::beast::string_view MimeType (boost::beast::string_view path);
    std::string PathCat
    (
        boost::beast::string_view base, boost::beast::string_view path
    );
    template <class Body, class Allocator, class Send>
    void HandleRequest
    (
        boost::beast::string_view doc_root,
        boost::beast::http::request
            <Body, boost::beast::http::basic_fields <Allocator>>&& req,
        Send&& send
    );
    void Fail (boost::beast::error_code ec, const char* what);
};

#endif // SERVER_H
