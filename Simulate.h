#ifndef SIMULATE_H
#define SIMULATE_H

#include <string>
#include <fstream>
#include <vector>
#include <chrono>

#include "Config.h"
#include "RequestGenerator.h"
#include "LoadBalancer.h"
class Simulate {

    public:
        //constructor
        Simulate(const std::string& config_file);
        // destructor
        ~Simulate();
        
        bool Simulate::initialize(); // initializes all components (config, LB, Requests, WebServers)
        void Simulate::run(); // runs simulation loop based on configurations
        void Simulate::output_summary(); // summary of simulation results to log file
        void cleanup(); // cleanup for destructor, gets rid of all allocated memory

    private:
        Config sim_config; // configuration values for LB, requests generator, simulation times
        RequestGenerator* request_generator;
        LoadBalancer* main_load_balancer;
        int current_sim_time;
        int request_id_count; // continuously increases for each request
        std::ofstream log_file;

        void initial_queue(); // fills the initial queue for requests
        void initial_blocked_ips(); // adds blocked ips to blocked ip list
        void log_event(const std::string message);
};



#endif // SIMUALTE_H