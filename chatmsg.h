#ifndef CHATMSG_H
#define CHATMSG_H

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
    size_t msgSize; /**< The size of this message. */
    MsgType msgType;/**< The type of this message. */
    char* userName; /**< Name of user who sent this message. */
    char* msgData;  /**< The data of this message. */
};

#endif // CHATMSG_H
