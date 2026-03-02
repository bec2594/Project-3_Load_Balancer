#include "Request.h"
#include <string>    // Explicitly included
#include <utility>   // For std::move
#include <iostream>  // For std::endl in print method

Request::Request(int request_id, std::string ip_in, std::string ip_out,
                 int process_time, char type, int arrival_time_param)
    : id(request_id), // Initializes 'id' member with 'request_id' parameter
      IP_in(std::move(ip_in)),
      IP_out(std::move(ip_out)),
      processing_time(process_time),
      job_type(type),
      arrival_time(arrival_time_param),
      start_time(-1),
      completion_time(-1)
{}

void Request::print(std::ostream& os) const {
    os << "Req ID: " << id
       << " | IP In: " << IP_in
       << " | IP Out: " << IP_out
       << " | Proc Time: " << processing_time
       << " | Type: " << job_type
       << " | Arrival: " << arrival_time;
    if (start_time != -1) {
        os << " | Start: " << start_time;
    }
    if (completion_time != -1) {
        os << " | Complete: " << completion_time;
        // Calculate and display total time spent in system
        os << " | Total System Time: " << (completion_time - arrival_time);
        // Calculate and display wait time (queue time)
        os << " | Wait Time: " << (start_time - arrival_time);
    }
    os << std::endl; // Ensure a newline after each request's details
}