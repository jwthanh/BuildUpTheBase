#pragma once
#ifndef NETWORK_H
#define NETWORK_H
#include <string>
#include "base/CCRef.h"
#include <functional>

namespace cocos2d{
    namespace network{
        class HttpResponse;
        class HttpRequest;
    }
    class Node;
    class Vec2;
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
    cocos2d::network::HttpRequest* _request;

    std::string get_response_str() const;
    bool is_valid_response() const;

    //validates the response and then calls the callback
    void set_callback(std::function<void(std::string)> callback);

    void send();
};

class NetworkConsole
{
public:
    static void set_position(cocos2d::Node* node);
    static void get_string();

    static cocos2d::Vec2 parse_vec2(std::string response_body);

    static void send_helper(std::string url, std::function<void(std::string)> callback);
};

#endif
