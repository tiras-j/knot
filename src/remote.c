/* Definitions for remote node */

#include "node.h"
#include "networking.h"
#include "rpc.h"

struct node *succ_remote(struct node *self);
struct node *pred_remote(struct node *self);
struct node *find_pred_remote(struct node *self, id_t id);
struct node *find_succ_remote(struct node *self, id_t id);
struct node *closest_preceding_finger_remote(struct node *self, id_t id);
void join_remote(struct node *self, struct node *peer);
void notify_remote(struct node *self, struct node *peer);
void stabilize_remote(struct node *self);
void fix_fingers_remote(struct node *self);
void free_remote(struct node *self);

static struct remote_meta {
    int fd;
    char *ip;
    char *port;
};

static struct node cookie_cutter {
    0, // id
    0, // meta
    succ_remote,
    pred_remote,
    find_succ_remote,
    find_pred_remote,
    closest_preceding_finger_remote,
    join_remote,
    notify_remote,
    stabilize_remote,
    fix_fingers_remote,
    free_remote
};

struct node *create_remote(node_id_t id, int fd)
{
    struct remote_meta *m = malloc(sizeof(*m));
    struct node *n = malloc(sizeof(*n));
    if(!m || !n) {
        if(m) free(m);
        if(n) free(n);
        return NULL;
    }

    // Copy function pointers
    memcpy(n, &cookie_cutter, sizeof(cookie_cutter));

    // Set up meta object
    m->fd = fd;
    set_peer_name(m);

    // Set node data
    n->id = id;
    n->meta = m;

    return n;
}

// Explicitly some functions are not applicable
// to remote nodes.
void fix_fingers_remote(struct node *self)
{
    return;
}

void stabilize_remote(struct node *self)
{
    return;
}

void join_remote(struct node *self)
{
    return;
}

// The remaining functions simply execute
// the associated RPC call
struct node *succ_remote(struct node *self)
{
    
