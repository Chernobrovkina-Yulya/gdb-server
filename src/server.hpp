#include <cstdint>
#include <string>
#define MAX_BUF_SIZE 0x8000

#include "ktest.h"

// это функции, экспортируемые из библиотеки эмулятора
// они предоставляют API для работы с эмулятором
extern "C" uint32_t uemu_init(void);
extern "C" uint32_t uemu_dsp (uint32_t num, ...);

// структура, представляющая RSP-пакет
struct Packet
{
    char data[MAX_BUF_SIZE];
    size_t len;
    uint8_t CheckSum();
};

class GDBServer
{
public:
    // функции, устанавливающие и закрывающие соединение между сервером и GDB
    void StartServer(int port, std::string &elf_name);
    void StopServer();

    // функции для обработки запросов GDB
    void ProcessRequests();
    void HandleRequest();
    
    void KillProcess();
    void Continue();
    void ReportException();
    void QueryPacket();
    void vPacket();
    void qSupported();
    void qOffsets();
    void vMustReplyEmpty();
    void EmptyResp();
    void qAttached();
    void ReadReg();
    void ReadMem();

    // функции для работы с пакетами
    void PackStr(std::string str);
    int GetChar();
    void PutChar(char ch);
    void PutPkt();
    int GetPkt();

private:
    // сокеты сервера и GDB
    int server_sock_fd;
    int client_sock_fd;

    Packet pack;
};

// utility functions
char HexToChar(uint8_t d);
uint8_t CharToHex(int ch);
std::string ValToHex(uint64_t val, int numbytes);