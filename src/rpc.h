#ifndef _RPC_H
#define _RPC_H

#include "node.h"
#include "networking.h"

#define R_SUCC 0
#define R_PRED 1
#define R_FIND_SUCC 2
#define R_FIND_PRED 3
#define R_CLOSEST_PRECEDING 4
#define R_NOTIFY 5

struct rpc_message {
    node_id_t target_id;
    uint32_t request;
};

struct rpc_response {
    node_id_t node_id;
    char *node_ip;
    char *node_port;
    uint32_t request;
};

#endif // _RPC_H
