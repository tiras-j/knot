/* Functions necessary for rpc calls */

#include "rpc.h"
#include <endian.h>

#define RPC_SEND_SIZE 12
int rpc_send(struct connection *c, struct rpc_message *m)
{
    if(c->w_cap == 0) {
        if((c->write_buf = malloc(BASE_BUF_SIZE)) == NULL)
            return -1;
        c->w_cap = BASE_BUF_SIZE;
        c->w_size = 0;
    }

    if((c->w_cap - c->w_size) < RPC_SEND_SIZE) {
        char *temp = c->write_buf;
        if((c->write_buf = malloc(2 * c->w_cap)) == NULL) {
            c->write_buf = temp;
            return -1;
        }
        memcpy(c->write_buf, temp, c->w_cap);
        free(temp);
        c->w_cap *= 2;
    }

    memcpy(c->write_buf + c->w_size, htobe32(m->request), sizeof(m->request));
    c->w_size += sizeof(m->request);
    memcpy(c->write_buf + c->w_size, htobe64(m->target_id), sizeof(m->target_id));
    c->w_size += sizeof(m->target_id);

    return network_send(c);
}

struct rpc_response *rpc_recv(struct connection *c)
{
    struct rpc_response *r;
    if(network_recv(c, 1))
        return NULL;

    if((r = malloc(sizeof(*r))) == NULL)
        return NULL

    
