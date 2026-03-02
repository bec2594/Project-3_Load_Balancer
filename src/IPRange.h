/**
 * @file IPRange.h
 * @brief Declares the IPRange class for representing an inclusive IPv4 address range and testing membership. [4]
 */

#ifndef IPRANGE_H
#define IPRANGE_H

#include <string>
#include <arpa/inet.h>

/**
 * @class IPRange
 * @brief Represents a start and end IPv4 address range and provides a containment test. [4]
 */
class IPRange {
public:
    /** @brief Starting IPv4 address (dotted-decimal string). [4] */
    std::string start_ip;

    /** @brief Ending IPv4 address (dotted-decimal string). [4] */
    std::string end_ip;

    /**
     * @brief Constructs an inclusive IPv4 range from @p start to @p end. [4]
     * @param start Starting IPv4 address in dotted-decimal notation. [4]
     * @param end Ending IPv4 address in dotted-decimal notation. [4]
     */
    IPRange(std::string start, std::string end);

    /**
     * @brief Checks whether an IPv4 address lies within this range (inclusive). [4]
     * @param ip IPv4 address to check in dotted-decimal notation. [4]
     * @return true if @p ip is between start_ip and end_ip (inclusive), otherwise false. [4]
     */
    bool contains(const std::string& ip) const;

private:
    /**
     * @brief Converts an IPv4 dotted-decimal string into a host-order 32-bit integer for comparisons. [4]
     * @param ip_string IPv4 address in dotted-decimal notation. [4]
     * @return IPv4 address as a 32-bit unsigned integer in host byte order. [4]
     */
    uint32_t ip_to_32bit_format(const std::string &ip_string) const;
};

#endif // IPRANGE_H