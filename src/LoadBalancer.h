/**
 * @file LoadBalancer.h
 * @brief Declares the LoadBalancer class, which manages request queuing, dispatching to WebServers,
 *        dynamic server scaling, firewall IP blocking, and per-load-balancer logging. [6]
 */

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

/**
 * @class LoadBalancer
 * @brief Coordinates a pool of WebServer instances by accepting Requests, filtering blocked IPs,
 *        dispatching work, and scaling server count based on queue thresholds. [6]
 */
class LoadBalancer {
public:
    /** @brief Unique identifier for this load balancer instance. [6] */
    int id;

    /** @brief FIFO queue of pending requests owned by the load balancer. [6] */
    std::queue<Request*> request_queue;

    /** @brief List of managed WebServer instances (owned by the load balancer). [6] */
    std::vector<WebServer*> web_servers;

    /** @brief Current local time counter for this load balancer (simulation cycles). [6] */
    int time;

    /** @brief List of blocked incoming IP ranges (firewall rules). [6] */
    std::vector<IPRange> blocked_ip;

    /** @brief Pointer to shared configuration data (not owned). [6] */
    const Config* config;

    /** @brief Pointer to shared statistics collector (not owned by LoadBalancer in this design). [6] */
    LogFileStats* log_file;

    /** @brief Output stream for per-load-balancer event logging. [6] */
    std::ofstream log_file_stream;

    /**
     * @brief Constructs a LoadBalancer. [6]
     *
     * @param LB_id Unique identifier for each load balancer (supports multiple instances). [6]
     * @param config_ptr Pointer to shared configuration object. [6]
     * @param log_file_stats_ptr Pointer to shared LogFileStats collector for summary reporting. [6]
     * @param curr_time Initial simulation time for this load balancer. [6]
     */
    LoadBalancer(int LB_id, const Config* config_ptr, LogFileStats* log_file_stats_ptr, int curr_time);

    /**
     * @brief Destructor; cleans up owned WebServers and any remaining Requests. [6]
     */
    ~LoadBalancer();

    /**
     * @brief Adds a request to the queue unless it is blocked by firewall IP ranges. [6]
     *
     * @param request Pointer to a Request object.
     * @return true if the request is queued; false if it is blocked (and deleted). [6]
     */
    bool add_request(Request* request);

    /**
     * @brief Sends queued requests to available WebServers. [6]
     *
     * Dispatch continues until either the queue is empty or no WebServer is available. [6]
     */
    void dispatch_requests();

    /**
     * @brief Adjusts the number of WebServers based on current queue size thresholds. [6]
     *
     * Queue thresholds are scaled by the current server count (see Config values). [6]
     */
    void manage_server_count();

    /**
     * @brief Advances one simulation tick for the load balancer and its WebServers. [6]
     *
     * Updates time, processes servers, and finalizes completed requests (recording stats). [6]
     */
    void process_time();

    /**
     * @brief Adds an IP range to the blocked firewall list. [6]
     *
     * @param range IPRange to block.
     */
    void add_blocked_IP_range(const IPRange& range);

    /**
     * @brief Returns the number of requests currently waiting in the queue. [6]
     * @return Current queue size.
     */
    int queue_size() const;

    /**
     * @brief Returns the number of WebServers currently managed by this load balancer. [6]
     * @return Current server count.
     */
    int server_count() const;

    /**
     * @brief Returns the LogFileStats collector associated with this load balancer. [6]
     * @return Pointer to LogFileStats.
     */
    LogFileStats* get_stats() const;

    /**
     * @brief Logs an event message to the per-load-balancer log stream (and typically stdout). [6]
     * @param message Message to log.
     */
    void log_event(const std::string& message);

private:
    /**
     * @brief Creates and adds a new WebServer to the managed pool. [6]
     */
    void add_server();

    /**
     * @brief Removes one idle WebServer from the pool, if available. [6]
     */
    void remove_server();

    /** @brief The last simulation time at which scaling decisions were evaluated. [6] */
    int last_server_check_time;
};

#endif // LOADBALANCER_H