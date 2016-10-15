#ifndef _NETWORKING_H
#define _NETWORKING_H

struct connection {
    int fd;

    char *read_buf;
    size_t r_cap;
    size_t r_size;
    size_t r_pending;

    char *write_buf;
    size_t w_cap;
    size_t w_size;
};

#endif // _NETWORKING_H
