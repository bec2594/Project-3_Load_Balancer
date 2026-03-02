#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <ostream>

class Request {
    public:
        int id;
        std::string IP_in;
        std::string IP_out;
        int processing_time;
        char job_type;
        int arrival_time;
        int start_time;
        int completion_time;

        Request(int request_id, std::string ip_in, std::string ip_out, int process_time, char type, int arrival_time);
        // no destructor because there are no pointers and strings handle resource management

        // print the details of the request
        void print(std::ostream& output) const;
};
#endif // REQUEST_H