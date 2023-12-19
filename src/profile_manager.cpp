#include "profile_manager.hpp"
#include "constants.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "communication_manager.hpp"
#include "notification_manager.hpp"

RW_Monitor ProfileManager::profiles_monitor;


ProfileManager::ProfileManager(){
    this->setProfiles(this->getAllFollowersFromFile());
}
bool ProfileManager::profileExists(std::string profilename){
    //count the number of keys with the same value as profilename
    if (this->profiles.count(profilename) > 0)
        return true;
    return false;
}
void ProfileManager::follow(std::string user, std::string followedUser)
{
    //mtx.lock();
    ProfileManager profileManager;
    
    if (user != followedUser) {   
        if (profileExists(followedUser) && profileExists(user)){
            std::vector<std::string> followersOfFollowedUser = this->profiles[followedUser].getFollowers();
            bool alreadyFollows = std::find(
                followersOfFollowedUser.begin(),
                    followersOfFollowedUser.end(),
                        user) != followersOfFollowedUser.end();
            if (!alreadyFollows)
                this->profiles[followedUser].addFollower(user);
            else 
                std::cout << user << " already follows " << followedUser << "." << std::endl;
        }
    }
    else{
        std::cout << user << " attempted to follow himself." << std::endl;
    }
    //mtx.unlock();
}
void ProfileManager::printProfiles(){
    for (ProfileMap::iterator itr = this->profiles.begin(); itr != this->profiles.end(); ++itr) {
        std::cout << itr->first << ": ";
        for(std::string follower : itr->second.getFollowers())
            std::cout << follower << " ";
        std::cout << "\n";
    }        
}
void ProfileManager::setProfiles(ProfileMap profiles){
    this->profiles = profiles;
}
ProfileMap ProfileManager::getAllFollowersFromFile(){
    /*format
    profile: follower1 follower2
    profile2: follower2 follower1 follower3
    .
    */
    std::string profilename,follower,followers;
    std::ifstream file;
    ProfileMap loadedProfiles;
    file.open(DEFAULT_PROFILES_FILE);
    while (file.good())
    {
        std::getline(file, profilename, ':');
        Profile profile = Profile(profilename);
        char c;
        file.get(c);
        if(c == ' ')
        {
            std::getline(file, followers);
            std::istringstream iss(followers);
            do {
                profile.addFollower(follower);
            }while(std::getline(iss, follower, ' '));
        }
        loadedProfiles[profilename] = profile; 
    }
    return loadedProfiles;
}
Profile ProfileManager::getProfile(std::string profileName){
    /*this return a copy of the profile
      if the profile doesnt exists it will return a new profile with the default name
      we can later on disregard this profile with default name
    */
    return this->profiles[profileName];
}
void ProfileManager::saveAll(){
    /*format
    profile: follower1 follower2
    profile2: follower2 follower1 follower3
    .
    */
    std::ofstream file;
    file.open(DEFAULT_PROFILES_FILE);
    if (file){
        for (ProfileMap::iterator itr = this->profiles.begin(); itr != this->profiles.end(); ++itr) {
            file << itr->first << ": ";
            for(std::string follower : itr->second.getFollowers())
                file << follower << " ";
            file << "\n";
        }        
    }
}
void ProfileManager::addProfile(Profile profile){
    std::string name = profile.getName();
    this->profiles[name] = profile;
}
// returns the sessionID or -1 if the connection failed
int ProfileManager::addSession(std::string username, struct sockaddr_in address)
{
    // check if exist on map
    SessionStateMap::iterator it = this->sessionStates.find(username);
    bool found = it != this->sessionStates.end();

    
    // if not, add it with 1
    if (!found)
    {
        this->sessionStates[username].push_back(1);
        this->sessionAddresses[username + "#1"] = address;
        return 1;
    }

    
    auto current = it->second;

    // if already on max sessions, return false
    if (current.size() >= MAX_SESSIONS)
    {
        std::cout << "limite excedido " << current.size() << std::endl;
        return -1;
    }

    
    //  increment the current value and return true
    if(current.front() == 1)
    { 
        it->second.push_back(2);
        this->sessionAddresses[username + "#2"] = address;
    }
    else 
    {
        it->second.push_back(1);
        this->sessionAddresses[username + "#1"] = address;
    }

    for (auto &session : this->sessionStates)
    {
        std::cout << "username: " << session.first << std::endl;
        for (auto &sessionId : session.second)
        {
            std::cout << "sessionId: " << sessionId << std::endl;
        }
    }

    for (auto &sessionAddr : sessionAddresses)
    {
        std::cout << "username: " << sessionAddr.first << " port: " << 
        sessionAddr.second.sin_port << " ip: " << 
        sessionAddr.second.sin_addr.s_addr << std::endl;
    }

    return it->second.back();
}
int ProfileManager::deleteSession(std::string username, int sessionNumber){

    std::cout << username  << "-" << std::endl;
    // check if exist on map
    SessionStateMap::iterator it = this->sessionStates.find(username);
    bool found = it != this->sessionStates.end();
    
    // if not return
    if (!found){
        return 1;
    }
    if (it->second.size() == 1){
        this->sessionStates.erase(username);
        
        sessionAddresses.erase(username+"#"+std::to_string(sessionNumber));
        
        return 1;
    }

    for (auto itVec = this->sessionStates[username].begin(); itVec != this->sessionStates[username].end(); ++itVec){
        if(*itVec == sessionNumber)
        {
            this->sessionStates[username].erase(itVec);
            break;
        }
    }

    for (auto itMap = this->sessionAddresses.begin() ; itMap != this->sessionAddresses.end(); ++itMap){
        std::cout << (*itMap).first  << std::endl;
        if((*itMap).first == username+"#"+std::to_string(sessionNumber))
        {
            this->sessionAddresses.erase(itMap);
            break;
        }
    }
    return 1;
}

void ProfileManager::sayToFollowers(int socket, std::string senderName, std::string message_content, int packetType){
    ProfileManager::profiles_monitor.requestRead();
    std::vector<std::string> followers = this->profiles[senderName].getFollowers();
    Notification *notification = NotificationManager::composeMessage(senderName,message_content,followers.size(), USER_MESSAGE);
    int notificationSize = sizeof(*notification) + notification->length;
    for (std::string follower: followers){
        for(int state : this->sessionStates[follower]){
            std::string session = follower + "#" + std::to_string(state);
            struct sockaddr_in addr = this->sessionAddresses[session];
            CommunicationManager::sendPacket(socket, packetType, (char*)notification, notificationSize,addr );
        }
    }
    ProfileManager::profiles_monitor.releaseRead();
}

SessionAddressMap ProfileManager::getSessionsAddr(){
    return this->sessionAddresses;
}
SessionStateMap ProfileManager::getSessionsState(){
    return this->sessionStates;
}
