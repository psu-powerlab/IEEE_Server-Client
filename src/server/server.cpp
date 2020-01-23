//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: HTTP server, synchronous
//
//------------------------------------------------------------------------------

#include "server.h"
#include <iostream>

namespace bb = boost::beast;  // alias to make reading easier

Server* Server::Instance()
{

}

Server::Server()
{
    std::cout << "[HTTP Server... constructed" << std::endl;
}

Server::~Server()
{
    // do nothing
}

// Return a reasonable mime type based on the extension of a file.
string_view Server::MimeType(bb::string_view path)
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

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
string Server::PathCat
    (
        bb::string_view base, bb::string_view path
    )
{
    if (base.empty())
    {
        return std::string(path);
    }

    std::string result(base);
    char constexpr path_separator = '/';
    if(result.back() == path_separator)
    {
        result.resize(result.size() - 1);
    }

    result.append(path.data(), path.size());
    return result;
}

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
void Server::HandleRequest
    (
        bb::string_view doc_root,
        bb::http::request<Body, bb::http::basic_fields<Allocator>>&& req,
        Send&& send
    )
{
    // Returns a bad request response
    auto const bad_request =
    [&req](bb::string_view why)
    {
        bb::http::response <bb::http::string_body> res
        {
            bb::http::status::bad_request, req.version()
        };
        res.set(bb::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(bb::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found =
    [&req](bb::string_view target)
    {
        bb::http::response <bb::http::string_body> res
        {
            bb::http::status::not_found, req.version()
        };
        res.set(bb::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(bb::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '"+std::string(target)+"' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response
    auto const server_error =
    [&req](bb::string_view what)
    {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };

    // Make sure we can handle the method
    if( req.method() != bb::http::verb::get &&
        req.method() != bb::http::verb::head)
        return send(bad_request("Unknown HTTP-method"));

    // Request path must be absolute and not contain "..".
    if( req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != bb::string_view::npos)
        return send(bad_request("Illegal request-target"));

    // Build the path to the requested file
    std::string path = PathCat(doc_root, req.target());
    if(req.target().back() == '/')
        path.append("index.html");

    // Attempt to open the file
    bb::error_code ec;
    bb::http::file_body::value_type body;
    body.open(path.c_str(), bb::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if(ec == bb::errc::no_such_file_or_directory)
        return send(not_found(req.target()));

    // Handle an unknown error
    if(ec)
        return send(server_error(ec.message()));

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if(req.method() == bb::http::verb::head)
    {
        bb::http::response<bb::http::empty_body> res
        {
            bb::http::status::ok, req.version()
        ;
        res.set(bb::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(bb::http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }

    // Respond to GET request
    bb::http::response<bb::http::file_body> res{
        std::piecewise_construct,
        std::make_tuple(std::move(body)),
        std::make_tuple(bb::http::status::ok, req.version())};
    res.set(bb::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(bb::http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
}
