/**
 * @file Simulate.cpp
 * @brief Implements the Simulate class, which coordinates configuration loading, component initialization,
 *        the main simulation loop, event logging, and final summary reporting. [14]
 */

#include "Simulate.h"
#include <iostream>
#include <random>  // For probabilistic request generation. [14]
#include <chrono>  // For seeding random engines. [14]

/**
 * @brief Constructs a Simulate instance and loads configuration / opens the global log file. [14]
 *
 * @details
 * - Attempts to load simulation settings from @p config_file via Config::load_from_file(). [14]
 * - Opens a global simulation log file using Config::log_file_name with a ".txt" suffix. [14]
 *
 * @param config_file Path to the configuration file to load. [14]
 */
Simulate::Simulate(const std::string& config_file) :
    request_generator(nullptr),
    main_load_balancer(nullptr),
    current_sim_time(0),
    request_id_count(0),        // Monotonically increasing id counter for generated requests. [14]
    main_stats_collector(nullptr) // Stats collector allocated during initialize(). [14]
{
    // Load the configuration file (prints an error message if loading fails). [14]
    if(!sim_config.load_from_file(config_file)) {
        std::cerr << "Failed to load configuration file." << std::endl;
    }

    // Open global simulation log file for appending. [14]
    std::string simulation_log_file_name = sim_config.log_file_name + ".txt";
    log_file.open(simulation_log_file_name, std::ios::out | std::ios::app);
    if(!log_file.is_open()) {
        std::cerr << "Failed to open Log File" << sim_config.log_file_name << std::endl;
    }
}

/**
 * @brief Destructor; releases allocated components and closes the global log file. [14]
 */
Simulate::~Simulate() {
    cleanup();     // Deallocates owned heap objects. [14]
    log_file.close();
}

/**
 * @brief Frees heap-allocated simulation components and resets pointers to nullptr. [14]
 *
 * @details
 * Deletes:
 * - RequestGenerator
 * - LogFileStats (main_stats_collector)
 * - LoadBalancer (which in turn deletes its owned WebServers/Requests) [5][14]
 *
 * Also writes cleanup events to the global log. [14]
 */
void Simulate::cleanup() {
    log_event("Simulation cleanup");

    // RequestGenerator is heap-allocated in initialize(). [14]
    delete request_generator;
    request_generator = nullptr;

    // If the load balancer exists, delete the stats collector and then the load balancer. [14]
    if(main_load_balancer) {
        delete main_stats_collector;
        main_stats_collector = nullptr;

        delete main_load_balancer;
        main_load_balancer = nullptr;
    }

    log_event("Simulation cleanup completed");
}

/**
 * @brief Initializes major simulation components (generator, stats collector, load balancer) and seeds initial state. [14]
 *
 * @details
 * - Allocates RequestGenerator using the loaded Config. [14]
 * - Allocates LogFileStats collector. [14]
 * - Allocates LoadBalancer with id=1 and the shared Config/LogFileStats. [14]
 * - Adds the initial blocked IP range and initial queue contents. [14]
 *
 * @return true (this implementation always returns true). [14]
 */
bool Simulate::initialize() {
    log_event("Simulation initializion.");

    request_generator = new RequestGenerator(&sim_config);  // Request generation parameters come from Config. [14]
    main_stats_collector = new LogFileStats();              // Stats collected across the simulation. [14]
    main_load_balancer = new LoadBalancer(1, &sim_config, main_stats_collector, current_sim_time); // Creates initial servers. [5][14]

    log_event("Load Balancer created with " + std::to_string(sim_config.initial_server_count) + "servers");
    log_event("Task Time Range: " + std::to_string(sim_config.min_process_time) + " - " + std::to_string(sim_config.max_process_time));

    initial_blocked_ips(); // Establish firewall rules. [14]
    log_event("Added firewall.");

    initial_queue(); // Seed the starting queue. [14]
    log_event("Initialized queue.");

    return true;
}

/**
 * @brief Populates the initial request queue using initial_queue_fill and initial_server_count. [14]
 *
 * @details
 * Computes:
 * @code
 * initial_queue_size = initial_queue_fill * initial_server_count
 * @endcode
 * Then generates that many requests and submits them to the LoadBalancer. [14]
 */
void Simulate::initial_queue() {
    int initial_queue_size =
        static_cast<int>(sim_config.initial_queue_fill * sim_config.initial_server_count);

    log_event("Initial queue populated with " + std::to_string(initial_queue_size) + " requests.");

    for(int i = 0; i < initial_queue_size; i++) {
        Request* new_request = request_generator->generate_request(current_sim_time, request_id_count);
        main_load_balancer->add_request(new_request);
    }
}

/**
 * @brief Adds the initial blocked IP range to the LoadBalancer firewall list. [14]
 *
 * @details
 * Blocks requests whose IP_in lies in the inclusive range "10.0.0.1" to "10.1.0.1". [14]
 */
void Simulate::initial_blocked_ips() {
    IPRange blocked_ip_range("10.0.0.1", "10.1.0.1");
    main_load_balancer->add_blocked_IP_range(blocked_ip_range);
    log_event("Added blocked IP range from 10.0.0.1 to 10.1.0.1");
}

/**
 * @brief Runs the simulation loop for the configured number of cycles. [14]
 *
 * @details
 * For each cycle:
 * - With probability Config::new_request_probability, generates a new Request and submits it. [14]
 * - Advances load balancer/server time (process_time()) and collects queue stats. [5]
 * - Performs dynamic scaling (manage_server_count()) based on thresholds. [5]
 * - Dispatches queued requests to available servers (dispatch_requests()). [5]
 */
void Simulate::run() {
    log_event("Simulation started for " + std::to_string(sim_config.duration_cycles) + " cycles.");

    std::mt19937 prob_engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);

    for(current_sim_time = 0; current_sim_time < sim_config.duration_cycles; current_sim_time++) {
        if(prob_dist(prob_engine) < sim_config.new_request_probability) {
            Request* new_request = request_generator->generate_request(current_sim_time, request_id_count);
            main_load_balancer->add_request(new_request);
        }

        main_load_balancer->process_time();        // Advances server work and records completions. [5]
        main_load_balancer->manage_server_count(); // Adds/removes servers based on queue size. [5]
        main_load_balancer->dispatch_requests();   // Assigns queued work to idle servers. [5]
    }

    log_event("Simulation finished.");
}

/**
 * @brief Logs a simulation event to the global log file (if open) and to stdout. [14]
 *
 * @param message Event message text to log.
 */
void Simulate::log_event(const std::string message) {
    if (log_file.is_open()) {
        log_file << "[Global Time " << current_sim_time << "] SIM: " << message << std::endl;
    }
    std::cout << "[Global Time " << current_sim_time << "] SIM: " << message << std::endl;
}

/**
 * @brief Computes and outputs final summary statistics. [14]
 *
 * @details
 * - Counts remaining requests in the LoadBalancer queue. [14]
 * - Counts active vs inactive servers by checking WebServer::is_available(). [14]
 * - Stores these final snapshot values in LogFileStats and prints the generated summary. [7][14]
 */
void Simulate::output_summary() {
    log_event("\n--- FINAL SIMULATION SUMMARY ---\n");

    int final_queue_size = static_cast<int>(main_load_balancer->request_queue.size());
    int active_servers = 0;
    int inactive_servers = 0;

    for (const WebServer* server : main_load_balancer->web_servers) {
        if (server->is_available()) {
            inactive_servers++;
        } else {
            active_servers++;
        }
    }

    main_stats_collector->set_final_queue_size(final_queue_size);                 // Adds final queue snapshot. [7][14]
    main_stats_collector->set_final_server_counts(active_servers, inactive_servers); // Adds final server snapshot. [7][14]

    std::cout << main_stats_collector->summary();      // Prints formatted summary. [7][14]
    log_event(main_stats_collector->summary());        // Logs formatted summary. [7][14]
}