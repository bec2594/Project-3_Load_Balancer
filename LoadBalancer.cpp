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