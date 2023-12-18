#include "./include/profile_manager.hpp"
#include "./include/profile.hpp"
int main(){

    ProfileManager manager;
    {Profile profile = Profile("nib");
    manager.addProfile(profile);
    }
    manager.printProfiles();
}