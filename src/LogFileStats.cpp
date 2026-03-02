/**
 * @file LogFileStats.cpp
 * @brief Implements the LogFileStats class, which collects and summarizes simulation statistics
 *        (requests processed/blocked, server scaling actions, queue metrics, and timing averages). [7]
 */

#include "LogFileStats.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

/**
 * @brief Constructs a LogFileStats object with all counters initialized to zero and
 *        final snapshot fields initialized to -1 (meaning "not provided"). [7]
 */
LogFileStats::LogFileStats() :
    total_requests_processed(0),   // increment_processed() [7]
    total_servers_added(0),        // increment_servers_added() [7]
    total_servers_removed(0),      // increment_servers_removed() [7]
    max_queue_size(0),             // update_queue_stats() [7]
    current_queue_size_sum(0),     // update_queue_stats() [7]
    number_queue_samples(0),       // update_queue_stats() [7]
    requests_blocked(0),           // increment_blocked() [7]
    total_wait_time(0),            // record_completed_request() [7]
    total_processing_time(0),      // record_completed_request() [7]
    number_requests(0),            // record_completed_request() [7]
    final_requests_in_queue(-1),   // set_final_queue_size() [7]
    final_active_servers(-1),      // set_final_server_counts() [7]
    final_inactive_servers(-1)     // set_final_server_counts() [7]
{}

/**
 * @brief Increments the counter tracking how many servers were added during the simulation. [7]
 */
void LogFileStats::increment_servers_added() { total_servers_added++; }

/**
 * @brief Increments the counter tracking how many servers were removed during the simulation. [7]
 */
void LogFileStats::increment_servers_removed() { total_servers_removed++; }

/**
 * @brief Increments the counter tracking how many requests were fully processed (completed). [7]
 */
void LogFileStats::increment_processed() { total_requests_processed++; }

/**
 * @brief Increments the counter tracking how many requests were blocked by firewall rules. [7]
 */
void LogFileStats::increment_blocked() { requests_blocked++; }

/**
 * @brief Updates queue-related statistics for the current sample. [7]
 *
 * @details
 * Records the maximum queue size observed so far and accumulates values for computing the
 * average queue size across all samples. [7]
 *
 * @param current_size Current number of requests waiting in the queue. [7]
 */
void LogFileStats::update_queue_stats(int current_size) {
    max_queue_size = std::max(max_queue_size, current_size);
    number_queue_samples++;
    current_queue_size_sum += current_size;
}

/**
 * @brief Records timing data for a completed request. [7]
 *
 * @details
 * Adds:
 * - wait time: start_time - arrival_time
 * - processing time: completion_time - start_time
 *
 * Only records timing if the request pointer is valid and both start_time and completion_time
 * have been set (not equal to -1). [7]
 *
 * @param request Pointer to the completed request (not owned by LogFileStats). [7]
 */
void LogFileStats::record_completed_request(const Request* request) {
    if(request != nullptr && request->start_time != -1 && request->completion_time != -1) {
        total_wait_time += (request->start_time - request->arrival_time);
        total_processing_time += (request->completion_time - request->start_time);
        number_requests++;
    }
}

/**
 * @brief Sets the number of requests remaining in the queue at the end of the simulation. [7]
 * @param count Remaining requests in queue.
 */
void LogFileStats::set_final_queue_size(int count) {
    final_requests_in_queue = count;
}

/**
 * @brief Sets the final counts of active and inactive servers at the end of the simulation. [7]
 *
 * @param active Number of servers still busy (processing a request).
 * @param inactive Number of servers idle/available.
 */
void LogFileStats::set_final_server_counts(int active, int inactive) {
    final_active_servers = active;
    final_inactive_servers = inactive;
}

/**
 * @brief Produces a formatted multi-line summary of all collected simulation statistics. [7]
 *
 * @details
 * The summary includes totals (processed, blocked, servers added/removed), queue max/average,
 * request wait and processing averages, and optional final snapshot information (queue and servers). [7]
 *
 * @return Summary string suitable for printing to console and/or logs. [7]
 */
std::string LogFileStats::summary() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);

    ss << "\n--- Simulation Summary ---\n";
    ss << "Total Requests Processed: " << total_requests_processed << "\n";
    ss << "Requests Blocked (Firewall): " << requests_blocked << "\n";
    ss << "Servers Added: " << total_servers_added << "\n";
    ss << "Servers Removed: " << total_servers_removed << "\n";
    ss << "Max Queue Size Observed: " << max_queue_size << "\n";

    if (number_queue_samples > 0) {
        ss << "Average Queue Size: "
           << (double)current_queue_size_sum / number_queue_samples << "\n";
    } else {
        ss << "Average Queue Size: N/A (No samples)\n";
    }

    if (number_requests > 0) {
        ss << "Average Request Wait Time: "
           << (double)total_wait_time / number_requests << " cycles\n";
        ss << "Average Request Processing Time: "
           << (double)total_processing_time / number_requests << " cycles\n";
        ss << "Total Cycle Time for Completed Requests: "
           << (double)(total_wait_time + total_processing_time) / number_requests << " cycles\n";
    } else {
        ss << "Average Request Wait Time: N/A (No completed requests)\n";
        ss << "Average Request Processing Time: N/A (No completed requests)\n";
        ss << "Total Cycle Time for Completed Requests: N/A\n";
    }

    // Final snapshot section (only shown if values were provided). [7]
    if (final_requests_in_queue != -1) {
        ss << "Remaining Requests in Queue (at end): " << final_requests_in_queue << "\n";
    } else {
        ss << "Remaining Requests in Queue (at end): N/A (Data not provided)\n";
    }

    if (final_active_servers != -1) {
        ss << "Active Servers (at end): " << final_active_servers << "\n";
        ss << "Inactive Servers (at end): " << final_inactive_servers << "\n";
    } else {
        ss << "Active Servers (at end): N/A (Data not provided)\n";
        ss << "Inactive Servers (at end): N/A (Data not provided)\n";
    }

    return ss.str();
}