/**
 * @file IPRange.cpp
 * @brief Implements the IPRange class used to represent and query an IPv4 address range. [3]
 */

#include "IPRange.h"

/**
 * @brief Constructs an IPRange from a starting and ending IPv4 address (inclusive). [3]
 *
 * @param start Starting IPv4 address in dotted-decimal notation (e.g., "10.0.0.1"). [3]
 * @param end Ending IPv4 address in dotted-decimal notation (e.g., "10.1.0.1"). [3]
 */
IPRange::IPRange(std::string start, std::string end) {
    start_ip = std::move(start);
    end_ip = std::move(end);
}

/**
 * @brief Checks whether the provided IPv4 address lies within this range (inclusive). [3]
 *
 * The check converts all IP strings to a comparable 32-bit integer form and then
 * performs an inclusive range comparison. [3]
 *
 * @param ip IPv4 address to test in dotted-decimal notation. [3]
 * @return true if @p ip is between start_ip and end_ip (inclusive), otherwise false. [3]
 */
bool IPRange::contains(const std::string& ip) const {
    long check_ip = ip_to_32bit_format(ip);
    long long_start_ip = ip_to_32bit_format(start_ip);
    long long_end_ip = ip_to_32bit_format(end_ip);
    return (check_ip >= long_start_ip && check_ip <= long_end_ip);
}

/**
 * @brief Converts an IPv4 dotted-decimal string into a host-order 32-bit integer. [3]
 *
 * Uses inet_pton() to parse the IPv4 string into network byte order, then ntohl()
 * to convert into host byte order for numeric comparisons. [3]
 *
 * @param ip_string IPv4 address in dotted-decimal notation. [3]
 * @return IPv4 address as a 32-bit unsigned integer in host byte order. [3]
 */
uint32_t IPRange::ip_to_32bit_format(const std::string& ip_string) const {
    struct in_addr addr;
    inet_pton(AF_INET, ip_string.c_str(), &addr); // IP string -> binary form [3]
    return ntohl(addr.s_addr); // convert to host byte order for comparisons [3]
}