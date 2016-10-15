#ifndef _NODE_H
#define _NODE_H

typedef unsigned long node_id_t;

struct node {
    node_id_t id;
    void *meta;
    struct node *(*succ)(struct node *);
    struct node *(*pred)(struct node *);
    struct node *(*find_succ)(struct node *, node_id_t);
    struct node *(*find_pred)(struct node *, node_id_t);
    struct node *(*closest_preceding_finger)(struct node *, node_id_t);
    void (*join)(struct node *, struct node *);
    void (*notify)(struct node *, struct node *);
    void (*stabilize)(struct node *);
    void (*fix_fingers)(struct node *);
};

#endif // _NODE_H
