#include "WebServer.h"

// Constructor
// No request made yet so current request, remaining cycles, busy all set to nothing and false
WebServer::WebServer(int server_id) : 
    id(server_id),
    current_request(nullptr),
    remaining_cycles(0),
    busy(false) {}

// server takes request if not busy, and request is good (not nullptr)
// update each variable in webserver
/**
 * current_request is inputted request
 * remaining cycles is inputted request's processing time
 * busy set to true
 */

bool WebServer::take_request(Request* request) {
    if(!busy && request != nullptr) {
        current_request = request;
        remaining_cycles = request->processing_time;
        busy = true;
        return true;
    }
    return false;
}

// decrement the remaining cycles for the web server's request
// handling the request completion (when cycles = 0) is done in the process_request and the load balancer processing time functions
void WebServer::process_time() {
    if (busy) {
        remaining_cycles--;
    }
}

// check if Server is available by checking if there is a request
// if available the request should be a nullptr (that is what it is initialized too as well)
bool WebServer::is_available() const {
    return current_request == nullptr;
}

// check the current request id if the request exists
// no request returns -1 (invalid and means no id)
int WebServer::curr_request_id() const {
    if (current_request != nullptr) {
        return current_request->id;
    }
    return -1;
}


// if the current request exists and the cycles goes to 0
// return the current request
// reset the servers variables (current_request, remaining cycles, busy)
// otherwise return nullptr
Request* WebServer::processed_request() {
    if (current_request != nullptr && remaining_cycles <= 0) {
        Request* completed_request = current_request;
        current_request = nullptr;
        remaining_cycles = 0;
        busy = false;
        return completed_request;
    }
    return nullptr;
}