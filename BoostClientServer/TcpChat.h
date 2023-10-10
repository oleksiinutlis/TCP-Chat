#include <iostream>
#include <map>
#include <list>
#include <optional>

class Room;

struct User
{
    Room&             m_room;
    IClientSession*   m_session;
    const std::string m_userLogin;
    User( Room& match, IClientSession* session, std::string login) : m_room(match), m_session(session), m_userLogin(login) {
    }
};

class Room
{
public:
    const std::string       m_roomId;

    std::map<std::string, User> m_userMap;
    
public:
    Room() = default;
    Room( const std::string& matchId )
    : m_roomId(matchId)
    {}

};

class TcpChat: public ITcpChat
{
    io_context&             m_serverIoContext;

    std::map<std::string, Room> m_roomsList;
    
public:
    
    TcpChat( io_context& serverIoContext ) : m_serverIoContext( serverIoContext ) {}
    
    virtual void handlePlayerMessage( IClientSession& session, boost::asio::streambuf& message ) override
    {
        std::cout << "Recieved from client:" << std::string((const char*)message.data().data(), message.size()).c_str() << "\n";

        std::istringstream input;
        input.str( std::string( (const char*)message.data().data(), message.size() ) );

        std::string command;
        std::getline( input, command, ';');
        
        if (command == JOIN_ROOM_CMD)
        {
            std::string roomId;
            std::getline(input, roomId, ';');
            std::string loginId;
            std::getline(input, loginId, ';');
            {
                auto matchIt = m_roomsList.find(roomId);
                if (matchIt != m_roomsList.end())
                {
                    User newUser(m_roomsList[roomId], &session, loginId);
                    m_roomsList[roomId].m_userMap.emplace(loginId, newUser);
                    session.sendMessageToClient(JOIN_SUCCESS ";\n");
                    return;
                }
            }

            Room newRoom(roomId);
            User newUser(newRoom, &session, loginId);
            newRoom.m_userMap.emplace(loginId, newUser);

            m_roomsList.emplace(roomId, newRoom);
            session.sendMessageToClient(JOIN_SUCCESS ";\n");
        }
        else if (command == MSG) {
            std::string userId;
            std::getline(input, userId, ';');
            std::string roomId;
            std::getline(input, roomId, ';');
            std::string message;
            std::getline(input, message, ';');
            for (auto it : m_roomsList[roomId].m_userMap) {
                it.second.m_session->sendMessageToClient(MSG ";" + userId + ";" + message + "\n");
            }
        }
    }

    virtual void kickPlayer(IClientSession& client) override {

    }
};
