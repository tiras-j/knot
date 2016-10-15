/* Definitions for local node */
#include <stdlib.h>
#include "node.h"

#define FINGER_TBL_SIZE 16

struct finger {
    node_id_t start;
    struct node *n;
};

struct local_meta {
    struct node *pred;
    struct finger *finger_table;
    size_t table_sz;
};

struct node *succ_local(struct node *self);
struct node *pred_local(struct node *self);
struct node *find_pred_local(struct node *self, id_t id);
struct node *find_succ_local(struct node *self, id_t id);
struct node *closest_preceding_finger_local(struct node *self, id_t id);
void join_local(struct node *self, struct node *peer);
void notify_local(struct node *self, struct node *peer);
void stabilize_local(struct node *self);
void fix_fingers_local(struct node *self);

struct node cookie_cutter = {
    0,  // id
    0,  // meta
    succ_local,
    pred_local,
    find_succ_local,
    find_pred_local,
    find_closest_preceding_local,
    join_local,
    notify_local,
    stabilize_local,
    fix_fingers_local
};

void init_empty_finger_table(struct node *n)
{
    // We initialize all fingers to point to ourselves
    // but we set up "start" interval for each finger
    // entry to the proper value
    struct local_meta *m = n->meta;
    node_id_t step = 1;
    int i = 0, interval = (sizeof(node_id_t) * 8) / m->table_sz;

    for(; i < m->table_sz; i++) {
        m->finger_table[i].start = n->id + (step << (i * interval));
        m->finger_table[i].node = n;
    }
}

struct node *create_local(id_t id)
{
    struct node *n = malloc(sizeof(*n));
    struct local_meta *m = malloc(sizeof(*m));
    m->finger_table = calloc(FINGER_TBL_SIZE, sizeof(*m->finger_table));

    if(!n || !m || !m->finger_table) {
        if(m->finger_table) free(m->finger_table);
        if(m) free(m);
        if(n) free(n);
        fprintf(stderr, "Failed to allocate memory for node\n");
        return NULL;
    }

    // Set up function pointers
    memcpy(n, &cookie_cutter, sizeof(cookie_cutter));

    n->id = id;
    n->meta = m;

    // Set up initial finger table
    init_empty_finger_table(n);

    return n;
}

struct node *succ_local(struct node *self)
{
    struct local_meta *m = self->meta;
    return m->finger_table[0].node;
}

struct node *pred_local(struct node *self)
{
    struct local_meta *m = self->meta;
    return m->pred;
}

struct node *find_pred_local(struct node *self, node_id_t id)
{
    struct node *target = self;
    while(!within_interval_ri(id, target->id, target->succ(target)->id)) {
        target = target->closest_preceding_finger(target, id);
    }
    return target;
}

struct node *find_succ_local(struct node *self, node_id_t id)
{
    struct node *target = self->find_pred(self, id);
    return target->succ(target);
}

struct node *closest_preceding_finger_local(struct node *self, node_id_t id)
{
    struct local_meta *m = self->meta;
    int i = m->table_sz;
    struct node *target;

    for(; i > 0; i--) {
        if(within_interval_ex(m->finger_table[i].node->id, self->id, id))
            return m->finger_table[i].node;
    }
    return self;
}

void join_local(struct node *self, struct node *peer)
{
    struct local_meta *m = self->meta;
    m->pred = NULL;
    m->finger_table[0].node = peer->find_succ(peer, self->id);
}

void notify_local(struct node *self, struct node *peer)
{
    struct local_meta *m = self->meta;
    if(!m->pred || within_interval_ex(peer->id, m->pred->id, self->id)) {
        free_node(m->pred);
        m->pred = peer;
    }
}

void stabilize_local(struct node *self)
{
    struct local_meta *m = self->meta;
    struct node *x = m->finger_table[0].node.pred(m->finger_table[0].node);
    if(within_interval_ex(x->id, self->id, m->finger_table[0].node->id)) {
        free_node(m->finger_table[0].node);
        m->finger_table[0].node = x;
    }
    m->finger_table[0].node->notify(m->finger_table[0].node);
}

void fix_fingers_local(struct node *self)
{
    struct local_meta *m = self->meta;
    int idx = (rand() % (m->table_sz - 1)) + 1;
    free_node(m->finger_table[idx].node);
    m->finger_table[idx].node = self->find_succ(self, m->finger_table[idx].start);
}
