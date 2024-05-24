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
#define PACKET_COUNT 100

long long current_timestamp_ms() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_sec * 1000LL + spec.tv_nsec / 1e6;
}

int main() {
    int sock;
    struct sockaddr_in addr;
    char buffer[1024];
    int packet_count = PACKET_COUNT;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    addr.sin_port = htons(MULTICAST_PORT);

    for (int i = 0; i < packet_count; ++i) {
        long long timestamp = current_timestamp_ms();
        snprintf(buffer, sizeof(buffer), "%lld", timestamp);

        if (sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            perror("sendto");
            exit(1);
        }

        printf("Sent packet %d with timestamp %lld\n", i + 1, timestamp);
        sleep(1); // sleep for 1 second between packets
    }

    close(sock);
    return 0;
}
