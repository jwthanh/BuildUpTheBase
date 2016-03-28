#include "Network.h"

#include "network/HttpClient.h"
#include <json/document.h>

#include "cocos2d.h"

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
    else { CCASSERT(false, "need to implement this Request::Type"); }

    this->_request = new network::HttpRequest();
    this->_request->setRequestType(rtype);

    this->_request->setUrl(url.c_str());
    this->_request->setResponseCallback([this](network::HttpClient* sender, network::HttpResponse* response)
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


    return true;
};

void Request::send()
{
    network::HttpClient::getInstance()->send(this->_request);
    this->_request->release();
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
}

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

void NetworkConsole::get_position(Node* node)
{
    auto request = Request::create_get("localhost:8080/get_vec2");
    request->_request->setResponseCallback([request, node](network::HttpClient* client, network::HttpResponse* response)
    {
        request->_response = response;

        std::string resp_str = request->get_response_str();
        CCLOG("got the following string: '%s'", resp_str.c_str());

        rapidjson::Document document;
        document.Parse(resp_str.c_str());
        Vec2 input = { (float)document["x"].GetDouble(), (float)document["y"].GetDouble()};
        CCLOG("x: %f y: %f", input.x, input.y);

        node->setPosition(input);

    });
    request->send();
    request->retain();
}

void NetworkConsole::get_string()
{
    //auto request = Request::create_get("localhost:8080/get_string");
    // request->send();
    //request->retain();

    auto request = new network::HttpRequest();
    request->setUrl("localhost:8080/get_string");
    request->setRequestType(network::HttpRequest::Type::GET);
    request->setResponseCallback([](network::HttpClient* client, network::HttpResponse* response)
    {
        CCLOG("got the following string: '%s'", std::string(response->getResponseData()->begin(), response->getResponseData()->end()).c_str());
    });

    network::HttpClient::getInstance()->send(request);
    request->release();
};
