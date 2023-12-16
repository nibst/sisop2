#pragma once
#include <ctime>
#include <iostream>
#include <vector>
#define DEFAULT_PROFILE_NAME "_default_name___"
class Profile {
public:
    Profile();
    Profile(std::string name);
    std::string getName() const;
    void addFollower(std::string follower);
    std::vector<std::string> getFollowers();

private:
    std::string name; ;//will be used as identifier
    std::vector<std::string> followers;
};

