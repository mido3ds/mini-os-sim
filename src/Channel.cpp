#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "Channel.hpp"

#define MAX_CHARS 64
#define PERMISSIONS 0666

static int createMQ() {
    int msgid;
    while ((msgid = msgget(rand(), PERMISSIONS | IPC_EXCL | IPC_CREAT)) == -1) { }
    return msgid;
}

Channel::Channel() {}

Channel Channel::open() {
    Channel chnl;

    chnl.upID = createMQ();
    chnl.downID = createMQ();
    chnl.isopen = true;

    return chnl;
}

struct Buffer {
    long type;
    char text[MAX_CHARS];
};

bool Channel::send(string msg, long type) {
    if (!isopen) return false;
    if (msg.size() >= MAX_CHARS) return false;

    Buffer buf;
    buf.type = type;
    strcpy(buf.text, msg.c_str());

    return msgsnd(upID, &buf, sizeof buf, IPC_NOWAIT) != -1;
}

bool Channel::recv(string& msg, long type) {
    if (!isopen) return false;

    Buffer buf;

    if (msgrcv(downID, &buf, sizeof buf, type, ~IPC_NOWAIT) != -1) {
        msg = string(buf.text);
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