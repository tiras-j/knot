#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "networking.h"

#define PORT_STR_LEN 6

int connect_to_peer(char *addr, char *port)
{
    struct addrinfo hints, *res, *p;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(addr, port, &hints, &res) != 0)
        return -1;

    // Try each address, return first successful connection
    for(p = res; p != NULL; p = p->ai_next) {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
            close(sockfd);
    }

    freeaddrinfo(res); // free the linked list

    if(sockfd == -1)
        return -1;

    fprintf(stderr, "Returning socket fd: %d\n", sockfd);
    return sockfd;
}

int set_nonblocking(int fd)
{
    if(fcntl(fd, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl");
        close(fd);
        return -1;
    }
    return 0;
}

int bind_local(char *port)
{
    struct addrinfo hints, *p, *res;
    int sockfd, yes = 1;

    // first, load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    getaddrinfo(NULL, port, &hints, &res);
    for(p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            return -1;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);

    if(listen(sockfd, 10) == -1) {
        perror("listen");
        close(sockfd);
        return -1;
    }

    fprintf(stderr, "Returning socket fd: %d\n", sockfd);
    return sockfd;
}

void set_peer_name(struct connection *p)
{
    struct sockaddr_storage addr;
    socklen_t len = sizeof(struct sockaddr_storage);

    if(p->ip == NULL)
        p->ip = malloc(INET6_ADDRSTRLEN);
    if(p->port == NULL)
        p->port = malloc(PORT_STR_LEN);

    if(!p->ip || !p->port) {
        fprintf(stderr, "Failed to malloc peer name\n");
        return;
    }

    if(getpeername(p->fd, (struct sockaddr*)&addr, &len) == -1){
        fprintf(stderr, "Errno: %d\n", errno);
        perror("passed function");
        return;
    }

    if(addr.ss_family == AF_INET){
        fprintf(stderr, "Family is AF_INET\n");
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        snprintf(p->port, PORT_STR_LEN, "%d", ntohs(s->sin_port));
        inet_ntop(AF_INET, &s->sin_addr, p->ip, INET6_ADDRSTRLEN);
    } else {
        fprintf(stderr, "Family is NOT AF_INET\n");
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        snprintf(p->port, PORT_STR_LEN, "%d", ntohs(s->sin6_port));
        inet_ntop(AF_INET6, &s->sin6_addr, p->ip, INET6_ADDRSTRLEN);
    }
}

int send_message(struct connection *c, struct message *m)
{
    int ret, msglen, bytes_sent = 0;

    msglen = htonl(m->len);
    // First deliver size of data
    while(bytes_sent < sizeof msglen) {
        ret = send(c->fd, ((char*)&msglen) + bytes_sent, sizeof(msglen) - bytes_sent, 0);
        if(ret == -1 && (errno != EAGAIN || errno != EWOULDBLOCK))
            return -1;
        if(ret > 0)
            bytes_sent += ret;
    }

    // Now deliver the data
    bytes_sent = 0;
    while(bytes_sent < m->len) {
        ret = send(c->fd, m->data + bytes_sent, m->len - bytes_sent, 0);
        if(ret == -1 && (errno != EAGAIN || errno != EWOULDBLOCK))
            return -1;
        if(ret > 0)
            bytes_sent += ret;
    }

    return 0;
}

struct message *recv_message(struct connection *c)
{
    int ret, msglen, bytes_recv = 0;
    struct message *m;

    while(bytes_recv < sizeof msglen) {
        ret = recv(c->fd, ((char*)&msglen) + bytes_recv, sizeof(msglen) - bytes_recv, 0);
        if(ret == -1 && (errno != EAGAIN || errno != EWOULDBLOCK))
            return NULL;
        if(ret == 0) {
            close(c->fd);
            return NULL;
        }

        if(ret > 0)
            bytes_recv += ret;
    }

    msglen = ntohl(msglen);
    if((m = malloc(sizeof(*m))) == NULL)
        return NULL;

    m->len = msglen;
    if((m->data = malloc(m->len)) == NULL){
        free(m);
        return NULL;
    }

    bytes_recv = 0;
    while(bytes_recv < m->len) {
        ret = recv(c->fd, m->data + bytes_recv, m->len - bytes_recv, 0);
        if(ret == -1 && (errno != EAGAIN || errno != EWOULDBLOCK))
            return NULL;
        if(ret == 0) {
            close(c->fd);
            return NULL;
        }

        if(ret > 0)
            bytes_recv += ret;
    }

    return m;
}
