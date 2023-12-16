#include "profile_manager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

ProfileManager::ProfileManager(){
    ;
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
ProfileMap ProfileManager::loadProfilesFromFile(){
    std::string profilename,follower,followers;
    std::ifstream file;
    ProfileMap loadedProfiles;
    file.open(DEFAULT_PROFILES_FILE);
    while (file.good())
    {
        std::getline(file, profilename, ':');
        char c;
        file.get(c);
        if(c == ' ')
        {
            std::getline(file, followers);
            std::istringstream iss(followers);
            do {
                loadedProfiles[profilename].addFollower(follower);
            }while(std::getline(iss, follower, ' '));
        } 
    }
    return loadedProfiles;
}
Profile ProfileManager::getProfile(std::string profileName){
    return this->profiles[profileName];
}
void ProfileManager::saveAll(){
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
int main(){

    Profile nibsprofile("nibs"), smurfprofile("smurf"), otherprofile("other");
    nibsprofile.addFollower(otherprofile.getName());
    ProfileManager manager;
    ProfileMap profiles;
    profiles=manager.loadProfilesFromFile();
    manager.setProfiles(profiles);
    manager.printProfiles();
}