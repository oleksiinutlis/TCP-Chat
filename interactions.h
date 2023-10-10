#include <iostream>
#pragma once

void join() {
    std::string login = "";
    std::string roomNumber = "001";

    std::cout << "Enter room number: " << std::endl;
    std::cin >> roomNumber;
    

    std::cout << "Enter login: " << std::endl;
    std::cin >> login;
    

    Client player;
    io_context  ioContext;
    TcpClient client(ioContext, player);
    client.setRoomId(roomNumber);
    player.setTcpClient(&client);
    player.setUsername(login);

    client.execute("127.0.0.1", 1234, JOIN_ROOM_CMD ";" + roomNumber + ";" + login + ";");
    ioContext.run();
}

