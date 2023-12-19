#pragma once
#include "profile.hpp"
#include "RW_monitor.hpp"
#include <map>

#define DEFAULT_PROFILES_FILE "./profiles.txt"
typedef std::map<std::string, std::vector<int>> SessionStateMap; //profilename to session states (vector[i] == {1,2} if session 1 and 2 is active)
typedef std::map<std::string, struct sockaddr_in> SessionAddressMap; //profilename#session to sockaddr
typedef std::map<std::string,Profile> ProfileMap; //profilename to profile
class ProfileManager{
private:
    ProfileMap profiles;                //maped by profilename
    SessionAddressMap sessionAddresses; //maped by profilename#i
    SessionStateMap sessionStates;      //maped by profilename
public:
    static RW_Monitor profiles_monitor;

    ProfileManager();
    void sayToFollowers(int socket, std::string senderName, std::string message_content, int packetType);
    int addSession(std::string username, struct sockaddr_in sessionAddress);
    int deleteSession(std::string username, int sessionNumber);
    bool profileExists(std::string profilename);
    void follow(std::string user, std::string followedUser);
    Profile getProfile(std::string profileName);
    void setProfiles(ProfileMap profiles);
    SessionAddressMap getSessionsAddr();
    SessionStateMap getSessionsState();
    ProfileMap getAllFollowersFromFile();
    void addProfile(Profile profile);
    void saveAll();
    void printProfiles();

};
