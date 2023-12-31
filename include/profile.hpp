#pragma once
#include <ctime>
#include <iostream>
#include <vector>
#include <netinet/in.h>
#include "notification.hpp"
#define DEFAULT_PROFILE_NAME "_default_name___"

class Profile {
public:
    Profile();
    Profile(std::string name);
    std::string getName() const;
    void addFollower(std::string follower);
    void addNotification(Notification notification);
    std::vector<std::string> getFollowers();
    void loadFollowersFromFile();

private:
    //std::vector<Notification> notifications_history; // 
    std::string name;                       // will be used as identifier
    std::vector<std::string> followers;     // All followers of this profile
};

