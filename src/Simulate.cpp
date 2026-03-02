#include "Simulate.h"
#include <iostream>
#include <random> // making new requests
#include <chrono> // for seeding random engine

Simulate::Simulate(const std::string& config_file) :
    request_generator(nullptr),
    main_load_balancer(nullptr),
    current_sim_time(0),
    request_id_count(0), // Initializing the member variable request_id_count
    main_stats_collector(nullptr) // Initialize the new stats collector member
{
    // load config_file
    if(!sim_config.load_from_file(config_file)) {
        std::cerr << "Failed to load configuration file." << std::endl;
    }
    // open log file to append using config log file name
    std::string simulation_log_file_name = sim_config.log_file_name + ".txt";
    log_file.open(simulation_log_file_name, std::ios::out | std::ios::app);
    if(!log_file.is_open()) {
        std::cerr << "Failed to open Log File" << sim_config.log_file_name << std::endl;
    }
}

Simulate::~Simulate() {
    cleanup(); // deallocates memory objects
    log_file.close();
}
void Simulate::cleanup() {
    log_event("Simulation cleanup");
    // request_generator has no allocated memory
    delete request_generator;
    request_generator = nullptr;
    
    // Load Balancer has allocated memory to log_file
    if(main_load_balancer) {
        // Correctly delete the LogFileStats object owned by Simulate
        delete main_stats_collector;
        main_stats_collector = nullptr;

        delete main_load_balancer; // handles the requests, webservers, etc.
        main_load_balancer = nullptr; // Set pointer to null after deletion
    }
    log_event("Simulation cleanup completed");
}

bool Simulate::initialize() {
    log_event("Simulation initializion.");
    
    request_generator = new RequestGenerator(&sim_config);
    main_stats_collector = new LogFileStats(); // Allocate and assign to the member
    main_load_balancer = new LoadBalancer(1, &sim_config, main_stats_collector, current_sim_time);
    log_event("Load Balancer created with " + std::to_string(sim_config.initial_server_count) + "servers");
    log_event("Task Time Range: " + std::to_string(sim_config.min_process_time) + " - " + std::to_string(sim_config.max_process_time));
    initial_blocked_ips();
    log_event("Added firewall.");
    initial_queue();
    log_event("Initialized queue.");
    return true;
}
void Simulate::initial_queue() {
    int initial_queue_size = static_cast<int>(sim_config.initial_queue_fill * sim_config.initial_server_count); // Corrected config variable name
    log_event("Initial queue populated with " + std::to_string(initial_queue_size) + " requests.");
    // loop initial queue size times and add it new request to the load balancer
    for(int i = 0; i < initial_queue_size; i++) {
        Request* new_request = request_generator->generate_request(current_sim_time, request_id_count);
        main_load_balancer->add_request(new_request);
    }
}

void Simulate::initial_blocked_ips() {
    IPRange blocked_ip_range("10.0.0.1", "10.1.0.1");
    main_load_balancer->add_blocked_IP_range(blocked_ip_range);
    log_event("Added blocked IP range from 10.0.0.1 to 10.1.0.1");
}
void Simulate::run() {
    log_event("Simulation started for " + std::to_string(sim_config.duration_cycles) + " cycles.");
    
    std::mt19937 prob_engine(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
    
    for(current_sim_time = 0; current_sim_time < sim_config.duration_cycles; current_sim_time++) {
        if(prob_dist(prob_engine) < sim_config.new_request_probability) { // Corrected config variable name
            Request* new_request = request_generator->generate_request(current_sim_time, request_id_count);
            main_load_balancer->add_request(new_request);
        }
        main_load_balancer->process_time(); // This now handles manage_server_count and dispatch_requests internally
        main_load_balancer->manage_server_count();
        main_load_balancer->dispatch_requests();
    }

    log_event("Simulation finished.");
}
void Simulate::log_event(const std::string message) {
    if (log_file.is_open()) {
        log_file << "[Global Time " << current_sim_time << "] SIM: " << message << std::endl;
    }
    std::cout << "[Global Time " << current_sim_time << "] SIM: " << message << std::endl;
}

void Simulate::output_summary() {
    log_event("\n--- FINAL SIMULATION SUMMARY ---\n");

    int final_queue_size = main_load_balancer->request_queue.size(); // Current requests still in queue
    int active_servers = 0;
    int inactive_servers = 0;
    for (const WebServer* server : main_load_balancer->web_servers) {
        if (server->is_available()) { // Based on WebServer::is_available() logic
            inactive_servers++;
        } else {
            active_servers++;
        }
    }

    main_stats_collector->set_final_queue_size(final_queue_size);
    main_stats_collector->set_final_server_counts(active_servers, inactive_servers);

    std::cout << main_stats_collector->summary(); // Corrected to use member and correct method name
    log_event(main_stats_collector->summary()); // Corrected to use member and correct method name
}