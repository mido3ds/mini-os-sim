#pragma once
#include <string>

using std::string;

class Channel {
private:
    int upID, downID;
    bool isopen;

public:
    Channel();

    /*
     * send message `msg` with mtype `type`
     * 
     * @return: true if success, otherwise false
     */
    bool send(string msg, long type);

    /*
     * receive into `msg` and mtype into `type`
     * 
     * @return: true if success, otherwise false
     */
    bool recv(string& msg, long& type);

    /*
     * close both up/down streams
     * 
     * @return: true if success, otherwise false
     */
    bool close();

    /*
     * up stream is down stream and vice versa
     */
    Channel flip();
};