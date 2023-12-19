
#pragma once
#include <cstdint>

// Packet types
#define PAK_DATA           1 // Message packet
#define PAK_COMMAND        2 // Command packet
#define PAK_SERVER_MESSAGE 3 // Server message packet
#define PACKET_MAX   2048    // Maximum size (in bytes) for a packet
#define MAX_PAYLOAD_SIZE 320
//basicaly is the header + payload -> payload may be a Notification
typedef struct __packet{
    uint16_t type; //Tipo do pacote (p.ex. DATA | CMD)
    uint16_t seqn; //Número de sequência
    uint16_t length; //Comprimento do payload
    uint64_t timestamp; // Timestamp do dado
    const char _payload[MAX_PAYLOAD_SIZE]; //Dados da mensagem
}Packet;