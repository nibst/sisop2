#include <ctime>
#include <iostream>
#include <vector>


class Profile {
public:
    Profile(const char* name, const char* password);
    const char* getName() const;
    const char* getPassword() const;
    void addFollower(int followerId);

private:
    const char* name; ;//will be used as identifier
    const char* password;
    std::vector<int> followers;
};
typedef struct{
    int id;
    Profile *author;
    time_t time_of_notification;
    int char_count;
    int remaining_followers; //number of followers that didnt reeceived the notification
}Notification;
