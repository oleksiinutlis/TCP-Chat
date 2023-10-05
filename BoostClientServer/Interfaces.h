#include <iostream>
#include <map>
#include <boost/asio.hpp>

#pragma once

// messages from server to client
#define JOIN_SUCCESS       "Successfully joined!"

// messages from client to server

#define MSG                "Message from"
#define JOIN_ROOM_CMD      "JoinRoom"

using namespace boost::asio;
using ip::tcp;


class IChatSession
{
public:
    virtual ~IChatSession() = default;

    virtual void sendMessageToClient( std::string message ) = 0;
    virtual void sendMessageToClient( std::shared_ptr<boost::asio::streambuf> wrStreambuf ) = 0;

};

class ITcpChat
{
protected:
    virtual ~ITcpChat() = default;

public:
    virtual void handlePlayerMessage( IChatSession&, boost::asio::streambuf& message ) = 0;
};

class IClient
{
protected:
    virtual ~IClient() = default;

public:
    virtual void handleServerMessage( const std::string& command, boost::asio::streambuf& message ) = 0;
};
