#ifndef CHATMSG_H
#define CHATMSG_H

#include <sys/types.h>

/**
  Enumeration of the various possible message types.
  @author Nick Huber
  */
enum MsgType {
    CHAT
};

/**
  Structure about each message going across the alabtross protocol.
  @author Nick Huber
  */
struct ChatMsg {
    size_t size; /**< The size of this message. */
    MsgType type;/**< The type of this message. */
    const char* username; /**< Name of user who sent this message. */
    const char* data;  /**< The data of this message. */
};

#endif // CHATMSG_H
