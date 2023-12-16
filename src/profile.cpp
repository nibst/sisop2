#include "profile.hpp"
Profile::Profile():name(DEFAULT_PROFILE_NAME){;}
Profile::Profile(std::string name) : name(name) {;}
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
