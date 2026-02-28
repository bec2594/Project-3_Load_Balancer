#include "Request.h"


void Request::print(std::ostream& output) const {
    output << "Req ID: " << id
        << " | IP In: " << IP_in
        << " | IP Out: " << IP_out
        << " | Proc Time: " << processing_time
        << " | Type: " << job_type
        << " | Arrival: " << arrival_time;

    if (start_time != -1) {
        output << " | Start: " << start_time;
    }
    if (completion_time != -1) {
        output << " | Complete: " << completion_time;
        // Optionally calculate and display total time spent in system
        output << " | Total System Time: " << (completion_time - arrival_time);
        // Optionally calculate and display wait time (queue time)
        output << " | Wait Time: " << (start_time - arrival_time);
    }
    output << std::endl; // Ensure a newline after each request's details
}