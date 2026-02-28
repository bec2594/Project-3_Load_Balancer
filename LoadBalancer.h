#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <fstream>


#include "WebServer.h"
#include "Request.h"
#include "Config.h"
#include "LogFileStats.h"
#include "IPRange.h"


class LoadBalancer {
    public:
        int id;
        std::queue<Request*> request_queue;
        std::vector<WebServer*> web_servers;
        int time;
        std::vector<std::string> blocked_ip;
        const Config* config;
        LogFileStats* log_file;
        std::ofstream log_file_stream;
        // Constructor
        /**
         * Constructs Load Balancer
         * LB_id: unique identifier for each load balancer (need multiple [instruction 8])
         * config_ptr: ptr to shared config object (need configuration file [instruction 13])
         * log_file_stats_ptr: ptr to shared LogFileStats object (need log file for summary for LB actions [instruction 13])
         * curr_time: time value for simulation, when the LB gets created ()
         */
        LoadBalancer(int LB_id, const Config* config_ptr, LogFileStats* log_file_stats_ptr, int curr_time);
        // Destructor
        /**
         * Load Balancer Destructor
         * cleans up webservers, requests
         */
        ~LoadBalancer();

        // add request to queue
        /**
         * adds new request to queue
         * request: ptr to request object
         * return true if request added to queue, return false if blocked
         */
        bool add_request(Request* request);

        // dispatch request
        /**
         * sends a request from the queue to an available webserver
         * uses the request queue and web server vector
         */
        void dispatch_requests();

        // manage_server_count
        /**
         * adjust the amount of servers based on the queue size
         * queue size is between 50 and 80 times the number of servers
         */
        void manage_server_count();

        // process_time
        /**
         * process one tick for the Load balancer and the web servers
         * updates the server status (from unused to used) and completed requests (from used to unused)
         */
        void process_time();

        // add blocked ip address range
        /**
         * adds an ip address range to the blocked list blocked_ip
         * address_range: IPRange 
         */
        void add_blocked_IP_range(const IPRange& range);

        // queue_size()
        // returns active number of servers
        int queue_size() const;

        // server count
        // returns the number of servers
        int server_count() const;

        // get stats
        /**
         * access LogFileStats and return pointer to this object
         */
        LogFileStats* get_stats() const;

        // log event
        /**
         * logs event to LogFileStats file
         * message: message to log
         */
        void log_event(const std::string& message);

    private:
        void add_server();
        void remove_server();
        int last_server_check_time;
};

#endif //LOADBALANCER_H