#include "notification_manager.hpp"
#include "notification.hpp"
#include "profile_manager.hpp"
#include "constants.hpp"
#include <algorithm>
#include <iostream>
#include <string.h>
#include <chrono>

NotificationManager::NotificationManager(){;}


Notification* NotificationManager::composeMessage(std::string senderName, std::string content, int readersPending, int type){
    // Calculate total size of the message record struct
    int record_size = sizeof(Notification) + (content.length() + 1);

    // Create a record for the message
    Notification* msg = (Notification*)malloc(record_size); // Malloc memory
    bzero((void*)msg, record_size);                             // Initialize bytes to zero
    strcpy(msg->sender,senderName.c_str());                  // Copy sender name
    msg->timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // Current timestamp
    msg->type = type;                                   // Update message type
    msg->length = content.length() + 1;                 // Update message length
    msg->pending = readersPending;
    msg->id = 0;//TODO implement id
    strcpy((char*)msg->_string, content.c_str());      // Copy message
    //Return a pointer to it
    return msg;

}