#ifndef REQUESTGENERATOR_H
#define REQUESTGENERATOR_H

#include "Config.h"
#include <Request.h>
#include <string>
#include <random>

class RequestGenerator {
    public:
        RequestGenerator(const Config* config_ptr);

        Request* generate_request(int const time, int& id);

        std::string generate_IP() ;

        int generate_time() ;

        char generate_job_type() ;

    private:
        const Config* config;
        std::mt19937 random_engine; // Mersenne Twister engine for randomness
        std::uniform_int_distribution<int> time_dist;
        std::uniform_int_distribution<int> job_type_dist; // 0 for P, 1 for S
        std::uniform_int_distribution<int> ip_range_choice_dist;
        std::uniform_int_distribution<int> ip_octet_dist;
};

#endif // REQUESTGENERATOR_H