#include "notification_manager.hpp"
#include "notification.hpp"
#include "profile_manager.hpp"
#include <algorithm>
#include <iostream>
using namespace std;
NotificationManager::NotificationManager(){}
void NotificationManager::tweetReceived(string user, string msg,int timestamp)
{
    Notification newNotification,lastNotification;
    PendingNotification newPending;
    SessionMap::iterator itSes;
    bool found;


    newNotification.timestamp = timestamp;
    newNotification.message = msg; 

    mtx.lock();
    newNotification.remainingFollowers = this->users[user].followersList.size();
        
    //Gerando id de Notificação Caso não tenha notificações id=0 caso contrario id = max(id)+1    
    if(!this->users[user].notificationList.size())
    {
        newNotification.id = 0;   
    }
    else
    {
        lastNotification = this->users[user].notificationList.back();
        newNotification.id = lastNotification.id + 1;    
    }


    newPending.id = newNotification.id;
    newPending.sender = user;


    //Adiciona a nova notificação ao fiz da lista de notificaçãoes do usuário que gerou
    this->users[user].notificationList.push_back(newNotification); 

    //Percorre todos os seguidores do usuário e adiciona uma nova notificação pendente ao fim da lista de notificações pendentes deles
    for(auto itVec : this->users[user].followersList)
    {
        newPending.sessions.clear();
        create_user_if_not_found(itVec);
        itSes = this->sessionsQty.find(itVec);
        found = itSes != this->sessionsQty.end();

        //Verifica se existem sessões do seguidor
        if(found)
        {
            //Caso existam, adiciona a lista de sessões à pendingNotification
            newPending.sessions = this->sessionsQty[itVec];
        }
        else
        {
            //Caso contrário, coloca a sessão 1 (primeira que o user receberá ao fazer login)
            newPending.sessions.push_back(1);
        }

        //Adiciona a pending notification à lista
        this->users[itVec].pendingList.push_back(newPending);

    }
    mtx.unlock();
}

bool NotificationManager::needsToSend(string username, int session)
{
    bool need = false;
    PendingNotification pendingNot;
    
    mtx.lock();

    //Verifica se a pending list do usuário possui notificações pendentes
    if(this->users[username].pendingList.size() > 0){
        //percorre todas as notificações
        for(auto pendingNot : this->users[username].pendingList){
            for (auto itSes = pendingNot.sessions.begin(); itSes != pendingNot.sessions.end(); ++itSes){
                //Caso ache uma notificação que inclua a sessão atual entre as sessões pendentes, indica que precisa enviar
                if(*itSes == session)
                {
                    need = true;
                    break;
                }
            }   
        }
    }
    mtx.unlock();
    return need;


}

Packet NotificationManager::consumeTweet(string username,int session)
{
    PendingNotification foundNot;
    packet notificationPkt;
    bool shouldErase = false;
    bool shouldBreak = false;
    vector<PendingNotification>::iterator itPending;
    mtx.lock();
    //percorre a pending list do usuário que deseja consumir um tweet
    for(auto pendingNot = this->users[username].pendingList.begin(); pendingNot != this->users[username].pendingList.end(); ++pendingNot ){
        //para cada pending notification, verifica se possui o id de sessão igual ao da sessão atual
        for (auto itSes = (*pendingNot).sessions.begin(); itSes != (*pendingNot).sessions.end(); ++itSes){
            if(*itSes == session)
            {   
                // ao encontrar a notificação desejada a seleciona e verifica se a pending notification deve ser excluída             
                foundNot = (*pendingNot);
                this->users[username].pendingList[distance(this->users[username].pendingList.begin(),pendingNot)].sessions.erase(itSes);
                if(this->users[username].pendingList[distance(this->users[username].pendingList.begin(),pendingNot)].sessions.size() == 0)
                {
                
                    itPending = pendingNot;
                    shouldErase = true;
                }
                
                shouldBreak = true;
                break;
            }
        }

        if (shouldBreak) 
        {
            break;
        }
    }

    //percorre as notificações da lista de notificações do usuário que gerou a pending notification
    for (auto itVet = this->users[foundNot.sender].notificationList.begin(); itVet != this->users[foundNot.sender].notificationList.end(); ++itVet)
    {
        //encontra a notificação com id indicado na pending notification
        if((*itVet).id == foundNot.id)
        {   
            //gera o pacote que será enviado
            strcpy(notificationPkt._payload, (*itVet).message.c_str());
            notificationPkt.type = TIPO_NOTI;
            notificationPkt.timestamp = (*itVet).timestamp;
            strcpy(notificationPkt.user, foundNot.sender.c_str());
            notificationPkt.length = strlen(notificationPkt._payload);

            if(shouldErase)
            {
                this->users[username].pendingList.erase(itPending);
                (*itVet).remainingFollowers -=1;
                if((*itVet).remainingFollowers == 0)
                {
                    this->users[foundNot.sender].notificationList.erase(itVet);
                }
            }
            
            break;
        } 
    }
    mtx.unlock();
    return notificationPkt;
}