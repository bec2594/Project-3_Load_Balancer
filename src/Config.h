/**
 * @file Config.h
 * @brief Declares the Config class, which stores simulation settings and loads them from a config file.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

/**
 * @class Config
 * @brief Holds configuration parameters for the load balancer simulation.
 *
 * This class provides default values (via the constructor) and supports loading
 * overrides from a colon-separated configuration file. See load_from_file().
 */
class Config {
public:
    /** @brief Initial number of WebServer instances to create. */
    int initial_server_count;

    /** @brief Lower queue threshold multiplier used for scaling server count. */
    int queue_low_count;

    /** @brief Upper queue threshold multiplier used for scaling server count. */
    int queue_high_count;

    /** @brief Initial queue fill factor (used to seed the request queue at startup). */
    double initial_queue_fill;

    /** @brief Minimum request processing time (in cycles). */
    int min_process_time;

    /** @brief Maximum request processing time (in cycles). */
    int max_process_time;

    /** @brief Total simulation runtime duration (in cycles). */
    int duration_cycles;

    /** @brief Interval (in cycles) between server add/remove checks. */
    int server_check_interval_cycles;

    /** @brief Probability in [0.0, 1.0] that a new request arrives in a given cycle. */
    double new_request_probability; // 0.0->1.0

    /** @brief Base name used for log output files. */
    std::string log_file_name; // name of log file

    /**
     * @brief Constructs a Config object with default settings.
     */
    Config();

    /**
     * @brief Trims leading and trailing whitespace from a string.
     * @param str Input string.
     * @return A trimmed copy of @p str.
     */
    std::string trim_whitespace(const std::string& str);

    /**
     * @brief Loads configuration parameters from a file.
     *
     * The file is expected to contain lines formatted as:
     * @code
     * key: value
     * @endcode
     * Empty lines and lines beginning with '#' are ignored.
     *
     * Recognized keys include (as implemented): initial_server_count, queue_low_count,
     * queue_high_count, initial_queue_fill, min_process_time, max_process_time,
     * duration_cycles, server_check_interval_cycles, new_request_probability, log_file_name [1].
     *
     * @param config_file Path to the configuration file.
     * @return true if the file opens successfully and is parsed; false otherwise.
     */
    bool load_from_file(const std::string& config_file);
};

#endif // CONFIG_H