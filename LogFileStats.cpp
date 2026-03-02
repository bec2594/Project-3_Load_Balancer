#include "LogFileStats.h"

#include <algorithm>
#include <sstream>
#include <iomanip>

// Constructor sets all stats to 0
LogFileStats::LogFileStats() :
    total_requests_processed(0), // increment_processed
    total_servers_added(0), // increment_servers_added
    total_servers_removed(0), // increment_servers_removed
    max_queue_size(0), // update_queue_stats
    current_queue_size_sum(0), // update_queue_stats
    number_queue_samples(0), // update_queue_stats
    requests_blocked(0), // increment_blocked
    total_wait_time(0), // record_completed_request
    total_processing_time(0), // record_completed_request
    number_requests(0) {} // record_completed_request



void LogFileStats::increment_servers_added() {total_servers_added++;}
void LogFileStats::increment_servers_removed() {total_servers_removed++;}
void LogFileStats::increment_processed() {total_requests_processed++;}
void LogFileStats::increment_blocked() {requests_blocked++;}
// check if there is a new max queue size, increment the amount of times the queue has been sampled
// 
void LogFileStats::update_queue_stats(int current_size) {
    max_queue_size = std::max(max_queue_size, current_size);
    // stats to find the average queue size
    number_queue_samples++;
    current_queue_size_sum += current_size;
}


void LogFileStats::record_completed_request(const Request* request) {
    if(request != nullptr && request->start_time != -1 && request->completion_time != -1) {
        total_wait_time += (request->start_time - request->arrival_time);
        total_processing_time += (request->completion_time - request->start_time);
        number_requests++;
    }
}



std::string LogFileStats::summary() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2); // For average calculations

    ss << "\n--- Simulation Summary ---\n";
    ss << "Total Requests Processed: " << total_requests_processed << "\n";
    ss << "Requests Blocked (Firewall): " << requests_blocked << "\n";
    ss << "Servers Added: " << total_servers_added << "\n";
    ss << "Servers Removed: " << total_servers_removed << "\n";
    ss << "Max Queue Size Observed: " << max_queue_size << "\n";

    if (number_queue_samples > 0) {
        ss << "Average Queue Size: " << (double)current_queue_size_sum / number_queue_samples << "\n";
    } else {
        ss << "Average Queue Size: N/A (No samples)\n";
    }

    if (number_requests > 0) {
        ss << "Average Request Wait Time: " << (double)total_wait_time / number_requests << " cycles\n";
        ss << "Average Request Processing Time: " << (double)total_processing_time / number_requests << " cycles\n";
        ss << "Total Cycle Time for Completed Requests: " << (double)(total_wait_time + total_processing_time) / number_requests << " cycles\n";
    } else {
        ss << "Average Request Wait Time: N/A (No completed requests)\n";
        ss << "Average Request Processing Time: N/A (No completed requests)\n";
        ss << "Total Cycle Time for Completed Requests: N/A\n";
    }
    return ss.str();
}