#pragma once

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <atomic>
#include "RW_monitor.hpp"
#include "constants.hpp"
#include "notification_manager.hpp"
#include "profile_manager.hpp"
enum Commands {
    SEND,
    FOLLOW,
    LOGIN,
    LOGOUT,
    LIST,
    //others...
};
class UDPServer {
private:

    static std::atomic<bool> stop_issued; // Atomic thread for stopping all threads
    static int server_socket;  // Socket the server listens at for new incoming messages

    struct sockaddr_in server_address;  // Server socket address
    struct sockaddr_in client_address;  // Client socket address
    pthread_t command_handler_thread; // Thread for handling server
    static std::map<std::string, pthread_t> connection_handler_threads; // Socket descriptor and threads for handling client connections
    static RW_Monitor threads_monitor;  // Monitor for connection handler threads list
    //static std::vector<std::string> activeClients; // active clients list
    static ProfileManager profileManager; //
    static NotificationManager notificationManager;
    static void *consumePendingNotifications(void* arg);

public:
    UDPServer();
    void run();
    ~UDPServer();
    static void *handleCommands(void* arg);
    static void stop();
    static void *handleConnection(void* arg);

};
