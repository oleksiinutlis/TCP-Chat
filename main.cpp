#include <iostream>

#include "BoostClientServer/Server.h"
#include "BoostClientServer/TcpClient.h"
#include "BoostClientServer/TcpChat.h"
#include "Client.h"
#include "interactions.h"

int main(int argc, char* argv[])
{
    SetThreadUILanguage(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
    // Server
    std::thread([]
        {
            io_context serverIoContext;
            TcpChat chat(serverIoContext);

            TcpServer server(serverIoContext, chat, 1234);
            server.execute();
        }).detach();
    // User
    join();
}
