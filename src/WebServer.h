#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"


class WebServer {
public:
    int id;
    Request* current_request;
    int remaining_cycles;
    bool busy;

    // Constructor
    // new server given unique id
    WebServer(int server_id);

    // take_request
    /**
     * assigns request to this webserver
     * request: pointer to request to be processed
     * if request taken return true, if busy then false
     */
    bool take_request(Request* request);

    // advance webserver process by a tick, lowers remaining cycles
    void process_time();

    // is available
    /**
     * check if webserver is vailable to take a new request
     * true if available, false if busy
    */
    bool is_available() const;

    // curr request id
    /**
    * ID of the currently processed request
    * return ID of the request or -1 if no request being processed
    */
    int curr_request_id() const;

    // processed request
    /**
    * get completed request once processing is done
    * clears servers current_request
    * returns pointer to completed request, nullptr if no request is finished
    */
    Request* processed_request();
};

#endif // WEBSERVER_H