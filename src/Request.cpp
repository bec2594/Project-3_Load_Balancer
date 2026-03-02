/**
 * @file Request.cpp
 * @brief Implements the Request class, which represents a single simulated workload request and
 *        provides formatted output for debugging/logging. [10]
 */

#include "Request.h"
#include <string>    // For std::string. [10]
#include <utility>   // For std::move. [10]
#include <iostream>  // For std::endl. [10]

/**
 * @brief Constructs a Request with the provided identifiers, IPs, processing time, type, and arrival time. [10]
 *
 * @details
 * The constructor initializes:
 * - id with @p request_id
 * - IP_in/IP_out using move semantics to avoid extra string copies
 * - processing_time and job_type
 * - arrival_time
 * - start_time and completion_time to -1 (meaning "not yet started/completed") [10]
 *
 * @param request_id Unique numeric id for the request. [11]
 * @param ip_in Source IP address (incoming). [11]
 * @param ip_out Destination IP address (outgoing). [11]
 * @param process_time Number of cycles required to process the request. [11]
 * @param type Job type code (e.g., 'P' or 'S'). [11]
 * @param arrival_time_param Simulation time when the request arrived. [10]
 */
Request::Request(int request_id, std::string ip_in, std::string ip_out,
                 int process_time, char type, int arrival_time_param)
    : id(request_id),
      IP_in(std::move(ip_in)),
      IP_out(std::move(ip_out)),
      processing_time(process_time),
      job_type(type),
      arrival_time(arrival_time_param),
      start_time(-1),
      completion_time(-1)
{}

/**
 * @brief Prints a human-readable representation of the request to an output stream. [10]
 *
 * @details
 * Always prints the basic request fields (id, IPs, processing time, type, arrival time).
 * If the request has started, prints start_time.
 * If the request has completed, prints completion_time and derived metrics:
 * - Total System Time = completion_time - arrival_time
 * - Wait Time         = start_time - arrival_time [10]
 *
 * @param os Output stream to write to (e.g., std::cout or a file stream). [10]
 */
void Request::print(std::ostream& os) const {
    os << "Req ID: " << id
       << " | IP In: " << IP_in
       << " | IP Out: " << IP_out
       << " | Proc Time: " << processing_time
       << " | Type: " << job_type
       << " | Arrival: " << arrival_time;

    // Only print start time if the request has been dispatched to a server. [10]
    if (start_time != -1) {
        os << " | Start: " << start_time;
    }

    // Only print completion and derived metrics if processing finished. [10]
    if (completion_time != -1) {
        os << " | Complete: " << completion_time;

        // Total time spent in the system (queue + processing). [10]
        os << " | Total System Time: " << (completion_time - arrival_time);

        // Time spent waiting before processing began. [10]
        os << " | Wait Time: " << (start_time - arrival_time);
    }

    os << std::endl;
}