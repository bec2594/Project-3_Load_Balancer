#include "Config.h"

#include <unordered_map>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>


// default constructor
Config::Config() : 
    initial_server_count(3),
    queue_low_count(50), // instruction 10
    queue_high_count(80),
    initial_queue_fill(100),
    min_process_time(10),
    max_process_time(100),
    duration_cycles(10000), // deliverable 2
    server_check_interval_cycles(50),
    new_request_probability(0.7),
    log_file_name("log.txt"){}


// AI trimming whitespace
std::string Config::trim_whitespace(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, (last - first + 1));
}


bool Config::load_from_file(const std::string& config_file) {
    // open file to read
    std::ifstream file(config_file);
    if(!file.is_open()) {
        std::cerr << "Error opening config file " << config_file << std::endl;
        return false;
    }
    std::unordered_map<std::string, std::string> config_data;

    for(std::string configuration{}; std::getline(file, configuration);) {
        // skip empty lines and commented lines
        if(configuration.empty() || configuration[0] == '#') {continue;}

        // configuration file will separt key and value using colons so if there is no colon, it isn't a configuration line
        if(configuration.find(':') != std::string::npos) {
            // strip line into different variables key, value
            std::istringstream iss{configuration};
            // getline first reads until the : which separates key and value, the key is saved
            // then remove the leading whitespace after reading to the :
            // then reads the rest for the value, the value is saved
            if(std::string key_ws{}, value_ws{}; std::getline(std::getline(iss, key_ws, ':') >> std::ws, value_ws)) {
                // put into unorderd map
                std::string key = trim_whitespace(key_ws);
                config_data[key] = trim_whitespace(value_ws);
            }
        }
    }
    // assign all key and values to configurations
    for(const auto& [key, value] : config_data) {
        if (key == "initial_server_count") {
                initial_server_count = std::stoi(value);
        } else if (key == "queue_low_count") {
            queue_low_count = std::stoi(value);
        } else if (key == "queue_high_count") {
            queue_high_count = std::stoi(value);
        } else if (key == "initial_queue_fill") {
            initial_queue_fill = std::stoi(value);
        } else if (key == "min_process_time") {
            min_process_time = std::stoi(value);
        } else if (key == "max_process_time") {
            max_process_time = std::stoi(value);
        } else if (key == "duration_cycles") {
            duration_cycles = std::stoi(value);
        } else if (key == "server_check_interval_cycles") {
            server_check_interval_cycles = std::stoi(value);
        } else if (key == "new_request_probability") {
            new_request_probability = std::stod(value); // Use stod for double
        } else if (key == "log_file_name") {
            log_file_name = value; // String doesn't need conversion
        } else {
            std::cerr << "WARNING: Unknown config key encountered: '" << key << "'" << std::endl;
        }
    }
    file.close();
    std::cout << "Config loaded successfully." << std::endl;
    return true;
}