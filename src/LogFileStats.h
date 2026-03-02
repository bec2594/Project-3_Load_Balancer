/**
 * @file LogFileStats.h
 * @brief Declares the LogFileStats class, which collects and summarizes simulation statistics
 *        (requests processed/blocked, server scaling actions, queue metrics, and timing data). [8]
 */

#ifndef LOGFILESTATS_H
#define LOGFILESTATS_H

#include <string>
#include <vector>
#include "Request.h" // Used for Request type in record_completed_request(). [8]

/**
 * @class LogFileStats
 * @brief Aggregates counters and timing metrics for the simulation and produces a summary report. [8]
 *
 * @details
 * This class tracks:
 * - Total processed and blocked requests
 * - Server scaling operations (added/removed)
 * - Queue statistics (max and average sampling)
 * - Wait time and processing time for completed requests
 * - Optional final snapshot values (queue size, active/inactive servers) [8]
 */
class LogFileStats {
public:
    /** @brief Total number of requests that completed processing. [8] */
    int total_requests_processed; // increment_processed()

    /** @brief Total number of servers added during scaling. [8] */
    int total_servers_added; // increment_servers_added()

    /** @brief Total number of servers removed during scaling. [8] */
    int total_servers_removed; // increment_servers_removed()

    /** @brief Maximum queue size observed across all queue samples. [8] */
    int max_queue_size; // update_queue_stats()

    /** @brief Sum of queue sizes across samples, used to compute average queue size. [8] */
    long long current_queue_size_sum; // update_queue_stats()

    /** @brief Number of queue samples collected. [8] */
    int number_queue_samples; // update_queue_stats()

    /** @brief Total number of requests blocked by firewall/IP filtering. [8] */
    int requests_blocked; // increment_blocked()

    /** @brief Sum of wait times for completed requests (start_time - arrival_time). [8] */
    long long total_wait_time; // record_completed_request()

    /** @brief Sum of processing times for completed requests (completion_time - start_time). [8] */
    long long total_processing_time; // record_completed_request()

    /** @brief Count of requests included in timing statistics. [8] */
    int number_requests; // record_completed_request()

    /**
     * @name Final snapshot fields
     * @brief Values intended to be set at the end of the simulation for final reporting. [8]
     * @{
     */
    /** @brief Number of requests left in the queue at simulation end; -1 can indicate "not set". [8] */
    int final_requests_in_queue;

    /** @brief Number of active (busy) servers at simulation end; -1 can indicate "not set". [8] */
    int final_active_servers;

    /** @brief Number of inactive (idle) servers at simulation end; -1 can indicate "not set". [8] */
    int final_inactive_servers;
    /** @} */

    /**
     * @brief Constructs a LogFileStats object and initializes all counters/accumulators. [8]
     */
    LogFileStats();

    /** @brief Increments the "servers added" counter. [8] */
    void increment_servers_added();

    /** @brief Increments the "servers removed" counter. [8] */
    void increment_servers_removed();

    /** @brief Increments the "requests processed" counter. [8] */
    void increment_processed();

    /** @brief Increments the "requests blocked" counter. [8] */
    void increment_blocked();

    /**
     * @brief Updates queue statistics with a new observed queue size. [8]
     *
     * @param current_size Current queue size to sample.
     */
    void update_queue_stats(int current_size);

    /**
     * @brief Records wait and processing time for a completed request. [8]
     *
     * @param request Pointer to a completed Request (not owned by LogFileStats).
     */
    void record_completed_request(const Request* request);

    /**
     * @brief Sets the final number of requests remaining in the queue. [8]
     * @param count Remaining queue size.
     */
    void set_final_queue_size(int count);

    /**
     * @brief Sets the final active/inactive server counts. [8]
     *
     * @param active Number of busy servers.
     * @param inactive Number of idle servers.
     */
    void set_final_server_counts(int active, int inactive);

    /**
     * @brief Generates a formatted summary of all collected statistics. [8]
     * @return Summary string.
     */
    std::string summary() const;
};

#endif // LOGFILESTATS_H