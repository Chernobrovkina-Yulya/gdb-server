#include "server.hpp"

// utility to mapping a value to hex character
// d - hexidecimal digit. Any non-hex digit returns a NULL char
char HexToChar(uint8_t d)
{
    const char map[] = "0123456789abcdef"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                       "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    return map[d];
}

// give the value of a hex char
// return -1 if the char is invalid
uint8_t CharToHex(int ch)
{
    return ((ch >= 'a') && (ch <= 'f')) ? ch - 'a' + 10:
           ((ch >= '0') && (ch <= '9')) ? ch - '0':
           ((ch >= 'A') && (ch <= 'F')) ? ch - 'A' + 10 : -1;
}

std::string ValToHex(uint64_t val, int numbytes)
{
    std::string s(2 * numbytes, '0');
    for (size_t n = 0; n < numbytes; ++n)
    {
        unsigned char byte = val & 0xff;
        s[2 * n] = HexToChar((byte >> 4) & 0xf);
        s[2 * n + 1] = HexToChar(byte & 0xf);
        val /= 256;
    }
    return s;
}
