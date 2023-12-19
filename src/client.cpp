
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <curses.h>

#include "client.hpp"
#include "packet.hpp"
#include "notification_manager.hpp"
#include "communication_manager.hpp"
#include "client_interface.hpp"
#include "constants.hpp"
#include "notification.hpp"

//define static variables
std::atomic<bool> UDPClient::stop_issued;
int UDPClient::server_socket;
pthread_t UDPClient::input_handler_thread;
Profile UDPClient::profile;
RW_Monitor UDPClient::socket_monitor;
std::map<std::string, int> UDPClient::commands;

static Profile profile; 
UDPClient::UDPClient(std::string profilename,std::string server_ip):
    server_ip(server_ip), server_port(SERVER_PORT){
        Profile profile = Profile(profilename);
        profile.loadFollowersFromFile();
        this->profile = profile;

        commands["SEND"] = USER_MESSAGE;
        commands["FOLLOW"] = FOLLOW_MESSAGE;
        commands["LOGIN"] = LOGIN_MESSAGE;
        commands["LOGOUT"] = LOGOUT_MESSAGE;
        commands["LIST"] = LIST_PROFILES_MESSAGE;
        commands["DISC"] = DISCONNECT_CLIENT_MESSAGE;
}

UDPClient::UDPClient(std::string profilename,std::string server_ip, std::string server_port):
     server_ip(server_ip){
        Profile profile = Profile(profilename);
        profile.loadFollowersFromFile();
        this->profile = profile;
        this->server_port = atoi(server_port.c_str());


        commands["SEND"] = USER_MESSAGE;
        commands["FOLLOW"] = FOLLOW_MESSAGE;
        commands["LOGIN"] = LOGIN_MESSAGE;
        commands["LOGOUT"] = LOGOUT_MESSAGE;
        commands["LIST"] = LIST_PROFILES_MESSAGE;
        commands["DISC"] = DISCONNECT_CLIENT_MESSAGE;
     }


void UDPClient::setupConnection()
{
    Notification* login_record; // Record for sending login packet
  	struct hostent *server;
    server = gethostbyname(this->server_ip.c_str());
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    if ((this->server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        perror("ERROR opening socket");
    // Fill server socket address
    server_address.sin_family = AF_INET;
    server_address.sin_port   = htons(server_port);
    server_address.sin_addr = *((struct in_addr *)server->h_addr);

    // Prepare message record with login_record information
    login_record = NotificationManager::composeMessage(this->profile.getName(), std::string(this->profile.getName()), this->profile.getFollowers().size() , LOGIN_MESSAGE);

    // Sends the command packet to the server
    CommunicationManager::sendPacket(this->server_socket, PAK_COMMAND, (char*)login_record, sizeof(*login_record) + login_record->length, this->server_address); 

    // Free login_record 
    free(login_record);

    // Start user input getter thread
    pthread_create(&input_handler_thread, NULL, handleUserInputWrapper, this);

};

void UDPClient::getMessages()
{
    int read_bytes = -1;                // Number of bytes read from the header
    char server_message[PACKET_MAX];    // Buffer for message sent from server
    Notification* received_message;   // Pointer to a message record, used to decode received packet payload
    Packet* received_packet;

    char message_time[9];     // Timestamp of the message
    std::string chat_message; // Final composed chat message string, printed to the interface
    std::string username;     // Name of the user who sent the message

    // Clear buffer to receive new packets
    bzero((void*)server_message, PACKET_MAX);

    // Wait for messages from the server
    struct sockaddr_in received_addr;
    while(!stop_issued )
    {

        read_bytes = CommunicationManager::receivePacket(this->server_socket, server_message, &received_addr);
        
        // Decode message into packet format
        received_packet = (Packet*)server_message;


        // Try to read the rest of the payload from the socket stream
        switch(received_packet->type)
        {
            case PAK_DATA: // Data packet (messages)
                // Decode payload into a message record
                received_message = (Notification*)received_packet->_payload;       

                // Get time into a readable format
                strftime(message_time, sizeof(message_time), "%H:%M:%S", std::localtime((time_t*)&received_message->timestamp));

                if (received_message->type == SERVER_MESSAGE)
                {
                    chat_message = message_time + std::string(" ") + received_message->_string;
                }
                else if (received_message->type == USER_MESSAGE)
                {
                    chat_message = message_time + std::string(" ") + received_message->sender + ": " + received_message->_string;
                }

                // Display message
                std::cout << chat_message << std::endl;
                //ClientInterface::printMessage(chat_message);
                break;

            case PAK_SERVER_MESSAGE: // Server broadcast message for clients login/logout 
                
                // Decode payload into a message record
                received_message = (Notification*)received_packet->_payload;

                // Get time into a readable format
                strftime(message_time, sizeof(message_time), "%H:%M:%S", std::localtime((time_t*)&received_message->timestamp));

                // Display message
                chat_message = message_time + std::string(" ") + std::string(received_message->_string);
                std::cout << chat_message << std::endl;

                break;

            case PAK_COMMAND: // Command packet (disconnect)

                // Decode payload into a message record
                received_message = (Notification*)received_packet->_payload;

                // Show message to user
                std::cout << received_message->_string << std::endl;

                // Stop the client application
                stop_issued = true;
                read_bytes = 0;
                break;

            default: // Unknown packet
                std::cout << "Unknown packet" << std::endl;
                break;
        }

        // Clear buffer to receive new packets
        bzero((void*)server_message, PACKET_MAX);

    }
    // If server closes connection
    if (read_bytes == 0)
    {
        std::cout << "Connection closed by the server" << std::endl;
    }

    // Signal input handler to stop
    stop_issued = true;

    // Wait for input handler thread to finish
    pthread_join(UDPClient::input_handler_thread,NULL);

};
void *UDPClient::_handleUserInput(){

    Notification* message;

    // Get user messages to be sent until Ctrl D is pressed
    char buffer[MESSAGE_MAX + 1];
    do
    {   
        bzero(buffer, MESSAGE_MAX + 1);
        fgets(buffer, MESSAGE_MAX + 1, stdin);
        if (!stop_issued)
        {   
            std::string command;
            std::string user_message;
            std::istringstream iss(buffer);
            std::getline(iss, command, ' ');
            std::getline(iss,user_message);
            int notificationType = commands[command];
            try
            {
                if (user_message.length() > 0) 
                {
                    int packetType = PAK_COMMAND;
                    if (notificationType == USER_MESSAGE){
                        packetType = PAK_DATA;
                    }
                    // Compose message
                    message = NotificationManager::composeMessage(this->profile.getName(), user_message, this->profile.getFollowers().size(), notificationType);
                    
                    // Request write rights
                    socket_monitor.requestWrite();
                    // Send message to server
                    CommunicationManager::sendPacket(this->server_socket, packetType, (char*)message, sizeof(Notification) + message->length, this->server_address);

                    // Release write rights
                    this->socket_monitor.releaseWrite();

                    // Free composed message
                    free(message);
                }

            }
            catch(const std::runtime_error& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        bzero((void*)buffer, MESSAGE_MAX + 1);
    }
    while(!stop_issued);

    // Signal server-litening thread to stop
    this->stop_issued = true;

    // Close listening socket
    close(this->server_socket);

    // End with no return value
    pthread_exit(NULL);
}

void *UDPClient::handleUserInputWrapper(void* arg)
{
     reinterpret_cast<UDPClient*>(arg)->_handleUserInput();
}