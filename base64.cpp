#include "base64.h"
#include <string>

// bit masks
// 0xFC     0b11111100
// 0x03     0b00000011
// 0xF0     0b11110000
// 0x0F     0b00001111
// 0xC0     0b11000000
// 0x3F     0b00111111
// 0x30     0b00110000

static const char *base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "0123456789+/";


std::string base64_encode(const std::string &str)
{
    const unsigned char *bytes = reinterpret_cast<const unsigned char *>(str.data());
    std::string encoded;

    std::string::size_type length = str.length();
    // reserve estimated size
    encoded.reserve(4/3 * length);
    for (size_t pos = 0; pos < length; pos += 3) {
        encoded.push_back(base64_table[(bytes[pos] & 0xFC) >> 2]);
        if (pos+1 < length) {
            encoded.push_back(base64_table[(bytes[pos] & 0x03) << 4 | (bytes[pos+1] & 0xF0) >> 4]);
            if (pos+2 < length) {
                encoded.push_back(base64_table[(bytes[pos+1] & 0x0F) << 2 | (bytes[pos+2] & 0xC0) >> 6]);
                encoded.push_back(base64_table[bytes[pos+2] & 0x3F]); 
            } else {
                encoded.push_back(base64_table[(bytes[pos+1] & 0x0F) << 2]);
                encoded.push_back('=');
            }
        } else {
            encoded.push_back(base64_table[(bytes[pos] & 0x03) << 4]);
            encoded.push_back('=');
            encoded.push_back('=');
        }
    }
    return encoded;
}

static int base64_to_index(const char chr)
{
         if ('A' <= chr && 'Z' >= chr) return chr - 'A';
    else if ('a' <= chr && 'z' >= chr) return chr - 'a' + ('Z'-'A') + 1;
    else if ('0' <= chr && '9' >= chr) return chr - '0' + ('Z'-'A') + ('z'-'a') + 2;
    else if (chr == '+') return 62;
    else if (chr == '/') return 63;
    return -1;
}

std::string base64_decode(const std::string &str)
{
    std::string decoded;
    std::string::size_type length = str.length();
    // reserve estimated size
    decoded.reserve(3/4 * length);
    for (size_t pos = 0; pos < length; pos += 4) {
        decoded.push_back((base64_to_index(str[pos]) & 0x3F) << 2 | (base64_to_index(str[pos+1]) & 0x3F) >> 4);
        if (pos+2 < length) {
            if (str[pos+2] != '=') {
                decoded.push_back((base64_to_index(str[pos+1]) & 0x3F) << 4 | (base64_to_index(str[pos+2]) & 0x3F) >> 2);
            }
            if (pos+3 < length) {
                if (str[pos+3] != '=') {
                    decoded.push_back((base64_to_index(str[pos+2]) & 0x3F) << 6 | (base64_to_index(str[pos+3]) & 0x3F));
                }
            }
        }
    }
    return decoded;
}