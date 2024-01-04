#include <iostream>

#include "server.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2)
    { 
        std::cout << "no port provided\n";
        exit(EXIT_FAILURE);
    }
    if( argc < 3)
    {
        std::cout << "no file name provided\n";
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    std::string elf_name (argv[2]);
    std::cout << "TCP server is starting\n";
    GDBServer server;
    server.StartServer(port, elf_name);
    server.ProcessRequests();
    server.StopServer();
    return 0;
}