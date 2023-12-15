#include "profile.hpp"

Profile::Profile(const char* name, const char* password) : name(name), password(password) {;}
const char* Profile::getName() const{
    return this->name;
}
const char* Profile::getPassword() const{
    return this->password;
}
void Profile::addFollower(int followerId){
    this->followers.push_back(followerId);
}


