#include "client.h"

namespace bb = boost::beast;  // alias to make things easier to read

Client::Client() : host_("0.0.0.0"), port_("80"), resolver_(ioc_), stream_(ioc_)
{
    // do nothing
}

Client* Client::Instance()
{
    // c++11 will only run this once
    static Client* instance = new Client ();
    return instance;
}

void Client::SetHost(std::string& host)
{
    host_ = host;
}

void Client::SetPort(std::string& port)
{
    port_ = port;
}

bb::http::response <bb::http::string_body>
Client::Get(const std::string& target, const std::string& query)
{
    Client::Initialize();

    std::string href = target + query;
    bb::http::request <bb::http::string_body> req
    {
        bb::http::verb::get, href, 11
    };
    req.set(bb::http::field::host, host_);

    req.prepare_payload();
    std::cout << req << std::endl;
    return Client::Send (req);
}

bb::http::response <bb::http::string_body>
Client::Post(const std::string& target, const std::string& resource)
{
    Client::Initialize();

    bb::http::request <bb::http::string_body> req
    {
        bb::http::verb::post, target, 11
    };
    req.set(bb::http::field::host, host_);

    req.body() = resource;
    req.prepare_payload();

    return Client::Send (req);
}

bb::http::response <bb::http::string_body>
Client::Put(const std::string& target, const std::string& resource)
{
    Client::Initialize();

    bb::http::request <bb::http::string_body> req
    {
        bb::http::verb::put, target, 11
    };
    req.set(bb::http::field::host, host_);

    req.body() = resource;
    req.prepare_payload();

    return Client::Send (req);
}

bb::http::response <bb::http::string_body>
Client::Delete(const std::string& target)
{
    Client::Initialize();

    bb::http::request <bb::http::string_body> req
    {
        bb::http::verb::delete_, target, 11
    };
    req.set(bb::http::field::host, host_);

    req.prepare_payload();

    return Client::Send (req);
}

void Client::Initialize()
{
	// Look up the domain name
	auto const results = resolver_.resolve(host_, port_);

	// Make the connection on the IP address we get from a lookup
	stream_.connect(results);
}


bb::http::response <bb::http::string_body>
Client::Send(bb::http::request<bb::http::string_body>& req)
{
	// Send the HTTP request to the remote host
	bb::http::write(stream_, req);

	// This buffer is used for reading and must be persisted
	bb::flat_buffer buffer;

	// Declare a container to hold the response
	bb::http::response<bb::http::string_body> res;

	// Receive the HTTP response
	bb::http::read(stream_, buffer, res);

	return res;
}
