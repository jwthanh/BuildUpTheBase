#pragma once
#ifndef NETWORK_H
#define NETWORK_H
#include <string>
#include "base/CCRef.h"

namespace cocos2d{
    namespace network{
        class HttpResponse;
    }
}

class Request : public cocos2d::Ref
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

    static Request* create_get(std::string url);
    static Request* create_post(std::string url, std::string data);
    static Request* create(Type request_type, std::string url, std::string data);

    Request();
    bool init(Type request_type, std::string url, std::string data);

    Type type;
    std::string _url;
    cocos2d::network::HttpResponse* _response;

    std::string get_response_str() const;
    bool is_valid_response() const;
};

#endif
