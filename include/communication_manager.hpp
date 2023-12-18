#pragma once
#include "packet.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
class CommunicationManager{
    public:
        //*addr -> where the receiving struct sockaddr_in will be stored
        //*buffer -> where the receiving bytes will be stored
        static int receivePacket(int socket, char* buffer, struct sockaddr_in *addr);
        static int sendPacket(int socket, int packet_type, char* payload, int payload_size, struct sockaddr_in addr);
        static int receiveVariableSizePacket(int socket, char* buffer, struct sockaddr_in *addr);
};