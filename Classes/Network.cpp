#include "Network.h"

#include "network/HttpClient.h"

using namespace cocos2d;

Request::Request() 
    : type(Type::GET), _url(""), _response(NULL)
{
    
};

bool Request::init(Type request_type, std::string url, std::string data)
{
    this->type = request_type;
    this->_url = url;

    network::HttpRequest::Type rtype = network::HttpRequest::Type::GET;
    if (request_type == Type::GET) { rtype = network::HttpRequest::Type::GET; }
    else if (request_type == Type::POST) { rtype = network::HttpRequest::Type::POST; }
    else { throw std::exception("need to implement this Request::Type"); }

    network::HttpRequest* request = new network::HttpRequest();
    request->setRequestType(rtype);

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
        this->release();
    });

    network::HttpClient::getInstance()->send(request);
    request->release();

    return true;
};

Request* Request::create(Type request_type, std::string url, std::string data) 
{ 
    Request *pRet = new(std::nothrow) Request(); 
    if (pRet && pRet->init(request_type, url, data)) 
    { 
        pRet->autorelease(); 
        return pRet; 
    } 
    else 
    { 
        delete pRet; 
        pRet = nullptr; 
        return nullptr; 
    } 
};

Request* Request::create_get(std::string url)
{
    return Request::create(Type::GET, url, "");
}

Request* Request::create_post(std::string url, std::string data)
{
    return Request::create(Type::GET, url, data);
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
