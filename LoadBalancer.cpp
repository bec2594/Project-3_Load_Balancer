#include "LoadBalancer.h"


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
    // clean up request queue
    while(!request_queue.empty()) {
        delete request_queue.front();
        request_queue.pop();
    }

    // clean up web_servers
    // clean up requests first in web servers
    for(WebServer* server : web_servers) {
        if(server->current_request != nullptr) {
            delete server->current_request;
        }
        delete server;
    }
    log_file_stream.close();
}


bool LoadBalancer::add_request(Request* request) {
    // check for ip in firewall 
    for(const auto& range : blocked_ip) {
        if(range.contains(request->IP_in)) {
            // block request, log blocked request, return fail
            log_event("Request " + std::to_string(id) + " was blocked from IP_in " + request->IP_in);
            
            // update log file stats
            log_file->increment_blocked();
            delete request;
            return false;
        }
    }

    // if not blocked add request to queue
    request_queue.push(request);
    log_event("Request " + std::to_string(id) + " was added to queue at IP_in " + request->IP_in + ". Queue size is " + std::to_string(request_queue.size()));
    return true;
}

/*
two cases
1. no servers available
2. no request in queue
*/
void LoadBalancer::dispatch_requests() {
    // go through every request and assign to available server
    while(!request_queue.empty()) {
        WebServer* dispatch_server = nullptr;
        // find an available server
        for(WebServer* server : web_servers) {
            if(server->is_available()) {
                dispatch_server = server;
                break;
            }
        }
        // using found available server, assign request to server
        if(dispatch_server != nullptr) {
            Request* dispatch_request = request_queue.front();
            request_queue.pop();
            dispatch_request->start_time = time;
            dispatch_server->take_request(dispatch_request);
            log_event("Dispatched request " + std::to_string(dispatch_request->id) + " to WebServer " + std::to_string(dispatch_server->id));
        }
        else {break;}
    }
}

void LoadBalancer::manage_server_count() {
    // if time since last time servers were checked is under interval do nothing
    // to keep overhead of checking to often weighing down the workload
    // threshold in config file
    if (time - last_server_check_time < config->server_check_interval_cycles) {return;}

    // update last server check time
    last_server_check_time = time;

    // get thresholds for server count based off of queue size
    /*
    too much in queue = 80 * # of servers
    too little in queue = 50 * # of servers
    */

    int current_server_count = web_servers.size();
    int upper = config->queue_high_count * current_server_count; 
    int lower = config->queue_low_count * current_server_count;

    int current_num_requests = request_queue.size();
    // if not enough requests, remove server and update log file stats
    if(current_num_requests < lower && current_server_count >= 1) {
        remove_server();
        log_event("Removed a server due too few requests " + std::to_string(current_num_requests) + " < lower threshold " + std::to_string(lower) + ". New webserver count: " + std::to_string(web_servers.size()));
        log_file->total_servers_removed++;
    }
    // if too many requests, add server and update log file stats
    else if (current_num_requests > upper) {
        add_server();
        log_event("Added a server due to too many requests " + std::to_string(current_num_requests) + " < upper threshold " + std::to_string(upper) + ". New webserver count: " + std::to_string(web_servers.size()));
        log_file->total_servers_added++;
    }
}


// advance all time for load balancer and webservers time items
// for servers check if requests have finished (server's cycles remaining = 0)
// process request finishing on webserver, update log file, and delete the request
void LoadBalancer::process_time() {
    // advance time for load balancer
    time++;
    log_file->update_queue_stats(request_queue.size());

    for(WebServer* server : web_servers) {
        // advance time for webserver
        server->process_time();
        // check if request finished
        if(server->remaining_cycles <= 0 && server->current_request != nullptr) {
            Request* completed_request = server->processed_request();
            completed_request->completion_time = time;
            log_file->record_completed_request(completed_request);
            log_file->increment_processed();
            log_event("Completed request " + std::to_string(completed_request->id) + " by Webserver " + std::to_string(server->id) + " at time " + std::to_string(completed_request->completion_time));
            delete completed_request;
        }
    }
}


void LoadBalancer::add_blocked_IP_range(const IPRange& range) {
    blocked_ip.push_back(range);
    log_event("Block IP range " + range.start_ip + " to " + range.end_ip);
}


int LoadBalancer::queue_size() const {return request_queue.size();}
int LoadBalancer::server_count() const {return web_servers.size();}
LogFileStats* LoadBalancer::get_stats() const {return log_file;}

void LoadBalancer::log_event(const std::string& message) {
    if(log_file_stream.is_open()) {
        log_file_stream << "(Time: " << time << ") [Load Balancer: " << id << "] : " << message << std::endl;
        std::cout << "(Time: " << time << ") [Load Balancer: " << id << "] : " << message << std::endl;
    }
}


void LoadBalancer::add_server() {
    static int server_id = 0; // never goes back to zero, only assigned 0 at first function call
    web_servers.push_back(new WebServer(server_id++));
    log_event("New webserver added with id " + std::to_string(server_id));
}


void LoadBalancer::remove_server() {
    WebServer* idle = nullptr;
    for(auto server_iter = web_servers.begin(); server_iter != web_servers.end(); server_iter++) {
        if((*server_iter)->is_available()) {
            int remove_id = (*server_iter)->id;
            log_event("Removed Webserver " + std::to_string(remove_id));
            delete *server_iter;
            web_servers.erase(server_iter);
            return;
        }
    }
    // no idle server found
    log_event("No idle servers to remove.");
}