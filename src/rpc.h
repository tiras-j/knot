#ifndef _RPC_H
#define _RPC_H

#include "node.h"
#include "networking.h"


#define RPC_SUCC 1
#define RPC_PRED 2
#define RPC_FIND_SUCC 3
#define RPC_FIND_PRED 4
#define RPC_CLOS_PREC_FINGER 5
#define RPC_NOTIFY 6

// Outgoing
int rpc_make_call(struct connection *, int message_type, node_id_t target);
struct connection *rpc_recv_resp(struct connection *);

void rpc_handle_call(struct node *, struct connection *);
#endif // _RPC_H
