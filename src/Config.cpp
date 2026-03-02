/**
 * @file Config.cpp
 * @brief Implements the Config class used to store defaults and load simulation settings from a file.
 */

#include "Config.h"

#include <unordered_map>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

/**
 * @brief Default constructor that initializes all configuration fields to reasonable defaults.
 *
 * These defaults are used if no configuration file is provided or if specific keys are missing.
 */
Config::Config() :
    initial_server_count(3),
    queue_low_count(50),              // Lower queue threshold factor (see LoadBalancer scaling logic) [1]
    queue_high_count(80),             // Upper queue threshold factor (see LoadBalancer scaling logic) [1]
    initial_queue_fill(0.5),          // Fraction used to pre-fill the queue at startup [1]
    min_process_time(10),             // Minimum request processing time (cycles) [1]
    max_process_time(100),            // Maximum request processing time (cycles) [1]
    duration_cycles(10000),           // Total simulation duration (cycles) [1]
    server_check_interval_cycles(50), // How often to consider adding/removing servers [1]
    new_request_probability(0.3),     // Probability of generating a request per cycle [1]
    log_file_name("log.txt")          // Base name for log output [1]
{}

/**
 * @brief Trims leading and trailing whitespace from a string.
 *
 * Whitespace characters removed include spaces, tabs, and common newline/control characters.
 *
 * @param str Input string that may contain leading/trailing whitespace.
 * @return A new string with leading/trailing whitespace removed.
 */
std::string Config::trim_whitespace(const std::string& str) {
    // Find first non-whitespace character.
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    // If the string is all whitespace (or empty), return it unchanged.
    if (std::string::npos == first) {
        return str;
    }
    // Find last non-whitespace character.
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    // Return the substring bounded by first..last (inclusive).
    return str.substr(first, (last - first + 1));
}

/**
 * @brief Loads configuration parameters from a colon-separated configuration file.
 *
 * The loader:
 * - Opens the file at @p config_file
 * - Ignores empty lines and lines beginning with '#'
 * - Expects "key: value" pairs
 * - Trims whitespace from keys/values
 * - Assigns recognized keys to member variables
 * - Prints a warning for unknown keys
 *
 * @param config_file Path to the configuration file to read.
 * @return true if the file could be opened and parsed; false if the file could not be opened.
 */
bool Config::load_from_file(const std::string& config_file) {
    // Open file for reading.
    std::ifstream file(config_file);
    if(!file.is_open()) {
        std::cerr << "Error opening config file " << config_file << std::endl;
        return false;
    }

    // Temporary storage for parsed key/value pairs.
    std::unordered_map<std::string, std::string> config_data;

    // Read the file line-by-line.
    for(std::string configuration{}; std::getline(file, configuration);) {

        // Skip empty lines and comment lines.
        if(configuration.empty() || configuration[0] == '#') { continue; }

        // Only process lines that contain ':' (key-value separator).
        if(configuration.find(':') != std::string::npos) {

            // Split the line into key and value around the colon.
            std::istringstream iss{configuration};

            // Read key up to ':' and then read the rest as the value.
            // ">> std::ws" removes leading whitespace before reading the value.
            if(std::string key_ws{}, value_ws{};
               std::getline(std::getline(iss, key_ws, ':') >> std::ws, value_ws)) {

                // Trim both key and value and store them.
                std::string key = trim_whitespace(key_ws);
                config_data[key] = trim_whitespace(value_ws);
            }
        }
    }

    // Assign parsed key/value pairs to class members.
    for(const auto& [key, value] : config_data) {
        if (key == "initial_server_count") {
            initial_server_count = std::stoi(value);
        } else if (key == "queue_low_count") {
            queue_low_count = std::stoi(value);
        } else if (key == "queue_high_count") {
            queue_high_count = std::stoi(value);
        } else if (key == "initial_queue_fill") {
            initial_queue_fill = std::stod(value);
        } else if (key == "min_process_time") {
            min_process_time = std::stoi(value);
        } else if (key == "max_process_time") {
            max_process_time = std::stoi(value);
        } else if (key == "duration_cycles") {
            duration_cycles = std::stoi(value);
        } else if (key == "server_check_interval_cycles") {
            server_check_interval_cycles = std::stoi(value);
        } else if (key == "new_request_probability") {
            // Stored as a floating-point probability in [0.0, 1.0].
            new_request_probability = std::stod(value);
        } else if (key == "log_file_name") {
            // Strings do not require numeric conversion.
            log_file_name = value;
        } else {
            // Unknown keys are not fatal, but will be reported to help debug configuration issues.
            std::cerr << "WARNING: Unknown config key encountered: '" << key << "'" << std::endl;
        }
    }

    // Close the file and report success.
    file.close();
    std::cout << "Config loaded successfully." << std::endl;
    return true;
}