#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define MULTICAST_GROUP "239.0.0.1"
#define MULTICAST_PORT 12345

long long current_timestamp_ms() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_sec * 1000LL + spec.tv_nsec / 1e6;
}

int main() {
    int sock;
    struct sockaddr_in addr;
    struct ip_mreq mreq;
    char buffer[1024];

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(MULTICAST_PORT);

    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    while (1) {
        int nbytes = recvfrom(sock, buffer, sizeof(buffer), 0, NULL, 0);
        if (nbytes < 0) {
            perror("recvfrom");
            exit(1);
        }

        long long received_timestamp = atoll(buffer);
        long long current_time = current_timestamp_ms();
        long long time_diff = current_time - received_timestamp;

        printf("Received timestamp: %lld, Current time: %lld, Time difference: %lld ms\n",
               received_timestamp, current_time, time_diff);
    }

    close(sock);
    return 0;
}
