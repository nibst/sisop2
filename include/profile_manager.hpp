#pragma once
#include "profile.hpp"
#include <map>

#define DEFAULT_PROFILES_FILE "./profiles.txt"
typedef std::map<std::string,Profile> ProfileMap;
class ProfileManager{
private:
    ProfileMap profiles;

public:
    ProfileManager();
    bool profileExists(std::string profilename);
    void follow(std::string user, std::string followedUser);
    Profile getProfile(std::string profileName);
    void setProfiles(ProfileMap profiles);
    ProfileMap loadProfilesFromFile();
    void addProfile(Profile profile);
    void saveAll();
    void printProfiles();
};
