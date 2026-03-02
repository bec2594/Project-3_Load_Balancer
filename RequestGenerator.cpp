#include "RequestGenerator.h"

#include <chrono>
#include <sstream>

// constructor
RequestGenerator::RequestGenerator(const Config* config_ptr) :
    config(config_ptr),
    random_engine(std::chrono::system_clock::now().time_since_epoch().count()), // Seed with current time
    ip_octet_dist(0, 255), // For generating IP octets (e.g., 192.168.1.0-255)
    time_dist(config_ptr->min_process_time, config_ptr->max_process_time),
    job_type_dist(0, 1), // 0 for 'P', 1 for 'S'
    ip_range_choice_dist(0, 2)
{}

Request* RequestGenerator::generate_request(int const time, int& id) {
    int req_id = id++;
    std::string ip_in = generate_IP();
    std::string ip_out = generate_IP();
    int process_time = generate_time();
    char job_type = generate_job_type();

    return new Request(req_id, ip_in, ip_out, process_time, job_type, time);
}

std::string RequestGenerator::generate_IP() {
    std::stringstream ss;

    // Randomly choose one of 3 common private IP ranges
    int choice = ip_range_choice_dist(random_engine);

    switch (choice) {
        case 0: // 192.168.X.X range
            ss << "192.168." << ip_octet_dist(random_engine) << "." << ip_octet_dist(random_engine);
            break;
        case 1: { 
            //  10.X.X.X generation:
            ss << "10." << ip_octet_dist(random_engine) << "." // X can be 0 or 1, hitting your range
               << ip_octet_dist(random_engine) << "." << ip_octet_dist(random_engine);
            break;
        }
        case 2: // 172.16.X.X to 172.31.X.X range
            ss << "172.16." << ip_octet_dist(random_engine) << "." << ip_octet_dist(random_engine);
            break;
    }
    return ss.str();
}

int RequestGenerator::generate_time()  {
    return time_dist(random_engine);
}

char RequestGenerator::generate_job_type()  {
    return (job_type_dist(random_engine) == 0) ? 'P' : 'S';
}