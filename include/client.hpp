#pragma once
#include <string>
#include "profile.hpp"
#include "RW_monitor.hpp"
#include <map>
#define LOCALHOST "127.0.0.1"
class UDPClient{
public:
    UDPClient(std::string profilename,std::string server_ip);
    UDPClient(std::string profilename,std::string server_ip, std::string server_port);
    //Setup connection to remote server
    void setupConnection();
    //Poll server for any new messages, showing them to the user
    void getMessages();
    static void *handleUserInputWrapper(void* arg); //we made a wrapper because method called by thread needs to be static


private:
    void *_handleUserInput();
    static std::atomic<bool> stop_issued;   // Atomic flag for stopping all threads
    std::string server_ip;                  // IP of the remote server
    int         server_port;                // Port of the remote server
    static int  server_socket;              // Socket for remote server communication
    struct sockaddr_in server_address;      // Server socket address
    static pthread_t input_handler_thread;  // Thread to listen for new incoming server messages
    static RW_Monitor socket_monitor;       // Monitor that controls the sending of data through the socke
    static Profile profile;                 // Client profile
    static std::map<std::string, int> commands; // Possible commands

};
