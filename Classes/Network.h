#pragma once
#ifndef NETWORK_H
#define NETWORK_H
#include <string>

namespace cocos2d{
    namespace network{
        class HttpResponse;
    }
}

class HttpResponse;

class Request 
{
public:
    enum class Type
    {
        GET,
        POST,
        PUT,
        DELETE,
        UNKNOWN,
    };

    Type type = Type::GET;
    std::string _url = "";

    cocos2d::network::HttpResponse* _response = NULL;

    Request(std::string url);

    std::string get_response_str() const;
    bool is_valid_response() const;
};

#endif
