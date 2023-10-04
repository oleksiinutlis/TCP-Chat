#include <iostream>
#include <map>
#include <list>
#include <optional>z

#include "Log.h"

class Room;

struct User: public IClientSessionUserData
{
    Room&             m_room;
    IChatSession*     m_session;
    const std::string m_userLogin;
    User( Room& match, IChatSession* session, std::string login) : m_room(match), m_session(session), m_userLogin(login) {
    }
    /*User(Room& match, IChatSession* session) : m_room(match), m_session(session) {
    }*/
};

class Room
{
    io_context&             m_serverIoContext;
    
public:
    const std::string       m_roomId;

    std::shared_ptr<User>   m_player1;
    std::shared_ptr<User>   m_player2;

    
public:
    Room( io_context& serverIoContext, const std::string& matchId )
    : m_serverIoContext(serverIoContext),
      m_roomId(matchId)
    {}

};

class TcpChat: public ITcpChat
{
    io_context&             m_serverIoContext;

    std::list<Room> m_roomsList;
    
public:
    
    TcpChat( io_context& serverIoContext ) : m_serverIoContext( serverIoContext ) {}
    
    virtual void handlePlayerMessage( IChatSession& session, boost::asio::streambuf& message ) override
    {
        LOG( "SERVER: Recieved from client: " << std::string( (const char*)message.data().data(), message.size() ).c_str() <<"\n");

        std::istringstream input;
        input.str( std::string( (const char*)message.data().data(), message.size() ) );

        std::string command;
        std::getline( input, command, ';');
        
        if (command == JOIN_ROOM_CMD)
        {
            // Get 'MatchId'
            std::string roomId;
            std::getline(input, roomId, ';');
            std::string loginId;
            std::getline(input, loginId, ';');


            LOG("m_matchList.size(): " << m_roomsList.size());
            {
                auto matchIt = std::find_if(m_roomsList.begin(), m_roomsList.end(), [&roomId](const auto& match) {
                    return match.m_roomId == roomId;
                    });

                // Match is created (we have received 'StartGame' message from 2-d player)
                if (matchIt != m_roomsList.end())
                {
                    //TODO
                    return;
                }
            }


            // we have received 'StartGame' message from 1-st player
            // Add new match
            m_roomsList.emplace_front(m_serverIoContext, roomId);

            //
            // Send WAIT_2d_PLAYER_CMD command to 1-st player
            //
            auto& front = m_roomsList.front();
            front.m_player1 = std::make_shared<User>(front, &session, loginId);
            session.sendMessageToClient(JOIN_SUCCESS ";\n");

            // set userPtr
            auto base = std::dynamic_pointer_cast<IClientSessionUserData>(front.m_player1);
            session.setUserInfoPtr(std::weak_ptr<IClientSessionUserData>(base));
            LOG("m_matchList.size(): " << m_roomsList.size());
        }
        else if (command == MSG) {
            std::string userId;
            std::getline(input, userId, ';');
            std::string message;
            std::getline(input, message, ';');

        }
    }
};
