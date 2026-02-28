#include "LoadBalancer.h"

// functions:
/*
Constructor
LoadBalancer(int LB_id, const Config* config_ptr, LogFileStats* log_file_stats_ptr, int curr_time);
Destructor
~LoadBalancer();
add_request(Request* request)
void dispatch_requests();
void manage_server_count();
void process_time();
void add_blocked_IP_range(const IPRange& range)
int queue_size() const;
int server_count() const;
LogFileStats* get_stats() const;
void log_event(const std::string& message);

private:
void add_server();
void remove_server();
int last_server_check_time();
*/

LoadBalancer::LoadBalancer(int LB_id, const Config* config_ptr, LogFileStats* log_file_stats_ptr, int curr_time):
    id(LB_id),
    time(curr_time),
    config(config_ptr),
    log_file(log_file_stats_ptr),
    last_server_check_time(curr_time) 
{
    // open log file to for this load balancer
    /*
    get filename for this load balancer: config(filename)_LB<id>.txt
    open
    */
    std::string log_file_name = config->log_file_name + "_LB" + std::to_string(id) + ".txt";
    log_file_stream.open(log_file_name, std::ios::app);
    if(!log_file_stream.is_open()) {
        std::cerr << "ERROR opening log file for LoadBalancer" << id << std::endl;
    }

    for(int i = 0; i < config->initial_server_count; i++) {
        add_server();
    }
    log_event("LoadBalancer " + std::to_string(id) +  " initialized with " + std::to_string(web_servers.size()) + " servers.");
}

LoadBalancer::~LoadBalancer() {
    
}