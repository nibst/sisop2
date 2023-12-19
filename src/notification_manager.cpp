#include "notification_manager.hpp"
#include "notification.hpp"
#include "profile_manager.hpp"
#include "constants.hpp"
#include "packet.hpp"
#include <algorithm>
#include <iostream>
#include <string.h>
#include <chrono>

pthread_mutex_t mutex;
RW_Monitor NotificationManager::notificationsMonitor;

NotificationManager::NotificationManager(){
    pthread_mutex_init(&mutex, NULL);

}

Notification *NotificationManager::composeMessage(std::string senderName, std::string content, int readersPending, int type){
    // Calculate total size of the message record struct
    int record_size = sizeof(Notification); //+ (content.length() + 1);

    // Create a record for the message
    Notification* msg = (Notification*)malloc(record_size); // Malloc memory
    bzero((void*)msg, record_size);                             // Initialize bytes to zero
    strcpy(msg->sender,senderName.c_str());                  // Copy sender name
    msg->timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // Current timestamp
    msg->type = type;                                   // Update message type
    msg->length = content.length() + 1;                 // Update message length
    msg->pending = readersPending;
    msg->id = 0;//TODO implement id
    strcpy((char*)msg->_string, content.c_str());      // Copy message
    //Return a pointer to it
    return msg;

}

void NotificationManager::notificationReceived(Notification *notification, std::vector<std::string> followers)
{
    Notification newNotification,lastNotification;
    PendingNotification newPending;
    std::string profilename(notification->sender);
    
    newNotification = *notification; //

    pthread_mutex_lock(&mutex);
        
    //Gerando id de Notificação Caso não tenha notificações id=0 caso contrario id = max(id)+1    
    if(!this->notificationsHistory[profilename].size())
        newNotification.id = 0;   
    else{
        lastNotification = this->notificationsHistory[profilename].back();
        newNotification.id = lastNotification.id + 1;    
    }
    newNotification.pending = followers.size();
    
    newPending.id = newNotification.id;
    newPending.sender = profilename;

    //Adiciona a nova notificação ao fim da lista de notificaçãoes do usuário que gerou (sent notifications)
    this->notificationsHistory[profilename].push_back(newNotification); 

    //Percorre todos os seguidores do usuário e adiciona uma nova notificação pendente ao fim da lista de notificações pendentes deles
    for(std::string follower : followers) {
        //Adiciona a pending notification à lista
        this->pendingNotifications[follower].push(newPending);
    }
    //depois basicamente pegar todas pendingNotifications e mandar para o maximo de sessoes possivel daquele follower, se só tiver uma sessao mandar naquela
    //se nao tiver nenhuma sessao esperar uma sessao abrir

    pthread_mutex_unlock(&mutex);
}
bool NotificationManager::needsToSend(std::string profilename, SessionStateMap sessions){
    //if there is pending notifications and there is a sessions available, then it needs to send
    return (this->pendingNotifications[profilename].size() > 0 && sessions[profilename].size() > 0);
}
Notification* NotificationManager::consumePending(std::string profilename)
{
    PendingNotification pendingNotification;
    pthread_mutex_lock(&mutex);
    pendingNotification = this->pendingNotifications[profilename].front();
    //consume
    this->pendingNotifications[profilename].pop();
    //percorre as notificações da lista de notificações do usuário (pendingNotification.sender) que gerou a pending notification
    for (Notification notification : this->notificationsHistory[pendingNotification.sender]){
        //encontra a notificação com id indicado na pending notification
        if(notification.id == pendingNotification.id){
            // Create a record for the message
            Notification* msg = (Notification*)malloc(sizeof(notification)); // Malloc memory
            *msg = notification; 
            //Return a pointer to it
            pthread_mutex_unlock(&mutex);
            return msg;
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
