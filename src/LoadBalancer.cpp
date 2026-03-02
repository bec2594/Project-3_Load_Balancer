/**
 * @file LoadBalancer.cpp
 * @brief Implements the LoadBalancer class responsible for queuing, dispatching, firewall blocking,
 *        dynamic server scaling, and per-load-balancer logging. [5]
 */

#include "LoadBalancer.h"

/**
 * @brief Constructs a LoadBalancer and initializes its server pool and log stream. [5]
 *
 * Opens a per-load-balancer log file named "<log_file_name>_LB<id>.txt" and creates the initial
 * set of web servers as specified by the shared Config object. [5]
 *
 * @param LB_id Unique identifier for this load balancer instance. [6]
 * @param config_ptr Pointer to a shared configuration object. [6]
 * @param log_file_stats_ptr Pointer to a shared statistics collector used for summary reporting. [6]
 * @param curr_time Initial simulation time for this load balancer. [6]
 */
LoadBalancer::LoadBalancer(int LB_id, const Config* config_ptr, LogFileStats* log_file_stats_ptr, int curr_time):
    id(LB_id),
    time(curr_time),
    config(config_ptr),
    log_file(log_file_stats_ptr),
    last_server_check_time(curr_time)
{
    /**
     * @details
     * The log file name is derived from the configured base log name plus a suffix identifying
     * this load balancer instance (e.g., "simulation_log_LB1.txt"). [5]
     */
    std::string log_file_name = config->log_file_name + "_LB" + std::to_string(id) + ".txt";
    log_file_stream.open(log_file_name, std::ios::app);
    if(!log_file_stream.is_open()) {
        std::cerr << "ERROR opening log file for LoadBalancer" << id << std::endl;
    }

    // Create the initial pool of web servers and update stats for each one added. [5]
    for(int i = 0; i < config->initial_server_count; i++) {
        add_server();
        log_file->increment_servers_added();
    }

    log_event("LoadBalancer " + std::to_string(id) +  " initialized with " +
              std::to_string(web_servers.size()) + " servers.");
}

/**
 * @brief Destructor that releases all dynamically allocated Requests and WebServers. [5]
 *
 * @details
 * - Deletes any requests still in the queue.
 * - Deletes any request currently owned by a WebServer.
 * - Deletes each WebServer instance.
 * - Closes the per-load-balancer log stream. [5]
 */
LoadBalancer::~LoadBalancer() {
    // Clean up request queue (requests not yet dispatched). [5]
    while(!request_queue.empty()) {
        delete request_queue.front();
        request_queue.pop();
    }

    // Clean up web servers and any in-progress request they own. [5]
    for(WebServer* server : web_servers) {
        if(server->current_request != nullptr) {
            delete server->current_request;
        }
        delete server;
    }

    log_file_stream.close();
}

/**
 * @brief Adds a request to the queue unless it is blocked by the firewall rules. [5]
 *
 * @details
 * The request is checked against each blocked IP range; if its IP_in is within any blocked range,
 * the request is discarded, a blocked counter is incremented, and false is returned. [5]
 *
 * @param request Pointer to the Request to enqueue (ownership is taken by the LoadBalancer on success). [6]
 * @return true if the request was accepted into the queue; false if it was blocked and deleted. [5]
 */
bool LoadBalancer::add_request(Request* request) {
    // Firewall check: block requests whose incoming IP falls inside any blocked IPRange. [5]
    for(const auto& range : blocked_ip) {
        if(range.contains(request->IP_in)) {
            log_event("Request " + std::to_string(id) + " was blocked from IP_in " + request->IP_in);

            // Update stats and free the request since it will not be processed. [5]
            log_file->increment_blocked();
            delete request;
            return false;
        }
    }

    // Not blocked: enqueue request and log. [5]
    request_queue.push(request);
    log_event("Request " + std::to_string(request->id) + " was added to queue at IP_in " +
              request->IP_in + ". Queue size is " + std::to_string(request_queue.size()));
    return true;
}

/**
 * @brief Dispatches queued requests to available WebServers until no more can be dispatched. [5]
 *
 * @details
 * Continues while there are queued requests and at least one available server. For each dispatched
 * request, the request's start_time is set to the current load balancer time and the request is
 * handed to a WebServer. [5]
 */
void LoadBalancer::dispatch_requests() {
    // Attempt to dispatch as many requests as possible. [5]
    while(!request_queue.empty()) {
        WebServer* dispatch_server = nullptr;

        // Find an available server (first-fit). [5]
        for(WebServer* server : web_servers) {
            if(server->is_available()) {
                dispatch_server = server;
                break;
            }
        }

        // If an available server exists, dispatch the next request; otherwise stop. [5]
        if(dispatch_server != nullptr) {
            Request* dispatch_request = request_queue.front();
            request_queue.pop();

            dispatch_request->start_time = time;
            dispatch_server->take_request(dispatch_request);

            log_event("Dispatched request " + std::to_string(dispatch_request->id) +
                      " to WebServer " + std::to_string(dispatch_server->id));
        } else {
            break;
        }
    }
}

/**
 * @brief Adds or removes servers based on queue size thresholds, at configured intervals. [5]
 *
 * @details
 * This method only performs scaling checks if enough time has elapsed since the last check
 * (server_check_interval_cycles). [5]
 *
 * Thresholds are computed as:
 * - upper = queue_high_count * current_server_count
 * - lower = queue_low_count  * current_server_count
 *
 * If current queue size is below lower, an idle server may be removed.
 * If current queue size is above upper, a server is added. [5]
 */
void LoadBalancer::manage_server_count() {
    // Only check scaling at the configured interval to reduce overhead. [5]
    if (time - last_server_check_time < config->server_check_interval_cycles) { return; }
    last_server_check_time = time;

    // Compute scaling thresholds based on current server count. [5]
    int current_server_count = static_cast<int>(web_servers.size());
    int upper = config->queue_high_count * current_server_count;
    int lower = config->queue_low_count * current_server_count;

    int current_num_requests = static_cast<int>(request_queue.size());

    // Scale down if queue is too small (attempts to remove an idle server). [5]
    if(current_num_requests < lower && current_server_count >= 1) {
        remove_server();
        log_file->increment_servers_removed();
        log_event("Removed a server due too few requests " + std::to_string(current_num_requests) +
                  " < lower threshold " + std::to_string(lower) +
                  ". New webserver count: " + std::to_string(web_servers.size()));
    }
    // Scale up if queue is too large. [5]
    else if (current_num_requests > upper) {
        add_server();
        log_file->increment_servers_added();
        log_event("Added a server due to too many requests " + std::to_string(current_num_requests) +
                  " > upper threshold " + std::to_string(upper) +
                  ". New webserver count: " + std::to_string(web_servers.size()));
    }
}

/**
 * @brief Advances simulation time by one tick and processes all WebServers. [5]
 *
 * @details
 * - Increments the load balancer time counter.
 * - Samples queue size for statistics.
 * - Advances each WebServer by one cycle.
 * - If a server finishes a request, records timing stats, increments processed count,
 *   logs completion, and deletes the completed request. [5]
 */
void LoadBalancer::process_time() {
    // Advance time for the load balancer. [5]
    time++;

    // Update queue statistics (max/average sampling). [5]
    log_file->update_queue_stats(static_cast<int>(request_queue.size()));

    // Process time for each server and finalize completed requests. [5]
    for(WebServer* server : web_servers) {
        server->process_time();

        // If a request has completed, extract it, record stats, and free memory. [5]
        if(server->remaining_cycles <= 0 && server->current_request != nullptr) {
            Request* completed_request = server->processed_request();
            if(completed_request != nullptr) {
                completed_request->completion_time = time;

                log_file->record_completed_request(completed_request);
                log_file->increment_processed();

                log_event("Completed request " + std::to_string(completed_request->id) +
                          " by Webserver " + std::to_string(server->id) +
                          " at time " + std::to_string(completed_request->completion_time));

                delete completed_request;
            }
        }
    }
}

/**
 * @brief Adds a blocked IP range to the firewall list. [5]
 *
 * Requests with IP_in inside this range will be rejected in add_request(). [5]
 *
 * @param range IPRange to add to the blocked list. [6]
 */
void LoadBalancer::add_blocked_IP_range(const IPRange& range) {
    blocked_ip.push_back(range);
    log_event("Block IP range " + range.start_ip + " to " + range.end_ip);
}

/**
 * @brief Returns the number of requests currently waiting in the queue.
 * @return Current request queue size. [5]
 */
int LoadBalancer::queue_size() const { return static_cast<int>(request_queue.size()); }

/**
 * @brief Returns the number of WebServers currently managed by the load balancer.
 * @return Current server count. [5]
 */
int LoadBalancer::server_count() const { return static_cast<int>(web_servers.size()); }

/**
 * @brief Returns the shared LogFileStats collector used by this load balancer.
 * @return Pointer to the associated LogFileStats object. [5]
 */
LogFileStats* LoadBalancer::get_stats() const { return log_file; }

/**
 * @brief Logs an event message to the per-load-balancer log file and stdout. [5]
 *
 * @param message Message to write.
 */
void LoadBalancer::log_event(const std::string& message) {
    if(log_file_stream.is_open()) {
        log_file_stream << "(Time: " << time << ") [Load Balancer: " << id << "] : "
                        << message << std::endl;
        std::cout << "(Time: " << time << ") [Load Balancer: " << id << "] : "
                  << message << std::endl;
    }
}

/**
 * @brief Allocates and appends a new WebServer to the managed server list. [5]
 *
 * @details
 * Uses a static counter to assign unique server ids over the lifetime of the program. [5]
 */
void LoadBalancer::add_server() {
    static int server_id = 0; // Assigned only once; monotonically increases. [5]
    web_servers.push_back(new WebServer(server_id++));
    log_event("New webserver added with id " + std::to_string(server_id));
}

/**
 * @brief Removes one idle WebServer (if available) from the managed server list. [5]
 *
 * @details
 * Searches for the first available (idle) server and deletes it. If no idle server exists,
 * logs that removal was not possible. [5]
 */
void LoadBalancer::remove_server() {
    for(auto server_iter = web_servers.begin(); server_iter != web_servers.end(); server_iter++) {
        if((*server_iter)->is_available()) {
            int remove_id = (*server_iter)->id;
            log_event("Removed Webserver " + std::to_string(remove_id));
            delete *server_iter;
            web_servers.erase(server_iter);
            return;
        }
    }

    // No idle server was found. [5]
    log_event("No idle servers to remove.");
}