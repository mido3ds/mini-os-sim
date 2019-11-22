#pragma once
#include <string>

using std::string;

class Channel {
private:
    int upID, downID;
    bool isopen = false;

    Channel();

public:

    /*
     * return new open Channel
     */
    static Channel open();

    /*
     * close both up/down streams
     * 
     * @return: true if success, otherwise false
     */
    bool close();

    /*
     * send message `msg` of `type`
     * type >0
     * 
     * @return: true if success, otherwise false
     */
    bool send(string msg, long type);

    /*
     * receive into `msg` from `type`
     * type >0 
     * if type == 0 -> recv the first message is read regardless of its type
     * 
     * @return: true if success, otherwise false
     */
    bool recv(string& msg, long type);

    /*
     * up stream is down stream and vice versa
     */
    Channel flip();
};