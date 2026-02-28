#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
    public:
        int initial_server_count;
        int queue_low_count;
        int queue_high_count;
        int initial_queue_fill;
        int min_process_time;
        int max_process_time;
        int duration_cycles;
        int server_check_interval_cycles;
        double new_request_probability; // 0.0->1.0
        std::string log_file_name; // name of log file

        // constructor
        Config();

        // loads configuration parameters from file (input configuration file [instruction 13])
        // return true if loading was successful, false if fails
        bool load_from_file(const std::string& file);

        private:
            // function to parse config lines
            void parseLine(const std::string& line);
};

#endif // CONFIG_H