
#pragma once
#include "packet.hpp"
#include "notification.hpp"
#include "profile.hpp"
#include "profile_manager.hpp"
#include <string>
#include <map>


//typedef std::map<std::string,std::vector<Notification>> NotificationsMap;
//basically mailbox of one profile 
class NotificationManager{
public:
    NotificationManager();
    static Notification* composeMessage(std::string senderName, std::string content, int readersPending,  int type);
    void processClientPacket(char* client_message);
    void tweetReceived(std::string user, std::string msg,int timestamp);
    bool needsToSend(std::string username,int session);
    Packet consumeTweet(std::string username,int session);
private:
    static std::string profilename; 
    std::string server_ip;                  // IP of the remote server
    int         server_port;                // Port of the remote server
    static int  server_socket;              // Socket for remote server communication
    struct sockaddr_in server_address;      // Server socket address
    ProfileManager profile_manager;

};