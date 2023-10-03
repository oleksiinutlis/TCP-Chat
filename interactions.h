#include <iostream>
#pragma once

void join() {
    std::string login = "";
    std::string roomNumber = "001";

    std::cout << "Enter room number: " << std::endl;
    std::cin >> roomNumber;
    roomNumber += ";";

    std::cout << "Enter login: " << std::endl;
    std::cin >> login;
    

    Client player1;
    io_context  ioContext1;
    TcpClient client1(ioContext1, player1);
    player1.setTcpClient(&client1);
    player1.setUsername(login);
    login += ";";
    client1.execute("127.0.0.1", 1234, JOIN_ROOM_CMD ";" + roomNumber + login);
    ioContext1.run();
}

