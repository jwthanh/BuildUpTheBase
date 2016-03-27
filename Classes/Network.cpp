#include "Network.h"

#include "network/HttpClient.h"

using namespace cocos2d;

Request::Request(std::string url)
    : type(type), _url(url)
{
    network::HttpRequest* request = new network::HttpRequest();
    request->setRequestType(network::HttpRequest::Type::GET);

    CCLOG("sending request");

    request->setUrl(url.c_str());
    request->setResponseCallback([this](network::HttpClient* sender, network::HttpResponse* response)
    {
        this->_response = response;

        if (this->is_valid_response()) {
            CCLOG("response received");
            std::string resp_str = this->get_response_str();
            CCLOG("response is %s", resp_str.c_str());
        } else {
            CCLOG("empty response or error");
        }
    });

    request->setTag("test_request");

    network::HttpClient::getInstance()->send(request);
}

bool Request::is_valid_response() const
{
    return this->_response && this->_response->getResponseCode() == 200 && this->_response->getResponseData();
};

std::string Request::get_response_str() const
{
    if (this->_response)
    {
        auto raw_resp = this->_response->getResponseData();
        std::string resp_str = std::string(raw_resp->begin(), raw_resp->end());
        return resp_str;
    }
    else {
        CCLOG("no response set");
        return "";
    };
};
