#ifndef LOGFILESTATS_H
#define LOGFILESTATS_H

#include <string>
#include <vector>
#include "Request.h"

class LogFileStats {
    public:
        int total_requests_processed;
        int total_servers_added;
        int total_servers_removed;
        int max_queue_size;
        long long current_queue_size_sum;
        int number_queue_samples;
        int requests_blocked;
        long long total_wait_time;
        long long total_processing_time;
        int number_requests;

        // Constructor initialize all to 0
        LogFileStats();


        // inccrement total_servers_added
        // called in the manage servers function in LoadBalancer
        void increment_servers_added();

        // increment total_servers_removed 
        // called in the manage servers function in LoadBalancer
        void increment_servers_removed();

        
        // update queue stats
        /**
         * updates queue statistics
         * max_queue_size, number_queue_samples, current_queue_size, 
         */
        void update_queue_stats(int current_size);

        // increment processed
        /**
         * increments count of total requests processed
         */
        void increment_processed();

        // record completed request
        /**
         * records details of completed request to update timing
         * request: pointer to completed request object
         */

        void record_completed_request(const Request* request);

        // increment blocked: increment count of requests blocked by firewall
        void increment_blocked();

        // generates summary of all statistics
        // returns string containing simulation summary
        std::string summary() const;

};

#endif // LOGFILESTATS_H