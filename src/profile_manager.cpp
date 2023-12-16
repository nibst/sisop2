#include "profile_manager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

ProfileManager::ProfileManager(){
    ;
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
ProfileMap ProfileManager::loadProfilesFromFile(){
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