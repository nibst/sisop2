#pragma once
#include <cstdint>


// notification types
#define SERVER_MESSAGE 1 // Indicates a message sent by server 
#define USER_MESSAGE   2 // Indicates a message sent by a user
#define LOGIN_MESSAGE  3 // Login message, indicates start of conection with server
#define FOLLOW_MESSAGE 4 
#define LOGOUT_MESSAGE 5 // Logout message
#define LIST_PROFILES_MESSAGE 6
#define DISCONNECT_CLIENT_MESSAGE 7

#define MESSAGE_MAX  256 // Max _string size
typedef struct __notification{
    char sender[23];      // User who sent this message
    uint32_t id;          //Identificador da notificação (sugere-se um identificador único)
    uint64_t timestamp;   //Timestamp da notificação
    uint16_t length;      //Tamanho da mensagem
    uint16_t pending;     //Quantidade de leitores pendentes
    uint16_t type;        //Tipo do pacote 
    char _string[MESSAGE_MAX];  //Mensagem
    
} Notification;