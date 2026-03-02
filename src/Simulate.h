/**
 * @file Simulate.h
 * @brief Declares the Simulate class, which orchestrates configuration loading, initialization,
 *        the main simulation loop, global logging, cleanup, and summary reporting. [15]
 */

#ifndef SIMULATE_H
#define SIMULATE_H

#include <string>
#include <fstream>
#include <vector>
#include <chrono>

#include "LogFileStats.h"
#include "Config.h"
#include "RequestGenerator.h"
#include "LoadBalancer.h"

/**
 * @class Simulate
 * @brief High-level simulation controller that owns the main components and drives the simulation. [15]
 *
 * @details
 * Simulate is responsible for:
 * - Loading configuration values (Config) [15]
 * - Creating the RequestGenerator, LogFileStats collector, and main LoadBalancer [14][15]
 * - Seeding initial conditions (blocked IP ranges and initial queue fill) [14][15]
 * - Running the simulation loop for Config::duration_cycles ticks [14][15]
 * - Writing events to a global log file and printing a final summary [14][15]
 */
class Simulate {
public:
    /**
     * @brief Constructs the simulation controller and attempts to load the configuration file. [14][15]
     *
     * @param config_file Path to the configuration file used to initialize Config. [14][15]
     */
    Simulate(const std::string& config_file);

    /**
     * @brief Destructor; releases owned resources and closes the global log file. [14][15]
     */
    ~Simulate();

    /**
     * @brief Initializes simulation components (generator, stats collector, load balancer) and seeds initial state. [14][15]
     *
     * @return true if initialization succeeds; false otherwise (current implementation returns true). [14]
     */
    bool initialize();

    /**
     * @brief Runs the simulation loop for the configured number of cycles. [14][15]
     */
    void run();

    /**
     * @brief Computes and outputs the final simulation summary to stdout and the log. [14][15]
     */
    void output_summary();

    /**
     * @brief Deletes dynamically allocated components and resets pointers to nullptr. [14][15]
     */
    void cleanup();

private:
    /** @brief Configuration values controlling simulation and generation parameters. [15] */
    Config sim_config;

    /** @brief Generates Request instances according to Config parameters (owned). [14][15] */
    RequestGenerator* request_generator;

    /** @brief Main LoadBalancer instance for the simulation (owned). [14][15] */
    LoadBalancer* main_load_balancer;

    /** @brief Current global simulation time (in cycles). [14][15] */
    int current_sim_time;

    /** @brief Monotonically increasing id counter used when generating new requests. [14][15] */
    int request_id_count;

    /** @brief Global simulation log output stream. [14][15] */
    std::ofstream log_file;

    /** @brief Aggregates statistics across the run for summary reporting (owned). [14][15] */
    LogFileStats* main_stats_collector;

    /**
     * @brief Seeds the initial request queue using Config::initial_queue_fill and initial_server_count. [14][15]
     */
    void initial_queue();

    /**
     * @brief Adds initial blocked IP ranges to the load balancer firewall rules. [14][15]
     */
    void initial_blocked_ips();

    /**
     * @brief Writes a simulation event to the global log file and stdout. [14][15]
     *
     * @param message The message to record.
     */
    void log_event(const std::string message);
};

#endif // SIMULATE_H