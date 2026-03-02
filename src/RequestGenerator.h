/**
 * @file RequestGenerator.h
 * @brief Declares the RequestGenerator class, which creates randomized Request objects for the simulation. [13]
 */

#ifndef REQUESTGENERATOR_H
#define REQUESTGENERATOR_H

#include "Config.h"
#include "Request.h"
#include <string>
#include <random>

/**
 * @class RequestGenerator
 * @brief Generates randomized Request instances using simulation configuration parameters. [13]
 *
 * @details
 * Uses a Mersenne Twister RNG and several distributions to generate:
 * - IPv4 addresses (from a set of private ranges)
 * - Processing times within the configured min/max bounds
 * - Job types ('P' or 'S') [13]
 */
class RequestGenerator {
public:
    /**
     * @brief Constructs a RequestGenerator configured using @p config_ptr. [13]
     *
     * @param config_ptr Pointer to the shared Config instance used to parameterize generation (not owned). [13]
     */
    RequestGenerator(const Config* config_ptr);

    /**
     * @brief Generates a new heap-allocated Request with randomized fields. [13]
     *
     * @details
     * The caller assumes ownership of the returned pointer. The @p id parameter is used as a
     * monotonically increasing request id counter and is incremented as a side effect. [12]
     *
     * @param time Current simulation time (used as the request arrival_time). [12]
     * @param id Reference to the request id counter (incremented). [12]
     * @return Pointer to a newly created Request. [12]
     */
    Request* generate_request(int const time, int& id);

    /**
     * @brief Generates a randomized IPv4 address string. [12]
     * @return IPv4 address in dotted-decimal notation.
     */
    std::string generate_IP();

    /**
     * @brief Generates a randomized processing time in cycles. [12]
     * @return Processing time in cycles.
     */
    int generate_time();

    /**
     * @brief Generates a randomized job type character ('P' or 'S'). [12]
     * @return Job type character.
     */
    char generate_job_type();

private:
    /** @brief Shared configuration used to parameterize generation (not owned). [13] */
    const Config* config;

    /** @brief Mersenne Twister engine used to generate randomness. [13] */
    std::mt19937 random_engine; // Mersenne Twister engine for randomness

    /** @brief Distribution for IPv4 octets in [0, 255]. [12] */
    std::uniform_int_distribution<int> ip_octet_dist;

    /** @brief Distribution for processing time in [Config::min_process_time, Config::max_process_time]. [12] */
    std::uniform_int_distribution<int> time_dist;

    /** @brief Distribution selecting between job types (0 -> 'P', 1 -> 'S'). [12] */
    std::uniform_int_distribution<int> job_type_dist; // 0 for P, 1 for S

    /** @brief Distribution selecting which private IP range to generate from. [12] */
    std::uniform_int_distribution<int> ip_range_choice_dist;
};

#endif // REQUESTGENERATOR_H