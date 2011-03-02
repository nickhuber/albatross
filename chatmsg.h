#ifndef CHATMSG_H
#define CHATMSG_H
/**
  @file
  The ChatMsg structure, and the associated MsgType Enum

  msgType:
  CHAT - This message is designed to be sent to all members in the chat.
  */

/**
  Structure about each message going across the alabtross protocol.
  @author Nick Huber
  */
struct ChatMsg{
  size_t msgSize; /**< The size of this message. */
  MsgType msgType;/**< The type of this message. */
  char* userName; /**< Name of user who sent this message. */
  char* msgData;  /**< The data of this message. */
};

enum MsgType {
  CHAT
};

#endif // CHATMSG_H
