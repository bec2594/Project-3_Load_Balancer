#include "IPRange.h"


// functions
/*
constructor
IPRange(std::string start, std::string end);

bool contains(const std::string& ip) const;

private:
long ip_to_long_format(const std::string &ip_string) const;
*/

IPRange::IPRange(std::string start, std::string end) {
    start_ip = std::move(start);
    end_ip = std::move(end);
}

bool IPRange::contains(const std::string& ip) const {
    long check_ip = ip_to_32bit_format(ip);
    long long_start_ip = ip_to_32bit_format(start_ip);
    long long_end_ip = ip_to_32bit_format(end_ip);
    return (check_ip >= long_start_ip && check_ip <= long_end_ip);
}


uint32_t IPRange::ip_to_32bit_format(const std::string& ip_string) const {
    struct in_addr addr;
    inet_pton(AF_INET, ip_string.c_str(), &addr); // IP-> binary at addr
    return ntohl(addr.s_addr); // correct byte order
}