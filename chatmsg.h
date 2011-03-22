/**
  @file

  Important. Sends and receives data in a specific order.
  <ol>
    <li>size</li>
    <li>nameSize</li>
    <li>IP Address</li>
    <li>username</li>
    <li>type</li>
    <li>data</li>
  </ol>

  */

#ifndef CHATMSG_H
#define CHATMSG_H

#ifndef _WIN32
#include <arpa/inet.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <sys/types.h>

#include <QString>

/**
  The various possible message types.
  @author Nick Huber
  */
enum MsgType {
    kChat,     /**< Textual chat msg. */
};

/**
  Structure about each message going across the alabtross protocol.
  @author Nick Huber
  @author Dean Morin
  */
struct ChatMsg {
    int size;             /**< The size of this message's data. */
    int nameSize;         /**< The size of this user's username.*/
    in_addr address;      /**< The sending client's IP address. */
    const char* username; /**< The user's username. */
    MsgType type;         /**< The type of this message. */
    const char* data;     /**< The data of this message. */
};

/**
  Possible return values for the readMsg function.
  @author Nick Huber
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
  @author Nick Huber
  @author Dean Morin
  */
return_readMsg readMsg(int socket, ChatMsg& chatMsg);

/**
  Attempts to send a message over the socket as specified by the ChatMsg.
  @param socket The socket to send over.
  @param chatMsg The data to send.
  @return Whether or not the data was able to be sent.
  @author Nick Huber
  */
bool sendMsg(int socket, const ChatMsg& chatMsg);

#endif // CHATMSG_H
