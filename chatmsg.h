#ifndef CHATMSG_H
#define CHATMSG_H

#include <sys/types.h>
#include <QString>

/**
  The various possible message types.
  @author Nick Huber
  */
enum MsgType {
    kChat /**< Msg is a textual chat msg. */
};

/**
  Structure about each message going across the alabtross protocol.
  @author Nick Huber
  */
struct ChatMsg {
    int size;             /**< The size of this message. */
    MsgType type;         /**< The type of this message. */
    const char* username; /**< Name of user who sent this message. */
    const char* data;     /**< The data of this message. */
};

/**
  Possible return values for the readMsg function.
  */
enum return_readMsg {
    kSuccess,    /**< Message was successfully read. */
    kDisconnect, /**< Message was actually a disconnect. */
    kError       /**< Message failed and resulted in an error. */
};

/**
  Attemps to read a message from the specified socket.
  @param socket The socket to read from.
  @param chatMsg The chatMsg to store the results into.
  @return The result of the read operation as defined in return_readMsg.
  @author Nick
  */
return_readMsg readMsg(int socket, ChatMsg& chatMsg);

/**
  Attempts to send a message over the socket as specified by the ChatMsg.
  @param socket The socket to send over.
  @param chatMsg The data to send.
  @return Whether or not the data was able to be sent.
  */
bool sendMsg(int socket, const ChatMsg& chatMsg);

#endif // CHATMSG_H
