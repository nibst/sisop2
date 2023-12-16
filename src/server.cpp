#include "server.hpp"

UDPServer::UDPServer() {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            perror("ERROR opening socket");
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        bzero(&(serv_addr.sin_zero), 8);

        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
            perror("ERROR on binding");

        clilen = sizeof(struct sockaddr_in);
    }
void UDPServer::run() {
        while (1) {
            /* receive from socket */
            int n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *)&cli_addr, &clilen);
            if (n < 0)
                perror("ERROR on recvfrom");
            printf("Received a datagram: %s\n", buf);

            /* send to socket */
            n = sendto(sockfd, "Got your message\n", 17, 0, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
            if (n < 0)
                perror("ERROR on sendto");
        }
    }
UDPServer::~UDPServer() {
        close(sockfd);
    }
int main() {
    UDPServer server;
    server.run();

    return 0;
}
