#include <iostream>
#include <string.h>

#include "server.hpp"

// глобальная переменная, хранящая регистры
reg_str r;
// глобальная переменная, хранящая область памяти
data_block d;

void GDBServer::ProcessRequests()
{
    while(true)
    {
        HandleRequest();
    }
}

void GDBServer::HandleRequest()
{
    if(GetPkt())
    { 
        perror("Communication failure");
        exit(EXIT_FAILURE);
    }

    std::cout << "packet from GDB: "<< pack.data << '\n';

    switch (pack.data[0])
    {
    case 'H':
        // request for setting thread for opertaions ('m', 'g', etc)
        //  на дпнный момент говорим GDB выбрать любой поток
        PackStr("");
        PutPkt();
        std::cout << "'H' command handle\n";
        break;

    case 'v':
        // v packet recieved
        vPacket();
        break;
         
    case 'q':
        // general query packet recieved
        QueryPacket();
        break;

    case '!':
        // request for extended remote mode
        PackStr("OK");
        std::cout << "request '!' handle\n";
        PutPkt();
        break;

    case '?':
        //return last signal id
        ReportException();
        break;
    case 'k':
        // kill request
        KillProcess();
        break;
    case 'c':
        // continue
        Continue();
        break;
    case 'g':
        // read the registers
        ReadReg();
        break;
    case 'm':
        // read the memory
        ReadMem();
        break;
    default:
        // unknown requests are ignored
        std::cout << "Warning: unknown RSP request: " << pack.data << '\n';
        break;
    }
}

void GDBServer::KillProcess()
{
    std::cout << "kill request recieved: " << pack.data << '\n';
}

void GDBServer::Continue()
{
    std::cout << "'c' request recieved: " << pack.data << '\n';
}

void GDBServer::ReportException()
{
    PackStr("S05");
    PutPkt();
    std::cout << "? command handle\n";
}

// handle query packets, recieved from GDB

void GDBServer::QueryPacket()
{
    if(strstr(pack.data, "qSupported"))
        qSupported();
    else if(strstr(pack.data, "qAttached"))
        qAttached();
    else if(strstr(pack.data,"qOffsets"))
        qOffsets();
    else
    {
        std::cout << "unknown query packet recieved: " << pack.data << '\n';
        EmptyResp();
    }
}

//handle v packets, recieved from GDB

void GDBServer::vPacket()
{
    if(strstr(pack.data, "vMustReplyEmpty"))
        vMustReplyEmpty();
    else
    {
        std::cout << "unknown 'v' packet recieved:" << pack.data << '\n';
        EmptyResp();
    }
}

// recieve a set of supported features from GDB and send server's set in response

void GDBServer::qSupported()
{
    if (strstr(pack.data, "hwbreak+;"))
    {
        PackStr("hwbreak+;");
        PutPkt();
        std::cout << "qSupported command handle\n";
    }
}

// tells GDB that the response to an unknown 'v' packet is empty string

void GDBServer::vMustReplyEmpty()
{
    EmptyResp();
    std::cout << "vMustReplyEmpty command handle\n";
}

void GDBServer::EmptyResp()
{
    PackStr("");
    PutPkt();
}

void GDBServer::qAttached()
{
    PackStr("0");
    PutPkt();
    std::cout << "qAttached command handle\n";
}

void GDBServer::qOffsets()
{
    PackStr("Text=0;Data=0;Bss=0");
    PutPkt();
    std::cout << "qOffsets command handle\n";
}

void GDBServer::ReadReg()
{
    uemu_dsp(8, &r);
    std::string s;
    for (int i = 0; i < 32; ++i)
    {   
        s += ValToHex(r.cpu[i].v, 8);
        //std::cout << "reg №" << i << ": val - " << r.cpu[i].v << "\tHex rep - " 
        //<<  ValToHex(r.cpu[i].v, 8) << '\n';
    }
    PackStr(s);
    PutPkt();
    std::cout << "g command handle\n";
}

void GDBServer::ReadMem()
{
    uint64_t addr;  // откуда читать информацию
    int len;        // сколько байтов прочесть
    sscanf(pack.data, "m%x,%x:", &addr, &len);

    uemu_dsp(11, &d, addr, static_cast<uint64_t>(addr + len - 1));

    std::string s;
    for (size_t i = 0; i < len; ++i)
    {
        s += ValToHex(d.d[i], 1);
    }
    PackStr(s);
    PutPkt();
    std::cout << "m command handle\n";
}
