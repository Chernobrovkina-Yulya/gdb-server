#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

#include "server.hpp"

uint8_t Packet::CheckSum()
{
    uint8_t checksum = 0;
    for (size_t i = 0; i < len; ++i)
        checksum += static_cast<uint8_t>(data[i]);
    return checksum;
}

// write string in packet
void GDBServer::PackStr(std::string str)
{
    strcpy(pack.data, str.c_str());
    pack.data[str.size()] = '\0';
    pack.len = str.size();
}

// get a single char from rcp connection
// return the character recieved and -1 on failure
int GDBServer::GetChar()
{
    char buf[1];
    int numread = read(client_sock_fd, buf, sizeof(char));
    if(numread == -1)
        return -1;
    return buf[0] & 0xff;
}

// put a single char out on the rsp connection
void GDBServer::PutChar(char ch)
{
    while(1)
    {
        char buf[1] = {ch};
        int numwritten = write(client_sock_fd, buf, sizeof(char));
        if(numwritten == 0 || numwritten == -1)
            continue;
        else
            return;
    }
}

// Get the next packet from gdb
// store result in struct Buf *buf
// returns 0 if success, 1 otherwise
int GDBServer::GetPkt()
{
    while(1)
    {
        int     count = 0;
        int     ch;

        ch = GetChar();
        while(ch != '$')        // wait for the start of packet '$'
        {
            if(ch == -1)
                return 1;       // connection failed
            else
                ch = GetChar();
        }
        while(count < MAX_BUF_SIZE - 1)
        {
            ch = GetChar();
            if(ch == -1)
                return 1;

            if(ch == '$')
            {
                count = 0;
                continue;
            }
            if(ch == '#')       // reach the end of packet
                break;

            pack.data[count] = (char)ch;
            ++count;
        }
        pack.data[count] = '\0';
        pack.len = count;

        if(ch == '#')
        {
            // validate the checksum
            uint8_t checksum = pack.CheckSum();
            uint8_t pkt_checksum;
            ch = GetChar();
            if(ch == -1)
                return 1;
            pkt_checksum = CharToHex(ch) << 4;

            ch = GetChar();
            if(ch == -1)
                return 1;
            pkt_checksum += CharToHex(ch);

            if(checksum != pkt_checksum)
            {
                perror("Bad checksum in RSP packet");
                PutChar('-');                           // put negative ack
            }
            else
            {
                PutChar('+');                           // put positive ack
                return 0;
            }
        }
        return 0;
    }
    return 0;
}

// put the packet out on the RSP connection
void GDBServer::PutPkt()
{
    uint8_t checksum = pack.CheckSum();
    int len = pack.len;
    int ch;
    do
    {
        PutChar('$');                                   // strart of the packet
        for (size_t count = 0; count < len; ++count)
        {
            int c = pack.data[count];
            PutChar(c);
        }
        PutChar('#');                                   // end of the packet
        PutChar(HexToChar(checksum >> 4));              // put the schecksum
        PutChar(HexToChar(checksum % 16));
        ch = GetChar();
    } while (ch != '+');
}
