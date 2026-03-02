/**
 * @file RequestGenerator.cpp
 * @brief Implements the RequestGenerator class, which creates randomized Request objects for the simulation
 *        using parameters from the Config (processing time bounds, request type distribution, and IP generation). [12]
 */

#include "RequestGenerator.h"
#include <chrono>
#include <sstream>

/**
 * @brief Constructs a RequestGenerator and initializes its random number distributions. [12]
 *
 * @details
 * - Seeds the random engine using the current system time. [12]
 * - Initializes:
 *   - ip_octet_dist to generate IPv4 octets in [0, 255]. [12]
 *   - time_dist to generate processing times in [min_process_time, max_process_time] from Config. [12]
 *   - job_type_dist to generate a job type selector (0 -> 'P', 1 -> 'S'). [12]
 *   - ip_range_choice_dist to select among 3 private IP ranges (0..2). [12]
 *
 * @param config_ptr Pointer to the shared configuration object used to parameterize distributions. [12]
 */
RequestGenerator::RequestGenerator(const Config* config_ptr) :
    config(config_ptr),
    random_engine(std::chrono::system_clock::now().time_since_epoch().count()), // Seed with current time [12]
    ip_octet_dist(0, 255), // For generating IP octets (e.g., 192.168.1.0-255) [12]
    time_dist(config_ptr->min_process_time, config_ptr->max_process_time), // Processing time range [12]
    job_type_dist(0, 1), // 0 for 'P', 1 for 'S' [12]
    ip_range_choice_dist(0, 2) // Choose among 3 IP ranges [12]
{}

/**
 * @brief Generates a new Request instance with randomized fields. [12]
 *
 * @details
 * - Uses and increments the passed-in id counter (by reference) to ensure unique request ids. [12]
 * - Randomly generates:
 *   - IP_in and IP_out
 *   - processing time
 *   - job type [12]
 *
 * Ownership of the returned Request pointer is transferred to the caller. [12]
 *
 * @param time Current simulation time (used as the Request arrival_time). [12]
 * @param id Reference to a monotonically increasing request id counter; incremented as a side effect. [12]
 * @return Pointer to a newly allocated Request. [12]
 */
Request* RequestGenerator::generate_request(int const time, int& id) {
    int req_id = id++; // Use then increment the id counter [12]
    std::string ip_in = generate_IP();
    std::string ip_out = generate_IP();
    int process_time = generate_time();
    char job_type = generate_job_type();
    return new Request(req_id, ip_in, ip_out, process_time, job_type, time);
}

/**
 * @brief Generates a randomized private IPv4 address string. [12]
 *
 * @details
 * Randomly chooses one of three private IP ranges and then fills the remaining octets randomly:
 * - choice 0: "192.168.X.Y"
 * - choice 1: "10.X.Y.Z"
 * - choice 2: "172.16.X.Y" (note: this implementation uses 172.16 specifically) [12]
 *
 * @return IPv4 address string in dotted-decimal notation. [12]
 */
std::string RequestGenerator::generate_IP() {
    std::stringstream ss;

    // Randomly choose one of 3 common private IP ranges. [12]
    int choice = ip_range_choice_dist(random_engine);
    switch (choice) {
        case 0: // 192.168.X.X range [12]
            ss << "192.168." << ip_octet_dist(random_engine) << "." << ip_octet_dist(random_engine);
            break;

        case 1: { // 10.X.X.X generation [12]
            ss << "10." << ip_octet_dist(random_engine) << "."
               << ip_octet_dist(random_engine) << "." << ip_octet_dist(random_engine);
            break;
        }

        case 2: // 172.16.X.X range (as implemented) [12]
            ss << "172.16." << ip_octet_dist(random_engine) << "." << ip_octet_dist(random_engine);
            break;
    }
    return ss.str();
}

/**
 * @brief Generates a randomized processing time (in cycles) within the configured bounds. [12]
 * @return Processing time in cycles. [12]
 */
int RequestGenerator::generate_time()  {
    return time_dist(random_engine);
}

/**
 * @brief Generates a randomized job type character. [12]
 *
 * @details
 * Uses job_type_dist where:
 * - 0 maps to 'P'
 * - 1 maps to 'S' [12]
 *
 * @return Job type character ('P' or 'S'). [12]
 */
char RequestGenerator::generate_job_type()  {
    return (job_type_dist(random_engine) == 0) ? 'P' : 'S';
}