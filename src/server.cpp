#include "server.hpp"
#include "communication_manager.hpp"
#include "constants.hpp"
#include "notification.hpp"
#include <signal.h>
//define the static variables
std::atomic<bool> UDPServer::stop_issued;
int UDPServer::message_history;
int UDPServer::server_socket;
std::map<std::string, pthread_t> UDPServer::connection_handler_threads;
ProfileManager UDPServer::profileManager; //
RW_Monitor UDPServer::threads_monitor;
pthread_mutex_t mtx;


UDPServer::UDPServer() {
        if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            perror("ERROR opening socket");
        this->server_address.sin_family = AF_INET;
        this->server_address.sin_port = htons(SERVER_PORT);
        this->server_address.sin_addr.s_addr = INADDR_ANY;
        bzero(&(this->server_address.sin_zero), 8);

        if (bind(server_socket, (struct sockaddr *)&this->server_address, sizeof(struct sockaddr)) < 0)
            perror("ERROR on binding");
        this->notificationManager = NotificationManager();
        profileManager = ProfileManager();
        pthread_mutex_init(&mtx, NULL);
        UDPServer::stop_issued.store(false);
}
void UDPServer::run() {
    
    // Create struct for socket timeout and set USER_TIMEOUT seconds
    //struct timeval timeout_timer;
    //timeout_timer.tv_sec = USER_TIMEOUT;
    //timeout_timer.tv_usec = 0;


    // Spawn thread for listening to administrator commands
    //pthread_create(&command_handler_thread, NULL, handleCommands, NULL);
    // Wait for incoming connections
    bool first_connection = true;
    int sockaddr_size = sizeof(struct sockaddr_in);
    while(!stop_issued)
    {   
        threads_monitor.requestRead();
        int connectionHandlerThreadsQuantity = connection_handler_threads.size();
        threads_monitor.releaseRead();

        if(first_connection || profileManager.getSessionsAddr().size() != connectionHandlerThreadsQuantity)
        {    
            first_connection = false;
            // Start new thread for new connection
            pthread_t comm_thread;
            // Spawn new thread for handling that client
            std::cout<<"thread create"<<std::endl;
            if (pthread_create( &comm_thread, NULL, handleConnection, NULL) < 0)
            {
                // Close socket if no thread was created
                std::cerr << "Could not create thread for client "  << std::endl;
            }

            // Request read rights
            pthread_mutex_lock(&mtx);
            ProfileManager::profiles_monitor.requestRead();  
            //Take unique key that is on sessionAddr but isnt on connection_handler_threads
            SessionAddressMap sessionsAddress = profileManager.getSessionsAddr();
            ProfileManager::profiles_monitor.releaseRead(); 
            std::string uniqueKey;
            for (const auto& pair : sessionsAddress) {
                const std::string& key = pair.first;
                // Check if the key is not present in connection_handler_threads
                if (connection_handler_threads.find(key) == connection_handler_threads.end()) {
                    uniqueKey = key;
                }
            }
            //TODO fix the leak of threads
   
            threads_monitor.requestWrite();
            // Add thread to list of connection handlers
            connection_handler_threads.insert(std::make_pair(uniqueKey, comm_thread));

            // Release write rights
            threads_monitor.releaseWrite();
            
        }
    }
    // Request read rights
    threads_monitor.requestRead();

    // Socket associated with each thread
    std::string thread_socket;

    // Wait for all threads to finish
    for (std::map<std::string, pthread_t>::iterator i = connection_handler_threads.begin(); i != connection_handler_threads.end(); ++i)
    {

        std::cout << "Waiting for client communication to end on socket " << i->first << "..." << std::endl;
        // Get the thread reference
        pthread_join(i->second, NULL);

        // Save the thread number
        thread_socket = i->first;
        
        // Remove this ended thread from the thread list
        connection_handler_threads.erase(thread_socket);
    }

    std::cout << "Waiting for command handler to end..." << std::endl;

    // Join with the command handler thread
    pthread_join(command_handler_thread, NULL);

    // Release read rights
    threads_monitor.releaseRead();
}
void *UDPServer::handleCommands(void* arg)
{
    // Get administrator commands until Ctrl D is pressed or 'stop' is typed
    std::string command;
    std::string argument;
    while(!stop_issued && std::getline(std::cin, command, ' ')){
        std::getline(std::cin, argument, ' ');
        //if the command exists
    }
    // Signal all other threads to end
    if (!stop_issued) UDPServer::stop();
    pthread_exit(NULL);
}
void UDPServer::stop()
{
    ProfileManager::profiles_monitor.requestRead();
    profileManager.saveAll();
    ProfileManager::profiles_monitor.releaseRead();

    // Issue a stop command to all running threads
    UDPServer::stop_issued = true;

    // Request read rights
    UDPServer::threads_monitor.requestRead();

    // Stop all communication with clients
    close(server_socket);
 
    // Release read rights
    UDPServer::threads_monitor.releaseRead();

}
int getSessions(){return 1;}

void *UDPServer::handleConnection(void* arg)
{
    std::string profilename;                    // Name of client
    int            read_bytes = -1;             // Number of bytes read from the message
    char           client_message[PACKET_MAX];  // Buffer for client message, maximum of PACKET_MAX bytes
    char           server_message[PACKET_MAX];  // Buffer for messages the server will send to the client, max PACKET_MAX bytes
    Notification*  command_message;             // Buffer for client login information
    std::string    message;                     // User chat message
    Notification* read_message;
    struct sockaddr_in client_address;
    int read_messages = 0;
    int offset = 0;
    std::cout<<"handle"<<std::endl;

    Profile* profile  = NULL; // User the client is connected as
    while((read_bytes = CommunicationManager::receivePacket(server_socket, client_message, &(client_address))) > 0)
    {
        // Decode received data into a packet structure
        Packet* received_packet = (Packet *)client_message;
        // Decide action according to packet type
        switch (received_packet->type)
        {
            case PAK_DATA:   // Data packet

                // Decode received message into a message record
                read_message = (Notification*)received_packet->_payload;
                profilename = std::string(read_message->sender);
                std::cout<<"querendo mandar para followers de "<<profilename<<std::endl;
                profile = new Profile(profilename); //allocate memory
                //request profiles reaD?
                if (profileManager.profileExists(profilename))
                    *profile = profileManager.getProfile(profilename);
                // If profile still exist
                if (profile != NULL && !stop_issued)
                {
                    // Say message to the followers
                    profileManager.sayToFollowers(server_socket, profilename, read_message->_string, PAK_DATA);
                }
                
                break;

            case PAK_COMMAND:   // Command packet (login)

                // Get profile login information
                command_message = (Notification*)received_packet->_payload; 
                if(command_message->type == LOGIN_MESSAGE)
                    if(getSessions () < 2){
                        //do the login process
                        profilename = std::string(command_message->_string);
                        ProfileManager::profiles_monitor.requestWrite();
                        if(!(profileManager.profileExists(profilename))){
                            Profile profile = Profile(profilename);
                            profileManager.addProfile(profile);
                        }
                        profileManager.addSession(profilename,client_address);
                        ProfileManager::profiles_monitor.releaseWrite();
                        pthread_mutex_unlock(&mtx);

                    }
                    else{
                        message = "Connection was refused: exceeds MAX_SESSIONS (" + std::to_string(MAX_SESSIONS) + ")";
                        // Compose message record
                        read_message = NotificationManager::composeMessage(profilename, message, profile->getFollowers().size(), PAK_SERVER_MESSAGE);                        
                        // Send message record to client
                        CommunicationManager::sendPacket(server_socket, PAK_COMMAND, (char*)read_message, sizeof(*read_message) + read_message->length, client_address);
                        // Reject connection
                        close(server_socket);
                        // Free received argument pointer
                        free(arg);
                        // Free composed message record
                        free(read_message);
                        // Request write rights
                        threads_monitor.requestWrite();
                        // Remove itself from the threads list
                        connection_handler_threads.erase(command_message->sender);
                        // Release write rights
                        threads_monitor.releaseWrite();
                        // Exit
                        pthread_exit(NULL);
                    }
                if (command_message->type == FOLLOW_MESSAGE){
                    std::string sender = command_message->sender;
                    std::string follower = command_message->_string;
                    profileManager.follow(sender, follower);
                }
                if(command_message->type == LIST_PROFILES_MESSAGE){
                    profileManager.printProfiles();
                }
                if(command_message->type == LOGOUT_MESSAGE){
                    std::cout << "LOGOUT " <<  std::endl;
                    // Request write rights
                    threads_monitor.requestWrite();

                    // Remove itself from the threads list
                    connection_handler_threads.erase(profilename);

                    // Release write rights
                    threads_monitor.releaseWrite();
                    return NULL;
                }
                break;
            default:
                std::cout << "Unkown packet received from socket at " <<  server_socket << std::endl;
                break;
        }

        // Clear the buffers
        //bzero((void*)client_message, PACKET_MAX);
        //bzero((void*)message_records, PACKET_MAX * Server::message_history);
    }

    // Free received argument
    free(arg);
    free(profile);
    if (!stop_issued)
    {
        // Request write rights
        threads_monitor.requestWrite();

        // Remove itself from the threads list
        connection_handler_threads.erase(profilename);

        // Release write rights
        threads_monitor.releaseWrite();
    }
    ProfileManager::profiles_monitor.requestRead();
    profileManager.saveAll();
    ProfileManager::profiles_monitor.releaseRead();
    // Exit
    pthread_exit(NULL);
}

UDPServer::~UDPServer() {
        profileManager.saveAll();
        close(server_socket);
    }
