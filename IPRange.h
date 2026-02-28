#ifndef IPRANGE_H
#define IPRANGE_H

#include <string>

class IPRange {
    public:
        std::string start_ip;
        std::string end_ip;

        // Constructor
        // start IP address -> end ip address
        IPRange(std::string start, std::string end);

        // contains
        /**
         * check if IP address within IP range
         * ip: ip address to check
         * return true if IP within range
         */
        bool contains(const std::string& ip) const;
    private:
        long ip_to_long_format(const std::string &ip_string) const;
};

#endif // IPRANGE_H