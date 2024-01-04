#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include "server.hpp"

static char elf_image[MAX_ELF_IMAGE_SIZE];
load_elf_str ed;

void print_elf_data() 
{
	std::cout << "\tРазмер файла                : " <<	ed.size << '\n';
	std::cout << "\tEndian (0-LE, 1-BE)         : " << 	ed.endian << '\n';
	std::cout << "\tАдрес точки входа _start    : " << ed.start << '\n';
	std::cout << "\tАдрес функции main          : 0x" << ed.main << '\n';
	std::cout << "\tВызов API (0-GOT, 1-PLT)    : " <<	ed.gotplt << '\n';
	std::cout << "\tКоличество записей в dynsym : " << 	ed.dynsyms << '\n';
	std::cout << "\tКоличество записей в symtab : " << 	ed.symbols << '\n';
	std::cout << "\tАдрес данных dynsym         : " <<	ed.d_addr << '\n';
	std::cout << "\tАдрес данных symtab         : " << 	ed.s_addr << '\n';
}

void GDBServer::StartServer(int port, std::string &elf_name)
{
    // загрузка исполняемого файла
    FILE *elf;
    long elf_size;
    // открываем двоичный файл для чтения
    elf = fopen(elf_name.c_str(), "rb");
    if(elf == NULL)
    {
        perror("ошибка открытия файла");
        exit(EXIT_FAILURE);
    }
    // чтение файла
    elf_size = fread(&elf_image, 1, MAX_ELF_IMAGE_SIZE, elf);
    if(elf_size <= 0)
    {
        perror("ошибка чтения файла");
        fclose(elf);
        exit(EXIT_FAILURE);
    }
    fclose(elf);

    // инициализируем эмулятор и загружаем в него файл
    uemu_init();
    uemu_dsp(7, &elf_image, elf_size, &ed);

    std::cout << "информация об исполняемом файле\n";
    print_elf_data();

    // создание сокета сервера
    server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_addr = {0};

    struct in_addr ip_to_num;
    inet_pton(AF_INET, "127.0.0.1", &ip_to_num);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr = ip_to_num;
    server_addr.sin_port = htons((uint16_t)port);

    if (bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock_fd, 1) == -1)
    {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Listening ... \n";

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    client_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_addr, &client_addr_size);
    if (client_sock_fd == -1)
    {
        perror("accept() failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Someone connected with adress: " <<  inet_ntoa(client_addr.sin_addr) << '\n';
}

void GDBServer::StopServer()
{
    close(client_sock_fd);
    close(server_sock_fd);
}