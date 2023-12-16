#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 4000

class UDPClient {
private:
    int sockfd;
    struct sockaddr_in serv_addr, from;
    unsigned int length;
    char buffer[256];
	struct hostent *server;

public:
    UDPClient(const char *hostname);
    void sendMessage();
    ~UDPClient();
};
