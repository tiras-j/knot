#ifndef _NETWORKING_H
#define _NETWORKING_H

struct connection {
    char *ip;
    char *port;
    int fd;
};

struct message {
    char *data;
    int len; // use int here for htonl compat
};

int connect_to_peer(char *addr, char *port);
int bind_local(char *port);
void set_peer_name(struct connection *);

int send_message(struct connection *, struct message *);
struct message *recv_message(struct connection *);

#endif // _NETWORKING_H
