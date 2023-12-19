
#pragma once
#include "packet.hpp"
#include "notification.hpp"
#include "profile.hpp"
#include "profile_manager.hpp"
#include "RW_monitor.hpp"
#include <string>
#include <map>
#include <queue>

typedef struct pendingNotification
{
    std::string sender;
    int id;             // id of the notification, used to search in the notificationHistory
}PendingNotification;

typedef std::map<std::string,std::vector<Notification>> NotificationsMap;
typedef std::map<std::string,std::queue<PendingNotification>> PendingNotificationsMap;
//basically mailbox of one profile 
class NotificationManager{
public:
    NotificationManager();
    static Notification* composeMessage(std::string senderName, std::string content, int readersPending,  int type);
    void notificationReceived(Notification *notification, std::vector<std::string> followers);
    bool needsToSend(std::string profilename, SessionStateMap sessions);
    Notification* consumePending(std::string profilename);

    static RW_Monitor notificationsMonitor;

private:
    std::string server_ip;                  // IP of the remote server
    int         server_port;                // Port of the remote server
    static int  server_socket;              // Socket for remote server communication
    struct sockaddr_in server_address;      // Server socket address
    NotificationsMap notificationsHistory;   // notifications that were sent BY the profilename
    PendingNotificationsMap pendingNotifications; //map username to a FIFO of pending notification, notifications that were sent TO profilename

};