/**
 * @file WebServer.h
 * @brief Declares the WebServer class, which processes Request objects over discrete simulation cycles. [17]
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"

/**
 * @class WebServer
 * @brief Represents a single server that can process at most one Request at a time. [17]
 *
 * @details
 * A WebServer tracks the currently assigned request (if any), the remaining processing cycles,
 * and whether it is busy. It provides methods to accept a request, advance time, query availability,
 * and return a completed request. [17]
 */
class WebServer {
public:
    /** @brief Unique identifier for this WebServer instance. [17] */
    int id;

    /** @brief Pointer to the currently assigned Request, or nullptr if idle. [17] */
    Request* current_request;

    /** @brief Remaining processing cycles for the current request (valid when busy). [17] */
    int remaining_cycles;

    /** @brief True if the server is currently processing a request; false otherwise. [17] */
    bool busy;

    /**
     * @brief Constructs a WebServer with a given unique id. [17]
     * @param server_id Unique id assigned to the server.
     */
    WebServer(int server_id);

    /**
     * @brief Assigns a request to this webserver if it is not busy. [17]
     *
     * @param request Pointer to the Request to process.
     * @return true if the request was accepted; false if the server is busy or request is nullptr. [17]
     */
    bool take_request(Request* request);

    /**
     * @brief Advances processing by one simulation tick, decrementing remaining_cycles when busy. [17]
     */
    void process_time();

    /**
     * @brief Checks whether the webserver is available to take a new request. [17]
     * @return true if available (idle), false if busy.
     */
    bool is_available() const;

    /**
     * @brief Returns the id of the currently processed request. [17]
     * @return Request id, or -1 if no request is being processed.
     */
    int curr_request_id() const;

    /**
     * @brief Returns the completed request (if processing finished) and clears the server state. [17]
     *
     * @return Pointer to the completed Request if finished; nullptr otherwise. [17]
     */
    Request* processed_request();
};

#endif // WEBSERVER_H