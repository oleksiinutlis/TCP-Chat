#pragma once

#include <iostream>
#include "BoostClientServer/Interfaces.h"


class Client : public IClient
{
    TcpClient*  m_tcpClient = nullptr;

    std::string m_username = "Client";

public:
    Client(){};

    void setTcpClient( TcpClient* tcpClient ) { m_tcpClient = tcpClient; }

    virtual void handleServerMessage(const std::string& command, boost::asio::streambuf& message) override
    {
        //std::cout << "Recieved from server: " << std::string((const char*)message.data().data(), message.size()).c_str();

        std::istringstream input;
        input.str(std::string((const char*)message.data().data(), message.size()));

        if (command == JOIN_SUCCESS)
        {
            std::cout << command << std::endl;
            startReading();
        }
        else if (command == MSG) {
            std::string userId;
            std::getline(input, userId, ';');
            std::string message;
            std::getline(input, message, ';');

            std::cout << "[" + userId + "]: " << message << std::endl;
        }
    }

    void setUsername(const std::string& username) { m_username = username; }

    void startReading()
    {
        std::thread inputThread([m_tcpClient = m_tcpClient, m_username = m_username]() {
            while (true)
            {
                std::string input;
                std::getline(std::cin, input);
                if (input != "") {
                    m_tcpClient->sendMessageToServer(MSG ";" + m_username + ";" + m_tcpClient->getRoomId() + ";" + input + ";");
                }
            }
            });
        inputThread.detach();
    }
};
