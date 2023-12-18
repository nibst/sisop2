#include "communication_manager.hpp"
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <chrono>


//this shit doest fuckingwork
int CommunicationManager::receiveVariableSizePacket(int socket, char* buffer, struct sockaddr_in *addr)
{
    int total_bytes   = 0; // Total number of bytes read
    int read_bytes    = 0; // Number of bytes in current read
    int payload_bytes = 0; // Bytes read from the payload
    int header_size  = sizeof(Packet); // Size of the packet header
    socklen_t addrlen = sizeof(struct sockaddr_in);
    // While the header hasn't arrived entirely and the socket was not closed
    while (read_bytes >= 0 && total_bytes < header_size && 
          (read_bytes = recvfrom(socket, buffer + total_bytes, header_size - total_bytes, 0, (struct sockaddr *)addr, &addrlen)) > 0)
    {        
        // If data has arrived, increase total
        if (read_bytes > 0)
            total_bytes += read_bytes;
        // If socket was closed, reset total
        else
            total_bytes = -1;
    }
    // If the entire header arrived
    if (total_bytes == header_size)
    {
        // Reset number of read bytes
        read_bytes = 0;

        // While the entire payload hasn't arrived
        while (read_bytes >= 0 && payload_bytes < ((Packet*)buffer)->length &&
        (read_bytes = recvfrom(socket, buffer + total_bytes, ((Packet*)buffer)->length - payload_bytes, 0, (struct sockaddr *)addr, &addrlen)) > 0)
        {
            // If data has arrived, increase totals
            if (read_bytes > 0)
            {
                payload_bytes += read_bytes;
                total_bytes   += read_bytes;
            }
            // If the socket was closed, reset total
            else
                read_bytes = -1;
        }
    }
    Packet* packet = (Packet*)buffer;
    // Return amount of bytes read/written
    return total_bytes;
}
int CommunicationManager::receivePacket(int socket, char* buffer, struct sockaddr_in *addr)
{
    int read_bytes    = 0; // Number of bytes in current read
    socklen_t addrlen = sizeof(struct sockaddr_in);

    read_bytes = recvfrom(socket, buffer, sizeof(Packet), 0, (struct sockaddr *)addr, &addrlen);

    // Return amount of bytes read/written
    return read_bytes;
}
int CommunicationManager::sendPacket(int socket, int packet_type, char* payload, int payload_size, struct sockaddr_in addr)
{
    // Calculate size of packet that will be sent
    int packet_size = sizeof(Packet) ;
    int bytes_sent = -1;   // Number of bytes actually sent to the client
    // TODO Sequence numbers!

    // Prepare packet
    Packet* data = (Packet*)malloc(packet_size);          // Malloc memory
    bzero((void*)data, packet_size);                             // Initialize bytes to zero
    data->type      = packet_type;                        // Signal that a data packet is being sent
    data->seqn       = 1;                                  // TODO Keep track of sequence numbers
    data->timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // Current timestamp
    data->length = payload_size;                          // Update payload size

    memcpy((char*)data->_payload, payload, payload_size); // Copy payload to packet
    // Send packet           
    bytes_sent = sendto(socket, data, packet_size, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if (bytes_sent < 0)
        std::cerr << "Unable to send message" << std::endl;

    // Free memory used for packet
    free(data);
    //free(payload);

    // Return number of bytes sent
    return bytes_sent;
}