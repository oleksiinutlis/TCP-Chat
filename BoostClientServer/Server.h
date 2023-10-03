#include <iostream>
#include <map>
#include <boost/asio.hpp>
#include "Interfaces.h"

using namespace boost::asio;
using ip::tcp;

class ChatSession : public std::enable_shared_from_this<ChatSession>, public IChatSession
{
    io_context&             m_ioContext;
    ITcpChat&               m_chat;
    tcp::socket             m_socket;
    boost::asio::streambuf  m_streambuf;
    
    std::weak_ptr<IClientSessionUserData> m_userInfoPtr;

public:
    ChatSession( io_context& ioContext, ITcpChat& chat, tcp::socket&& socket)
      : m_ioContext(ioContext),
        m_chat(chat),
        m_socket(std::move(socket))
    {
    }

    ~ChatSession() { std::cout << "!!!! ~ClientSession()" << std::endl; }
    
    virtual void  setUserInfoPtr( std::weak_ptr<IClientSessionUserData> userInfoPtr ) override { m_userInfoPtr = userInfoPtr; }
    virtual std::weak_ptr<IClientSessionUserData> getUserInfoPtr() override { return m_userInfoPtr; }

    virtual void sendMessageToClient( std::string command ) override
    {
        std::shared_ptr<boost::asio::streambuf> wrStreambuf = std::make_shared<boost::asio::streambuf>();
        std::ostream os(&(*wrStreambuf));
        os << command;

        sendMessageToClient( wrStreambuf );
    }

    virtual void sendMessageToClient( std::shared_ptr<boost::asio::streambuf> wrStreambuf ) override
    {
        auto self(shared_from_this());

        async_write( m_socket, *wrStreambuf,
            [this,self,wrStreambuf] ( const boost::system::error_code& ec, std::size_t bytes_transferred  )
            {
                if ( ec )
                {
                    std::cout << "!!!! ClientSession::sendMessage error: " << ec.message() << std::endl;
                    exit(-1);
                }
            });
    }

    void readMessage()
    {
        auto self(shared_from_this());

        async_read_until( m_socket, m_streambuf, '\n',
            [this,self] ( const boost::system::error_code& ec, std::size_t bytes_transferred )
            {
                if ( ec )
                {
                    std::cout << "!!!! ClientSession::readMessage error: " << ec.message() << std::endl;
                    exit(-1);
                }
                else
                {
                    // handle message
                    m_chat.handlePlayerMessage( *this, m_streambuf );

                    // read next message
                    m_streambuf.consume(bytes_transferred);
                    readMessage();
                }
        });
    }
};


class TcpServer
{
    ITcpChat&          m_game;
    
    io_context&     m_ioContext;
    tcp::acceptor   m_acceptor;
    tcp::socket     m_socket;

    std::vector<std::shared_ptr<ChatSession>> m_sessions;

public:
    TcpServer( io_context& ioContext, ITcpChat& game, int port ) :
        m_game(game),
        m_ioContext(ioContext),
        m_acceptor( m_ioContext, tcp::endpoint(tcp::v4(), port) ),
        m_socket(m_ioContext)
    {
    }

    void execute()
    {
        post( m_ioContext, [this] { accept(); } );
        m_ioContext.run();
    }
    
    void accept()
    {

        m_acceptor.async_accept( [this] (boost::system::error_code ec, ip::tcp::socket socket ) {
            if (!ec)
            {
                std::make_shared<ChatSession>( m_ioContext, m_game, std::move(socket) )->readMessage();
            }

            accept();
        });
    }
};
