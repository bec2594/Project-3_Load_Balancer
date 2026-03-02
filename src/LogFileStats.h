#ifndef LOGFILESTATS_H
#define LOGFILESTATS_H

#include <string>
#include <vector>
#include "Request.h" // For Request class in record_completed_request

class LogFileStats {
public:
    int total_requests_processed; // increment_processed
    int total_servers_added; // increment_servers_added
    int total_servers_removed; // increment_servers_removed
    int max_queue_size; // update_queue_stats
    long long current_queue_size_sum; // update_queue_stats
    int number_queue_samples; // update_queue_stats
    int requests_blocked; // increment_blocked
    long long total_wait_time; // record_completed_request
    long long total_processing_time; // record_completed_request
    int number_requests; // record_completed_request

    // New variables to store final snapshot data, to be set by the Simulator/LoadBalancer
    int final_requests_in_queue;
    int final_active_servers;
    int final_inactive_servers;
    //

    // Constructor sets all stats to 0
    LogFileStats();

    void increment_servers_added();
    void increment_servers_removed();
    void increment_processed();
    void increment_blocked();

    // check if there is a new max queue size, increment the amount of times the queue has been sampled
    // 
    void update_queue_stats(int current_size);

    void record_completed_request(const Request* request);

    // New setter methods for final snapshot data
    void set_final_queue_size(int count);
    void set_final_server_counts(int active, int inactive);
    //

    std::string summary() const;
};

#endif // LOGFILESTATS_H