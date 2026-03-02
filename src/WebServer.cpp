/**
 * @file WebServer.cpp
 * @brief Implements the WebServer class, which processes Requests over discrete simulation cycles. [16]
 */

#include "WebServer.h"

/**
 * @brief Constructs a WebServer with a unique identifier and no active request. [16]
 *
 * @param server_id Unique id assigned to this server instance. [17]
 */
WebServer::WebServer(int server_id) :
    id(server_id),
    current_request(nullptr),
    remaining_cycles(0),
    busy(false)
{}

/**
 * @brief Attempts to assign a Request to this server for processing. [16]
 *
 * @details
 * If the server is not busy and @p request is non-null, the server takes ownership of the
 * request pointer, initializes its remaining cycle count from Request::processing_time,
 * and marks itself busy. [16]
 *
 * @param request Pointer to a Request to be processed.
 * @return true if the request was accepted; false if the server was busy or @p request was null. [17]
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

/**
 * @brief Advances the server by one simulation tick. [16]
 *
 * @details
 * If the server is busy, decrements remaining_cycles by one. Completion handling is performed
 * elsewhere by checking remaining_cycles and calling processed_request(). [16]
 */
void WebServer::process_time() {
    if (busy) {
        remaining_cycles--;
    }
}

/**
 * @brief Indicates whether this server is available to take a new request. [16]
 *
 * @return true if no request is currently assigned (current_request == nullptr); otherwise false. [16]
 */
bool WebServer::is_available() const {
    return current_request == nullptr;
}

/**
 * @brief Returns the id of the currently processed request, if any. [16]
 *
 * @return Request id if current_request is non-null; otherwise -1. [16]
 */
int WebServer::curr_request_id() const {
    if (current_request != nullptr) {
        return current_request->id;
    }
    return -1;
}

/**
 * @brief Returns the completed request (if finished) and resets the server to idle. [16]
 *
 * @details
 * If a request exists and remaining_cycles is <= 0, the request is considered complete.
 * The server is reset (current_request cleared, remaining_cycles set to 0, busy set to false)
 * and the completed request pointer is returned to the caller. Otherwise returns nullptr. [16]
 *
 * @return Pointer to the completed Request if one finished; nullptr otherwise. [17]
 */
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