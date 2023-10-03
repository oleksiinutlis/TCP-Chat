#include <iostream>

#include "BoostClientServer/Server.h"
#include "BoostClientServer/TcpClient.h"
#include "BoostClientServer/Game.h"
#include "QtClientPlayer.h"
#include "interactions.h"

int main(int argc, char* argv[])
{

    // server
    std::thread([]
        {
            io_context serverIoContext;
            TcpChat chat(serverIoContext);

            TcpServer server(serverIoContext, chat, 1234);
            server.execute();
        }).detach();
    
    // Real player
    join();
}
