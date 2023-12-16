#include "client.hpp"
UDPClient::UDPClient(const char *hostname) {
        server = gethostbyname(hostname);
        if (server == NULL) {
            fprintf(stderr, "ERROR, no such host\n");
            exit(0);
        }

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            perror("ERROR opening socket");

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
        bzero(&(serv_addr.sin_zero), 8);
    }
void UDPClient::sendMessage() {
        printf("Enter the message: ");
        bzero(buffer, 256);
        fgets(buffer, 256, stdin);

        int n = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
        if (n < 0)
            perror("ERROR sendto");

        length = sizeof(struct sockaddr_in);
        n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *)&from, &length);
        if (n < 0)
            perror("ERROR recvfrom");

        printf("Got an ack: %s\n", buffer);
    }
UDPClient::~UDPClient() {
        close(sockfd);
    }

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage %s hostname\n", argv[0]);
        exit(0);
    }
    UDPClient client(argv[1]);
    while(true) {
    client.sendMessage();
    }
    return 0;
}