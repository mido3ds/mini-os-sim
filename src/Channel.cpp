#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "Channel.hpp"

#define MAX_TRIALS 300
#define MAX_CHARS 64
#define PERMISSIONS 0666

static int createMQ() {
    int msgid;
    int trials = 0;

    while ((msgid = msgget(rand(), PERMISSIONS | IPC_EXCL | IPC_CREAT)) == -1) {
        if (++trials >= MAX_TRIALS) {
            perror("failed tried to create msgq more than MAX_TRIALS");
            exit(1);
        }    
    }

    return msgid;
}

Channel::Channel() :upID(createMQ()), downID(createMQ()), isopen(true) { }

struct Buffer {
    long type;
    char text[MAX_CHARS];
};

bool Channel::send(string msg, long type) {
    if (msg.size() >= MAX_CHARS) return false;

    Buffer buf;
    buf.type = type;
    strcpy(buf.text, msg.c_str());

    return msgsnd(upID, &buf, sizeof buf, 0) != -1;
}

bool Channel::recv(string& msg, long& type) {
    Buffer buf;

    if (msgrcv(downID, &buf, sizeof buf, 1, 0) != -1) {
        msg.assign(buf.text);
        type = buf.type;
        return true;
    } 

    return false;
}

bool Channel::close() {
    if (isopen) {
        isopen = false;

        return (msgctl(upID, IPC_RMID, NULL) != -1) && (msgctl(downID, IPC_RMID, NULL) != -1);
    }

    return false;
}

Channel Channel::flip() {
    std::swap(upID, downID);
    return *this;
}