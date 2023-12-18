#include "profile.hpp"
#include "notification_manager.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "communication_manager.hpp"

Profile::Profile():name(DEFAULT_PROFILE_NAME){;}
Profile::Profile(std::string name): name(name) {;}

std::string Profile::getName() const{
    return this->name;
}
void Profile::addFollower(std::string follower){
    this->followers.push_back(follower);
}
 std::vector<std::string> Profile::getFollowers(){
    //this returns a pointer to followers (can cause problems, but whatever)
    return this->followers;
}
void Profile::addNotification(Notification notification){
    //this->notifications_history.push_back(notification);
}
void Profile::loadFollowersFromFile(){
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
    this->followers = loadedProfiles[this->name].getFollowers();
}