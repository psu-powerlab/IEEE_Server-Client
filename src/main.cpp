#include <iostream>
#include "client.h"

using namespace std;

int main()
{
    Client* client = Client::Instance();
    std::string host = "example.com";
    client->SetHost(host);
    boost::beast::http::response <boost::beast::http::string_body> res = client->Get("/hi/there", "?hand=wave");
    std::cout << res << std::endl;
    return 0;
}
