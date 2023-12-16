#include "packet.hpp"
class CommunicationManager{
    public:
        Packet receivePacket();
        void sendPacket(Packet packet);
};